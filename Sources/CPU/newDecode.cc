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

    if(tcycle == 0)
    {
        lastPipelineStage(this);
        lastPipelineStage = StageType(dummyStage);
		isIndexRelated = false; //this must be reset here after the last stage is executed

        fetchInc(&pc,&ir);
    }
    else if(tcycle == 1)
    {
        decode(true);//instruction specific predecode signals here
		for(int i = 0; i < EnablingCondition::last; i++) enablingSignals[i] = isStageEnabled(EnablingCondition(i)); //evaluate after predecode signals

        int i = 0;
        for(i = enabledStages.size(); i > 0 && !enabledStages[i-1]; i--);
        pipelineSize = i + 2;

        fetch(&pc,&adr.low); //auto inc pc by default...? yes 52 out of 63 adr mode have a incPC signal
        if(prefetchIncPC) ++pc;
    }
    else
    {
        decode();
    }
    ++tcycle;
    if(tcycle >= pipelineSize)
    {
        checkInterupts();
        tcycle = 0;
		pipelineContent = REGULAR_INST;//move that in cycle 0 for more clarity
		prefetchIncPC = true;
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
    if(predecode) 
    {
        preDecodeStage = true;

        if(executeInterupt)
        {
            bool effectiveInterupt = true;
            if(internalIRQ && p.I()) effectiveInterupt = false;

            if(internalIRQ && !p.I()) {pipelineContent = IRQ_INTERUPT;}
            if(internalNMI) {pipelineContent = NMI_INTERUPT;}
            if(internalRST) {pipelineContent = RESET_INTERUPT;}

            if(effectiveInterupt) {--pc; invalidPrefetch();}
            executeInterupt = false;
        }
    }
    const int STARTING_CYCLE = 2;
    tcycle-=STARTING_CYCLE; //move that in isStageEnabled
    if(pipelineContent == REGULAR_INST)
    {
        switch(ir)
        {
		case 0x00: StackInterupt      	(StageType(BRK)); break;
        case 0x01: DirectXIndirect      (StageType(ORA)); break;
		case 0x02: StackInterupt      	(StageType(COP)); break;
		case 0x0C: AbsoluteRMW			(StageType(TSB)); break;
		case 0x0D: Absolute      		(StageType(ORA)); break;
		case 0x0E: AbsoluteRMW			(StageType(ASL)); break;
        case 0x10: RelativeBranch       (StageType(BPL)); break;
		case 0x1C: AbsoluteRMW			(StageType(TRB)); break;
        case 0x21: DirectXIndirect      (StageType(AND)); break;
		case 0x2C: Absolute      		(StageType(BIT)); break;
		case 0x2D: Absolute      		(StageType(AND)); break;
		case 0x2E: AbsoluteRMW			(StageType(ROL)); break;
        case 0x30: RelativeBranch       (StageType(BMI)); break;
        case 0x41: DirectXIndirect      (StageType(EOR)); break;
		case 0x4D: Absolute      		(StageType(EOR)); break;
		case 0x4E: AbsoluteRMW			(StageType(LSR)); break;
        case 0x50: RelativeBranch       (StageType(BVC)); break;
        case 0x61: DirectXIndirect      (StageType(ADC)); break;
		case 0x6D: Absolute      		(StageType(ADC)); break;
		case 0x6E: AbsoluteRMW			(StageType(ROR)); break;
        case 0x70: RelativeBranch       (StageType(BVS)); break;
        case 0x80: RelativeBranch       (StageType(BRA)); break;
        case 0x82: RelativeBranchLong   (StageType(dummyStage)); break;//BRL
		case 0x8C: isIndexRelated=true;AbsoluteWrite(StageType(STY)); break;
		case 0x8D: AbsoluteWrite		(StageType(STA)); break;
		case 0x8E: isIndexRelated=true;AbsoluteWrite(StageType(STX)); break;
        case 0x90: RelativeBranch       (StageType(BCC)); break;
		case 0x9C: AbsoluteWrite		(StageType(STZ)); break;
        case 0xA1: DirectXIndirect      (StageType(LDA)); break;
		case 0xAC: isIndexRelated=true;Absolute(StageType(LDY)); break;//this is the worst line of code ever wrote in all history
		case 0xAD: Absolute      		(StageType(LDA)); break;
		case 0xAE: isIndexRelated=true;Absolute(StageType(LDX)); break;
        case 0xB0: RelativeBranch       (StageType(BCS)); break;
        case 0xC1: DirectXIndirect      (StageType(CMP)); break;
		case 0xCC: isIndexRelated=true;Absolute(StageType(CPY)); break;
		case 0xCD: Absolute				(StageType(CMP)); break;
		case 0xCE: AbsoluteRMW			(StageType(DEC)); break;
        case 0xD0: RelativeBranch       (StageType(BNE)); break;
        case 0xE1: DirectXIndirect      (StageType(SBC)); break;
		case 0xEC: isIndexRelated=true;Absolute(StageType(CPX)); break;
		case 0xED: Absolute				(StageType(SBC)); break;
		case 0xEE: AbsoluteRMW			(StageType(INC)); break;
        case 0xF0: RelativeBranch       (StageType(BEQ)); break;
        }
    }
    else
    {
        switch(pipelineContent)
        {
            case IRQ_INTERUPT:      StackInterupt(StageType(IRQ));	 break;
            case NMI_INTERUPT:		StackInterupt(StageType(NMI));	 break;
            case RESET_INTERUPT:    StackInterupt(StageType(RESET)); break;
        }
    }
    
    tcycle+=STARTING_CYCLE;
    if(predecode) preDecodeStage = false;
}

