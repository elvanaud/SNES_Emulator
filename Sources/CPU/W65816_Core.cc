#include "W65816.h"
#include "../Bus.h"
#include "Stage.h"
#include "Types.h"

using std::bind;
using namespace std::placeholders;

#include <iostream>
#include<cassert>
using std::cout;
using std::endl;

W65816::W65816()
{
    initializeAddressingModes();
    initializeAdrModeASMDecode();
    initializeOpcodes();
    reloadPipeline();
    pc.set(0xFF); //Becomes useless

    interuptIRQ     = Instruction("<interupt IRQ>", StackInterupt, IRQ);
    interuptNMI     = Instruction("<interupt NMI>", StackInterupt, NMI);
    interuptRESET   = Instruction("<interupt RST>", StackInterupt, RESET);

    triggerRESET();
}

void W65816::attachBus(Bus * b)
{
    bus = b;
}

// ------- Getters ----------
uint8_t W65816::getP()
{
    return p.getVal();
}

uint16_t W65816::getPC()
{
    return pc.val();
}

uint32_t W65816::getFullPC()
{
    return (uint32_t(pbr)<<16)|pc.val();
}

uint16_t W65816::getAcc()
{
    return acc.val();
}

uint16_t W65816::getAdr()
{
    return adr.val();
}

uint16_t W65816::getIDB()
{
    return idb.val();
}

uint16_t W65816::getS()
{
    return s.val();
}

uint16_t W65816::getD()
{
    return d.val();
}

uint8_t W65816::getDBR()
{
    return dbr;
}

string W65816::getPString()
{
    return p.toString();
}

uint8_t W65816::getIR()
{
    return ir;
}

unsigned int W65816::getTCycle()
{
    return tcycle;
}

uint32_t W65816::getAddressBus()
{
    return addressBusBuffer;
}

uint16_t W65816::getX()
{
    return x.val();
}

uint16_t W65816::getY()
{
    return y.val();
}

Instruction & W65816::getInst()
{
    return decodingTable[ir];
}

// ---------- PINS ------------------
bool W65816::VDA()
{
    return vda;
}

bool W65816::VPA()
{
    return vpa;
}

bool W65816::E()
{
    return p.emulationMode;
}

bool W65816::M()
{
    return p.M();
}

bool W65816::X()
{
    return p.X();
}

bool W65816::RDY()
{
    return rdy;
}

void W65816::triggerRDY(bool status)
{
    rdy = status;
}

void W65816::triggerRESET()
{
    internalRST = true;
    executeInterupt = true; //Triggers RST directly (useful for booting the cpu (else it would execute an instruction which I guess isn't the appropriate behavior))
}

void W65816::triggerNMI()
{
    internalNMI = true;
}

void W65816::triggerIRQ()
{
    internalIRQ = true;
}

void W65816::IRQ()
{
    uint16_t vecAdr = 0xFFEE;
    if(p.emulationMode) vecAdr = 0xFFFE;
    internalIRQ = false; //TODO: maybe will have to reset that on each cycle as described in the docs
    adr.set(vecAdr);
    --pc;
}

void W65816::NMI()
{
    uint16_t vecAdr = 0xFFEA;
    if(p.emulationMode) vecAdr = 0xFFFA;
    internalNMI = false;
    adr.set(vecAdr);
    --pc;
}

void W65816::RESET()
{
    internalRST = false;
    adr.set(0xFFFC);
    //--pc; //TODO: Remove that ??

    s.high = 0x01;
    s.low = 0xFF; 
    d.set(0);
    x.set(0);
    y.set(0);
    pbr = 0;
    dbr = 0;

    p.setE(true); //Also sets the mem and index to 8 bit
}

void W65816::setReg(Register16 & r, uint16_t v)
{
    if((r.isIndex && p.index8) || (!r.isIndex && p.mem8)) r.low = v & 0xFF;
    else r.set(v);
}

uint16_t W65816::getReg(Register16 & r)
{
    if((r.isIndex && p.index8) || (!r.isIndex && p.mem8)) return r.low;
    return r.val();
}

void W65816::updateNZFlags(uint16_t v, bool indexValue, bool force16)
{
    unsigned int offset = 7;
    if((!indexValue && !p.mem8) || (indexValue && !p.index8) || force16) offset = 15;

    p.setN((v>>offset)&1);

    if(offset == 15) v &= 0xFFFF;
    else v &= 0xFF;

    p.setZ(v == 0);
}

