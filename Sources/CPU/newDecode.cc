#include "W65816.h"

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

    /*if(endOfPipeline)
    {
        tcycle = 0;
        endOfPipeline = false;
    }*/

    if(tcycle == 0)
    {
        lastPipelineStage(this);
        checkInterupts();

        fetchInc(&pc,&ir);
    }
    else if(tcycle == 1)
    {
        decode(true);//instruction specific predecode signals here
        /*for(int i = 0; i < Stage::EnablingCondition::last; i++)
        {
            enablingSignals[i] = isStageEnabled(Stage::EnablingCondition(i));
        }*/
        int i = 0;
        for(i = enabledStages.size(); i > 0 && !enabledStages[i-1]; i--);
        pipelineSize = i + 2;

        fetchInc(&pc,&adr.low); //auto inc pc by default...?
    }
    else
    {
        decode();
    }
    ++tcycle;
    if(tcycle >= pipelineSize)
    {
        tcycle = 0;
    }
}

/*void W65816::endPipeline(StageType inst)
{
    lastPipelineStage = inst;
    endOfPipeline = true;
}*/

bool W65816::isStageEnabled(unsigned int cycle, Stage::EnablingCondition signal)
{
    if(preDecodeStage)
    {
        enabledStages.resize(cycle,false);
        enabledStages[cycle] = isStageEnabled(signal);
        return false;
    }

    if(tcycle == cycle)
    {
        //if(enablingSignals[signal])
        if(enabledStages[tcycle])//TODO: should loop !! (multiple disabled stages in a row)
        {
            return true;
        }
        else
        {
            tcycle++;
        }
    }
    return false;
}

void W65816::decode(bool predecode)
{
    if(predecode) preDecodeStage = true;
    const int STARTING_CYCLE = 2;
    tcycle-=STARTING_CYCLE; //move that in isstageenabled
    switch(ir)
    {
    case 0x00://brk
        break;
    case 0x01: DirectXIndirect(StageType(ORA)); break;
    case 0x21: DirectXIndirect(StageType(AND)); break;
    case 0x41: DirectXIndirect(StageType(EOR)); break;
    case 0x61: DirectXIndirect(StageType(ADC)); break;
    case 0xA1: DirectXIndirect(StageType(LDA)); break;
    case 0xC1: DirectXIndirect(StageType(CMP)); break;
    case 0xE1: DirectXIndirect(StageType(SBC)); break;
    }
    tcycle+=STARTING_CYCLE;
    if(predecode) preDecodeStage = false;
}

void W65816::RelativeBranch(StageType inst)
{
    if(preDecodeStage)
    {
        inst(this);
        lastPipelineStage = StageType(dummyStage);
        //incPC();

        if(!branchTaken)
        {
            enabledStages.resize(0);
            return;
        }
    }

    if(isStageEnabled(0,Stage::SIG_ALWAYS))
    {
        dummyFetchLast();
        halfAdd(&pc.low,&adr.low);
        if(isStageEnabled(0,Stage::SIG_NATIVE_MODE))//careful, might want to rename those methods to not fuck up everything
            fixCarry(&pc.high,&SIGN_EXTENDED_OP_HALF_ADD);
    }
    if(isStageEnabled(1,Stage::SIG_PC_CROSS_PAGE_IN_EMUL))
    {
        dummyFetchLast();
        fixCarry(&pc.high,&SIGN_EXTENDED_OP_HALF_ADD);
    }
}

void W65816::DirectXIndirect(StageType inst)
{
    if(preDecodeStage)
    {
        //incPC(); //done before ?
        dhPrefetchInAdr();
        lastPipelineStage = inst;
    }

    if(isStageEnabled(0,Stage::SIG_DL_NOT_ZERO))
    {
        dummyFetchLast();
        halfAdd(&adr.low,&d.low);
        fixCarry(&adr.high,&ZERO);
    }
    if(isStageEnabled(1,Stage::SIG_ALWAYS))
    {
        dummyFetchLast();
        fullAdd(&adr,&x);
    }
    if(isStageEnabled(2,Stage::SIG_ALWAYS))
    {
        fetchIncLong(&ZERO,&adr,&idb.low);
    }
    if(isStageEnabled(3,Stage::SIG_ALWAYS))
    {
        fetchLong(&ZERO,&adr,&adr.high);
        moveReg8(&idb.low,&adr.low);
    }
    if(isStageEnabled(4,Stage::SIG_ALWAYS))
    {
        fetchIncLong(&ZERO,&adr,&idb.low);
    }
    if(isStageEnabled(5,Stage::SIG_MODE16_ONLY))
    {
        fetchLong(&ZERO,&adr,&idb.high);
    }
}
