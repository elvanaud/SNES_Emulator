#include "W65816.h"
#include "../Bus.h"

#include <iostream>
using std::cout;
using std::endl;

void W65816::processSignals()
{
    for(auto & sig : decodingTable[ir].Signals())
    {
        sig();
    }
}

void W65816::preDecode()
{
    for (auto & sig : decodingTable[ir].PredecodeSignals())
    {
        sig();
    }
}

void W65816::decode()
{
    preDecode();
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

void W65816::dummyStage()
{
    ; //Dummy
}

void W65816::dummyFetch(Register16 *src)
{
    handleValidAddressPINS(InternalOperation);
    bus->read(src->val());
}

void W65816::fetch(Register16 *src, uint8_t * dst)
{
    ValidAddressState state = DataFetch;
    if(src == &pc)
    {
        state = OperandFetch;
        if(tcycle == 0) state = OpcodeFetch;
    }
    handleValidAddressPINS(state);
    //todo: generateAddressWithBank(); //Based on vda / vpa
    addressBusBuffer = src->val();
    bus->read(addressBusBuffer);
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

void W65816::write(Register16 *adr, uint8_t * data)
{
    handleValidAddressPINS(DataFetch);
    //todo: generateAddressWithBank(); //Based on vda / vpa
    addressBusBuffer = adr->val();
    bus->write(addressBusBuffer, *data);
}

void W65816::writeInc(Register16 * adr, uint8_t * data)
{
    write(adr,data);
    ++(*adr);
}

void W65816::writeDec(Register16 * adr, uint8_t * data)
{
    write(adr,data);
    --(*adr);
}

void W65816::decReg(Register16 * reg)
{
    --(*reg);
}