void W65816::updateStatusFlags(uint32_t v, bool indexValue)
{
    unsigned int offset = 7;
    if((!indexValue && !p.mem8) || (indexValue && !p.index8)) offset = 15;

    p.setC(v>>(offset+1));

    updateNZFlags(v,indexValue);
}

void W65816::checkSignedOverflow(int a, int b, int c)
{
    p.setV((a == 0 && b == 0 && c == 1) || (a == 1 && b == 1 && c == 0));
}

void W65816::reloadPipeline()
{
    pipeline.clear();
    tcycle = 0;

    vector<StageType> T1 = lastPipelineStage; //Be careful to keep the previous inst stage before the new opcode fetch
    T1.push_back(Stage(Stage::SIG_ALWAYS,fetchInc,&pc,&ir).get());
    lastPipelineStage.clear();
    pipeline.push_back(T1);

    vector<StageType> T2 = {Stage(Stage::SIG_ALWAYS,decode).get(), Stage(Stage::SIG_ALWAYS,fetch,&pc,&adr.low).get()}; //Careful to keep the decode before the fetch
    pipeline.push_back(T2);
}

bool W65816::isStageEnabled(Stage const& st)
{
    uint8_t op;
    switch(st.getEnablingCondition())
    {
        case Stage::SIG_ALWAYS: return true;
        case Stage::SIG_INST: return true;
        case Stage::SIG_MEM16_ONLY: return !p.mem8;
        case Stage::SIG_MODE16_ONLY: if(decodingTable[ir].isIndexRelated()) return !p.index8; else return !p.mem8;
        case Stage::SIG_MODE8_ONLY: if(decodingTable[ir].isIndexRelated()) return p.index8; else return p.mem8;
        case Stage::SIG_DUMMY_STAGE: return true;
        case Stage::SIG_X_CROSS_PAGE: op = bus->privateRead(pc.val()); return op > op+x.low;
        case Stage::SIG_Y_CROSS_PAGE: op = bus->privateRead(pc.val()); return op > op+y.low;
        case Stage::SIG_DL_NOT_ZERO: return d.low != 0;
        case Stage::SIG_INDIRECT_Y_CROSS_PAGE_OR_X16: op = bus->privateRead(bus->privateRead(pc.val())+d.val()); return !p.index8 || op > op+y.low;
        case Stage::SIG_PC_CROSS_PAGE_IN_EMUL: op = bus->privateRead(pc.val()); return p.emulationMode && op > op+adr.low;
        case Stage::SIG_NATIVE_MODE: return !p.emulationMode;
        case Stage::SIG_ACC_ZERO: return acc.val()==0;
        case Stage::SIG_ACC_NOT_ZERO: return acc.val()!=0;
    }

    assert(false);
    return true; //Never reached
}

void W65816::handleValidAddressPINS(ValidAddressState state)
{
    switch(state)
    {
        case InternalOperation: vda = vpa = 0; break;
        case OpcodeFetch: vpa = vda = 1; break;
        case OperandFetch: vda = 0; vpa = 1; break;
        case DataFetch: vda = 1; vpa = 0; break;
    }
    if(forceInternalOperation)
    {
        vda = vpa = 0;
        forceInternalOperation = false;
    }
}

void W65816::generateAddress(uint8_t bank, uint16_t adr)
{
    addressBusBuffer = uint32_t(bank)<<16;
    addressBusBuffer |= adr;
}

void W65816::generateAddress(uint16_t adr)
{
    addressBusBuffer = 0;
    if(forceTmpBank) {addressBusBuffer = uint32_t(tmpBank)<<16;}
    else if(vpa) {addressBusBuffer = uint32_t(pbr)<<16;}
    else if(vda) {addressBusBuffer = uint32_t(dbr)<<16;}
    addressBusBuffer |= adr;
}

void W65816::checkInterupts()
{
    //if(tcycle != pipeline.size()) return;
    if(internalIRQ || internalNMI || internalRST) executeInterupt = true;
}

void W65816::tick()
{
    handleValidAddressPINS(ValidAddressState::InternalOperation);
    if(!rdy || clockStopped)
    {
        checkInterupts();
        if(executeInterupt) rdy = true; //TODO: if rdy is manually low maybe shouldn't resume exec on interupt ? (need to differentiate between user rdy and internal rdy
        if(clockStopped && internalRST) clockStopped = false;
        return;
    }

    for(auto &stage : pipeline[tcycle])
    {
        stage(this);
    }
    processSignals();
    ++tcycle;
    if(tcycle == pipeline.size()) checkInterupts();

    if(tcycle >= pipeline.size())
    {
        reloadPipeline();
    }
}
