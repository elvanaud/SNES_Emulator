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
    Immediate.setStages({{Stage(Stage::SIG_MEM16_ONLY,fetchInc,&pc,&idb.high)},{Stage(Stage::SIG_INST,instStage)}});
    Immediate.setSignals({bind(incPC,this),bind(opPrefetchInIDB,this)});

}

void W65816::initializeOpcodes()
{
    decodingTable[0x69] = Instruction("ADC", Immediate, ADC);
}

void W65816::instStage()
{
    ; //Dummy
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

void W65816::ADC()
{
    //check decimal mode
    if(mem8)
    {
        uint16_t res =  (acc.low)+idb.low+p.C();
        //todo:check overflow
        p.setC(res>>8);
        res &= 0xFF;
        p.setZ(res == 0);
        p.setN(res>>7);
        acc.low = res;
    }
    else
    {
        uint32_t res = acc.val()+idb.val()+p.C();
        p.setC(res>>16);
        res &= 0xFFFF;
        p.setZ(res == 0);
        p.setN(res>>15);
        acc.set(res);
    }
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

    vector<StageType> T1 = {Stage(Stage::SIG_ALWAYS,fetchInc,&pc,&ir).get()};
    //T1.push_back(lastStage);
    pipeline.push_back(T1);

    vector<StageType> T2 = {Stage(Stage::SIG_ALWAYS,fetch,&pc,&adr.low).get(), Stage(Stage::SIG_ALWAYS,decode).get()};
    pipeline.push_back(T2);
}

bool W65816::isStageEnabled(Stage &st)
{
    switch(st.getSignal())
    {
        case Stage::SIG_ALWAYS: return true;
        case Stage::SIG_INST: return true;
        case Stage::SIG_MEM16_ONLY: return !mem8;
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
    for(auto &st : decodingTable[ir].Stages())
    {
        vector<StageType> pipelineCycleN;
        bool stageGroupEmtpy = true;
        for(Stage & stage : st)
        {
            if(isStageEnabled(stage))
            {
                stageGroupEmtpy = false;
                pipelineCycleN.push_back(stage.get());
            }
        }
        if(!stageGroupEmtpy) pipeline.push_back(pipelineCycleN);
    }

    cout << "decode: " << pipeline.size() << endl;
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
    //setInvalidAddress();
    vda = vpa = 0;
    bus->read(src->val());
}

void W65816::fetch(Register16 *src, uint8_t * dst)
{
    //setValidAddress();
    if(src == &pc){ vpa = true; vda = tcycle == 0; }
    else{ vda = true; vpa = false; }
    //generateAddressWithBank();
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
