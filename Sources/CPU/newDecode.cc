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
        lastPipelineStage(this);
        tcycle = 0;
        endOfPipeline = false;
        checkInterupts();
    }

    switch(tcycle)
    {
    case 0:
        //maybe move the lastPipeline and interupt check here (would be more readable ?)
        fetchInc(&pc,&ir);
        break;
    case 1:
        preDecodeStage = true;
        decode();//instruction specific predecode signals here
        preDecodeStage = false;
        fetchInc(&pc,&adr.low); //auto inc pc by default
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

void W65816::decode()
{
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
}

void W65816::RelativeBranch(StageType inst)
{
    RelativeBranch.setStages({  {Stage(Stage::SIG_INST,dummyStage)}, //Special Case for branch instruction: the first stage is executed within the decode operation (in T1)
                                {Stage(Stage::SIG_ALWAYS,dummyFetchLast),Stage(Stage::SIG_ALWAYS,halfAdd,&pc.low,&adr.low),Stage(Stage::SIG_NATIVE_MODE,fixCarry,&pc.high,&SIGN_EXTENDED_OP_HALF_ADD)}, //The following stages are those to be executed if the branch is taken
                                {Stage(Stage::SIG_PC_CROSS_PAGE_IN_EMUL,dummyFetchLast),Stage(Stage::SIG_PC_CROSS_PAGE_IN_EMUL,fixCarry,&pc.high,&SIGN_EXTENDED_OP_HALF_ADD)}, //TODO: remove the sig_always on this line ?? (cycle accuracy of BNE (D0)
                                {Stage(Stage::SIG_DUMMY_STAGE, dummyStage)}});
    RelativeBranch.setSignals({bind(incPC,this,1)});
    RelativeBranch.setPredecodeSignals({bind(branchInstruction,this)});
    if(preDecodeStage)
    {
        inst(this);
        if(!branchTaken)
        {
            endPipeline(StageType(dummyStage));
        }
        return;
    }
    switch(tcycle)
    {
    case 0:
        dummyFetchLast();
        halfAdd(&pc.low,&adr.low);
        if(isStageEnabled(Stage::SIG_NATIVE_MODE))
            fixCarry(&pc.high,&SIGN_EXTENDED_OP_HALF_ADD);
        break;
    case 1:
        if(isStageEnabled(Stage::SIG_PC_CROSS_PAGE_IN_EMUL))
        {
            dummyFetchLast();
            fixCarry(&pc.high,&SIGN_EXTENDED_OP_HALF_ADD);
            break;
        }
        tcycle++;
    case 2:
        endPipeline(StageType(dummyStage));
        break;
    }
}

void W65816::DirectXIndirect(StageType inst)
{
    if(preDecodeStage)
    {
        dhPrefetchInAdr(); //here or in the if
        return;
    }
    switch(tcycle)
    {
    case 0:
        if(isStageEnabled(Stage::SIG_DL_NOT_ZERO))
        {
            dummyFetchLast();
            halfAdd(&adr.low,&d.low);
            fixCarry(&adr.high,&ZERO);
            break;
        }
        tcycle++;
    case 1:
        //incPC(); //done before
        dummyFetchLast();
        fullAdd(&adr,&x);
        break;
    case 2:
        fetchIncLong(&ZERO,&adr,&idb.low);
        break;
    case 3:
        fetchLong(&ZERO,&adr,&adr.high);
        moveReg8(&idb.low,&adr.low);
        break;
    case 4:
        fetchIncLong(&ZERO,&adr,&idb.low);
        break;
    case 5:
        if(isStageEnabled(Stage::SIG_MODE16_ONLY))
        {
            fetchLong(&ZERO,&adr,&idb.high);
            break;
        }
        tcycle++; //todo: create a macro for that if followed by inc, (have the macro create the cases too)
    case 6:
        endPipeline(inst);
    }
}