void W65816::Absolute(StageType&& inst)
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
		fetchInc(&adr,&idb.low);
	}
	if(isStageEnabled(2,SIG_MODE16_ONLY))
	{
		fetch(&adr,&idb.high);
	}
}

void W65816::AbsoluteWrite(StageType&& inst)
{
	if(preDecodeStage)
	{
		;//empty
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		fetchInc(&pc,&adr.high);
		inst(this);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		writeInc(&adr,&idb.low);
	}
	if(isStageEnabled(2,SIG_MODE16_ONLY))
	{
		write(&adr,&idb.high);
	}
}

void W65816::AbsoluteRMW(StageType&& inst)
{
	if(preDecodeStage)
	{
		;//empty
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		fetchInc(&pc,&adr.high);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		fetchInc(&adr,&idb.low);
	}
	if(isStageEnabled(2,SIG_MODE16_ONLY))
	{
		fetch(&adr,&idb.high);
	}
	if(isStageEnabled(3,SIG_ALWAYS))
	{
		dummyFetchLast();
		inst(this);
		if(enablingSignals[SIG_MODE8_ONLY])
			decReg(&adr);
	}
	if(isStageEnabled(4,SIG_ALWAYS))
	{
		if(enablingSignals[SIG_MODE16_ONLY])
			writeDec(&adr,&idb.high);
		write(&adr,&idb.low);
	}
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
        if(enablingSignals[SIG_NATIVE_MODE])
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

void W65816::StackInterupt(StageType&& inst)
{
    if(preDecodeStage)
    {
        lastPipelineStage = StageType(dummyStage);
        noAutoIncPC();
    }

    if(isStageEnabled(0,SIG_NATIVE_MODE))
    {
        push(&pbr);
    }
    if(isStageEnabled(1,SIG_ALWAYS))
    {
        push(&pc.high);
    }
    if(isStageEnabled(2,SIG_ALWAYS))
    {
        push(&pc.low);
    }
    if(isStageEnabled(3,SIG_ALWAYS))
    {
        pushP();
        enableInterupts(false);
    }
    if(isStageEnabled(4,SIG_ALWAYS))
    {
        inst(this);
        fetchIncLong(&ZERO,&adr,&pc.low);
        moveReg8(&ZERO,&pbr); 
        //TODO: Vector Pull signal here(and next line)
    }
    if(isStageEnabled(5,SIG_ALWAYS))
    {
        fetchLong(&ZERO,&adr,&pc.high);
    }
}
