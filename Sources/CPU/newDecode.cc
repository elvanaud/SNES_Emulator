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

    if(endOfPipeline)
    {
        tcycle = 0;
        endOfPipeline = false;
    }

    switch(tcycle)
    {
    case 0:
        lastPipelineStage(this);
        checkInterupts();

        fetchInc(&pc,&ir);
        break;
    case 1:
        decode(true);//instruction specific predecode signals here
        /*for(int i = 0; i < Stage::EnablingCondition::last; i++)
        {
            enablingSignals[i] = isStageEnabled(Stage::EnablingCondition(i));
        }*/

        fetchInc(&pc,&adr.low); //auto inc pc by default...?
        break;
    default:
        decode();
        break;
    }
    ++tcycle;
}

void W65816::endPipeline(StageType inst)
{
    lastPipelineStage = inst;
    endOfPipeline = true;
}

bool W65816::isStageEnabled(unsigned int cycle, Stage::EnablingCondition signal)
{
    if(preDecodeStage)
    {
        enabledStages.resize(cycle,false);
        enabledStages[cycle] = isStageEnabled(signal);
        //popback last false elements, then can use the same condition for end of pipeline
        return false;
    }

    if(tcycle == cycle)
    {
        //if(enablingSignals[signal])
        if(enabledStages[tcycle])
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
    tcycle-=STARTING_CYCLE; 
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
        if(!branchTaken)
        {
            endPipeline(StageType(dummyStage));
        }
        //incPC();
        return;
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
    
    endPipeline(StageType(dummyStage));
}

void W65816::DirectXIndirect(StageType inst)
{
    if(preDecodeStage)
    {
        //incPC(); //done before ?
        dhPrefetchInAdr(); //here or in the if
        //lastPipelineStage = inst;
        /*if(enablingSignals[Stage::SIG_MEM16_ONLY])
            endOfPipeline(5,inst);
        else
            endOfPipeline(4,inst);
        return;*/
    }

    if(isStageEnabled(0,Stage::SIG_DL_NOT_ZERO))
    {
        dummyFetchLast();
        halfAdd(&adr.low,&d.low);
        fixCarry(&adr.high,&ZERO);
    }//tcycle++;//auto handled in method ^
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
    //tcycle++; //todo: create a macro for that if followed by inc, (have the macro create the cases too)
    //case 6:
    //endPipeline(inst);
}
