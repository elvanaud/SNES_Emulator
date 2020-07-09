#include "W65816.h"
#include "Bus.h"
#include "Stage.h"
#include "Types.h"

using std::bind;

#include <iostream>
using std::cout;
using std::endl;

using namespace std::placeholders;

W65816::W65816()
{
    initializeAddressingModes();
    initializeOpcodes();
    reloadPipeline();
    pc.set(0x100);
}

void W65816::initializeAddressingModes()
{
    Immediate.setStages({{Stage(Stage::SIG_MODE16_ONLY,fetchInc,&pc,&idb.high)},{Stage(Stage::SIG_INST,instStage)}});
    Immediate.setSignals({bind(incPC,this),bind(opPrefetchInIDB,this)});
}

void W65816::initializeOpcodes()
{
    decodingTable[0x29] = Instruction("AND", Immediate, AND);
    decodingTable[0x49] = Instruction("EOR", Immediate, EOR);
    decodingTable[0x69] = Instruction("ADC", Immediate, ADC);
    decodingTable[0x89] = Instruction("BIT", Immediate, BIT);
    decodingTable[0xC9] = Instruction("CMP", Immediate, CMP);
    decodingTable[0xE0] = Instruction("CPX", Immediate, CPX); decodingTable[0xE0].setIsIndexRelated(true);
    decodingTable[0xC0] = Instruction("CPY", Immediate, CPY); decodingTable[0xC0].setIsIndexRelated(true);
}

void W65816::instStage()
{
    ; //Dummy
}

uint8_t W65816::getP()
{
    return p.val;
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

void W65816::updateNZFlags(uint16_t v, bool indexValue)
{
    unsigned int offset = 7;
    if((!indexValue && !p.mem8) || (indexValue && !p.index8)) offset = 15;

    p.setN(v>>offset);
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

void W65816::AND()
{
    uint16_t res = getReg(acc) & getReg(idb);
    updateNZFlags(res);
    setReg(acc,res);
}

void W65816::ADC()
{
    uint32_t r = 0;
    if(p.D())
    {
        auto decimalAdd8bit = [](uint8_t a, uint8_t b)
        {
            uint16_t aLeft = a&0xF0;
            uint16_t aRight = a&0x0F;
            uint16_t bLeft = b&0xF0;
            uint16_t bRight = b&0x0F;

            uint32_t res = aRight+bRight;
            if(res >= 0x0A)
                res += 0x06;
            res += aLeft+bLeft;
            if((res>>4) >= 0x0A)
                res += 0x60;

            return res;
        };

        r = decimalAdd8bit(idb.low,acc.low);
        if(!p.mem8) r = decimalAdd8bit(idb.high,acc.high+(r>>8));

        acc.low = r&0xFF;
        if(!p.mem8) acc.high = (r>>8)&0xFF;
    }
    else
    {
        r = getReg(acc)+getReg(idb)+p.C();

        int offset = 7;
        if(!p.mem8) offset = 15;
        int aSign = (acc.val()>>offset)&1;
        int bSign = (idb.val()>>offset)&1;
        int cSign = (r>>offset)&1;
        checkSignedOverflow(aSign,bSign,cSign);

        setReg(acc,r);
    }

    updateStatusFlags(r);
}

void W65816::BIT()
{
    uint16_t v = getReg(idb);
    if(decodingTable[ir].AdrMode().Name() != Immediate.Name())
    {
        p.setN((v>>7)&1);
        p.setV((v>>6)&1);
    }

    uint16_t r = getReg(acc) & v;
    p.setZ(r==0);
}

void W65816::CMP()
{
    uint16_t a = getReg(acc);
    uint16_t b = getReg(idb);
    uint16_t r = a - b;
    updateNZFlags(r);
    p.setC(a >= b);
}

void W65816::CPX()
{
    uint16_t a = getReg(x);
    uint16_t b = getReg(idb);
    uint16_t r = a - b;
    updateNZFlags(r,true);
    p.setC(a >= b);
}

void W65816::CPY()
{
    uint16_t a = getReg(y);
    uint16_t b = getReg(idb);
    uint16_t r = a - b;
    updateNZFlags(r,true);
    p.setC(a >= b);
}

void W65816::EOR()
{
    uint16_t r = getReg(acc)^getReg(idb);
    updateNZFlags(r);
    setReg(acc,r);
}

void W65816::incPC()
{
    if(tcycle == 1) ++pc;
    cout << "incPc" << endl;
}

void W65816::opPrefetchInIDB()
{
    if(tcycle == 1) idb.low = adr.low;
    cout << "opPrefetchInIDB" << endl;
}

void W65816::attachBus(Bus * b)
{
    bus = b;
}

void W65816::reloadPipeline()
{
    pipeline.clear();
    tcycle = 0;

    vector<StageType> T1 = lastPipelineStage; //Be careful to keep the previous inst stage before the new opcode fetch
    T1.push_back(Stage(Stage::SIG_ALWAYS,fetchInc,&pc,&ir).get());
    /*for(StageType & st : lastPipelineStage)
    {
        T1.push_back(st);
    }*/
    lastPipelineStage.clear();
    pipeline.push_back(T1);

    vector<StageType> T2 = {Stage(Stage::SIG_ALWAYS,fetch,&pc,&adr.low).get(), Stage(Stage::SIG_ALWAYS,decode).get()};
    pipeline.push_back(T2);
}

bool W65816::isStageEnabled(Stage const& st)
{
    switch(st.getSignal())
    {
        case Stage::SIG_ALWAYS: return true;
        case Stage::SIG_INST: return true;
        case Stage::SIG_MEM16_ONLY: return !p.mem8;
        case Stage::SIG_MODE16_ONLY: if(decodingTable[ir].isIndexRelated()) return !p.index8; else return !p.mem8;
        default: return true;
    }
    //return true;
}

void W65816::processSignals()
{
    for(auto & sig : decodingTable[ir].Signals())
    {
        sig();
    }
}

void W65816::decode()
{
    const auto & instructionStages = decodingTable[ir].Stages();
    for(unsigned int i = 0; i < instructionStages.size(); ++i)
    {
        const auto & stagesCycleN = instructionStages[i];
        vector<StageType> pipelineCycleN;
        bool stageGroupEmpty = true;
        for(const Stage & stage : stagesCycleN)
        {
            if(isStageEnabled(stage))
            {
                stageGroupEmpty = false;
                pipelineCycleN.push_back(stage.get());
            }
        }
        if(!stageGroupEmpty)
        {
            if(i == instructionStages.size()-1) lastPipelineStage = pipelineCycleN;
            else pipeline.push_back(pipelineCycleN);
        }
    }
}

bool W65816::VDA()
{
    return vda;
}

bool W65816::VPA()
{
    return vpa;
}

void W65816::dummyFetch(Register16 *src)
{
    vda = vpa = 0;
    bus->read(src->val());
}

void W65816::fetch(Register16 *src, uint8_t * dst)
{
    if(src == &pc){ vpa = true; vda = tcycle == 0; }
    else{ vda = true; vpa = false; }
    //todo: generateAddressWithBank();
    bus->read(src->val());
    *dst =  bus->DMR();
}

void W65816::fetchInc(Register16 *src, uint8_t * dst)
{
    fetch(src,dst);
    ++(*src);
}

void W65816::fetchDec(Register16 *src, uint8_t * dst)
{
    fetch(src,dst);
    --(*src);
}

void W65816::tick()
{
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
