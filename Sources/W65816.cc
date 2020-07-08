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
    reloadPipeline();
    pc.set(0x100);
}

void W65816::attachBus(Bus * b)
{
    bus = b;
}

void W65816::reloadPipeline()
{
    pipeline.clear();
    tcycle = 0;

    vector<StageType> T1 = {bind(fetchInc,_1,&pc,&ir)};
    pipeline.push_back(T1);

    vector<StageType> T2 = {Stage(Stage::SIG_ALWAYS,fetch,&pc,&adr.low).get(), Stage(Stage::SIG_ALWAYS,decode).get()};
    pipeline.push_back(T2);
}

bool W65816::isStageEnabled(Stage &st)
{
    return true;
}

void W65816::decode()
{
    for(auto &st : decodingTable[ir].Stages())
    {
        vector<StageType> pipelineCycleN;
        for(Stage & stage : st)
        {
            if(isStageEnabled(stage))
            {
                pipelineCycleN.push_back(stage.get());
            }
        }
        pipeline.push_back(pipelineCycleN);
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
    //setInvalidAddress();
    bus->read(src->val());
}

void W65816::fetch(Register16 *src, uint8_t * dst)
{
    //setValidAddress();
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
    cout << pc.val() << endl,
    //processSignals();
    //checkInterupts();

    ++tcycle;

    if(tcycle >= pipeline.size())
    {
        reloadPipeline();
    }
}
