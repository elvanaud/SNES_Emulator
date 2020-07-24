#include "W65816.h"
#include "../Bus.h"
#include "Stage.h"
#include "Types.h"

using std::bind;
using namespace std::placeholders;

#include <iostream>
using std::cout;
using std::endl;

W65816::W65816()
{
    initializeAddressingModes();
    initializeOpcodes();
    reloadPipeline();
    pc.set(0xFF);
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

void RESET()
{
    //TODO: reset some flags
    //TODO: reload pipeline with interupt content
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
    switch(st.getEnablingCondition())
    {
        case Stage::SIG_ALWAYS: return true;
        case Stage::SIG_INST: return true;
        case Stage::SIG_MEM16_ONLY: return !p.mem8;
        case Stage::SIG_MODE16_ONLY: if(decodingTable[ir].isIndexRelated()) return !p.index8; else return !p.mem8;
        case Stage::SIG_MODE8_ONLY: if(decodingTable[ir].isIndexRelated()) return p.index8; else return p.mem8;
        case Stage::SIG_DUMMY_STAGE: return true;
        case Stage::SIG_X_CROSS_PAGE: uint8_t op = bus->privateRead(pc.val()); return op > op+x.low;
        case Stage::SIG_Y_CROSS_PAGE: uint8_t op = bus->privateRead(pc.val()); return op > op+y.low;
    }
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

void W65816::tick()
{
    handleValidAddressPINS(ValidAddressState::InternalOperation);
    for(auto &stage : pipeline[tcycle])
    {
        stage(this);
    }
    processSignals();
    //checkInterupts();
    ++tcycle;

    if(tcycle >= pipeline.size())
    {
        reloadPipeline();
    }
}
