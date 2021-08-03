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

    /////////// in decode stage
    if(executeInterupt)
    {
        bool effectiveInterupt = true;
        if(internalIRQ && p.I()) effectiveInterupt = false;

        if(internalIRQ && !p.I()) {instToDecode = interuptIRQ;}
        if(internalNMI) {instToDecode = interuptNMI;}
        if(internalRST) {instToDecode = interuptRESET;}

        if(effectiveInterupt) {--pc; invalidPrefetch();}
        executeInterupt = false;
    }
    ////////////////

    if(tcycle == 0)
    {
        lastPipelineStage(this);
        checkInterupts();

        fetchInc(&pc,&ir);
    }
    else if(tcycle == 1)
    {
        decode(true);//instruction specific predecode signals here

        int i = 0;
        for(i = enabledStages.size(); i > 0 && !enabledStages[i-1]; i--);
        pipelineSize = i + 2;

        fetch(&pc,&adr.low); //auto inc pc by default...? yes 52 out of 63 adr mode have a incPC signal
        ++pc;//disable this with noIncPC signal
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

bool W65816::isStageEnabled(unsigned int cycle, EnablingCondition signal)
{
    if(preDecodeStage)
    {
        enabledStages.resize(cycle,false);
        enabledStages[cycle] = isStageEnabled(signal);
        return false;
    }

    if(tcycle == cycle)
    {
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
    case 0x01: DirectXIndirect      (StageType(ORA)); break;
    case 0x10: RelativeBranch       (StageType(BPL)); break;
    case 0x21: DirectXIndirect      (StageType(AND)); break;
    case 0x30: RelativeBranch       (StageType(BMI)); break;
    case 0x41: DirectXIndirect      (StageType(EOR)); break;
    case 0x50: RelativeBranch       (StageType(BVC)); break;
    case 0x61: DirectXIndirect      (StageType(ADC)); break;
    case 0x70: RelativeBranch       (StageType(BVS)); break;
    case 0x80: RelativeBranch       (StageType(BRA)); break;
    case 0x82: RelativeBranchLong   (StageType(dummyStage)); break;//BRL
    case 0x90: RelativeBranch       (StageType(BCC)); break;
    case 0xA1: DirectXIndirect      (StageType(LDA)); break;
    case 0xB0: RelativeBranch       (StageType(BCS)); break;
    case 0xC1: DirectXIndirect      (StageType(CMP)); break;
    case 0xD0: RelativeBranch       (StageType(BNE)); break;
    case 0xE1: DirectXIndirect      (StageType(SBC)); break;
    case 0xF0: RelativeBranch       (StageType(BEQ)); break;
    }
    tcycle+=STARTING_CYCLE;
    if(predecode) preDecodeStage = false;
}

void W65816::DirectXIndirect(StageType&& inst)
{
    if(preDecodeStage)
    {
        dhPrefetchInAdr();
        lastPipelineStage = inst;
    }

    if(isStageEnabled(0,SIG_DL_NOT_ZERO))
    {
        dummyFetchLast();
        halfAdd(&adr.low,&d.low);
        fixCarry(&adr.high,&ZERO);
    }
    if(isStageEnabled(1,SIG_ALWAYS))
    {
        dummyFetchLast();
        fullAdd(&adr,&x);
    }
    if(isStageEnabled(2,SIG_ALWAYS))
    {
        fetchIncLong(&ZERO,&adr,&idb.low);
    }
    if(isStageEnabled(3,SIG_ALWAYS))
    {
        fetchLong(&ZERO,&adr,&adr.high);
        moveReg8(&idb.low,&adr.low);
    }
    if(isStageEnabled(4,SIG_ALWAYS))
    {
        fetchIncLong(&ZERO,&adr,&idb.low);
    }
    if(isStageEnabled(5,SIG_MODE16_ONLY))
    {
        fetchLong(&ZERO,&adr,&idb.high);
    }
}

void W65816::RelativeBranch(StageType&& inst)
{
    if(preDecodeStage)
    {
        inst(this);
        lastPipelineStage = StageType(dummyStage);

        if(!branchTaken)
        {
            enabledStages.resize(0);
            return;
        }
    }

    if(isStageEnabled(0,SIG_ALWAYS))
    {
        dummyFetchLast();
        halfAdd(&pc.low,&adr.low);
        if(isStageEnabled(0,SIG_NATIVE_MODE))//careful, might want to rename those methods to not fuck up everything
            fixCarry(&pc.high,&SIGN_EXTENDED_OP_HALF_ADD);
    }
    if(isStageEnabled(1,SIG_PC_CROSS_PAGE_IN_EMUL))
    {
        dummyFetchLast();
        fixCarry(&pc.high,&SIGN_EXTENDED_OP_HALF_ADD);
    }
}

void W65816::RelativeBranchLong(StageType&& inst)
{
    if(preDecodeStage)
    {
        lastPipelineStage = inst;
    }

    if(isStageEnabled(0,SIG_ALWAYS))
    {
        fetchInc(&pc,&adr.high);
    }
    if(isStageEnabled(1,SIG_ALWAYS))
    {
        fullAdd(&pc,&adr);
    }
}
