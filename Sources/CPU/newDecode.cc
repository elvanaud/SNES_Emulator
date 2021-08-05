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
		pipelineContent = REGULAR_INST;//move that in cycle 0 for more clarity
		prefetchIncPC = true;
		isIndexRelated = false; //this must be reset here after the last stage is executed
		doPrefetchInIDB = false;

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
		if(doPrefetchInIDB) idb.low = adr.low;
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
		if(enabledStages[tcycle])//use enablingsignals instead
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
		case 0x00: StackInterupt            (StageType(BRK)); break;
		case 0x01: DirectXIndirect          (StageType(ORA)); break;
		case 0x02: StackInterupt            (StageType(COP)); break;
		case 0x04: DirectRMW                (StageType(TSB)); break;
		case 0x05: Direct                   (StageType(ORA)); break;
		case 0x06: DirectRMW                (StageType(ASL)); break;
		case 0x07: DirectIndirectLong       (StageType(ORA)); break;
		case 0x09: Immediate                (StageType(ORA)); break;
		case 0x0A: Accumulator              (StageType(ASL)); break;
		case 0x0C: AbsoluteRMW              (StageType(TSB)); break;
		case 0x0D: Absolute                 (StageType(ORA)); break;
		case 0x0E: AbsoluteRMW              (StageType(ASL)); break;
		case 0x0F: AbsoluteLong             (StageType(ORA)); break;
		case 0x10: RelativeBranch           (StageType(BPL)); break;
		case 0x11: DirectIndirectY          (StageType(ORA)); break;
		case 0x12: DirectIndirect           (StageType(ORA)); break;
		case 0x14: DirectRMW                (StageType(TRB)); break;
		case 0x15: DirectX                  (StageType(ORA)); break;
		case 0x16: DirectXRMW               (StageType(ASL)); break;
		case 0x17: DirectIndirectYLong      (StageType(ORA)); break;
		case 0x18: Implied                  (StageType(CLC)); break;
		case 0x19: AbsoluteY                (StageType(ORA)); break;
		case 0x1A: Accumulator              (StageType(INC)); break;
		case 0x1B: Implied                  (StageType(TCS)); break;
		case 0x1C: AbsoluteRMW              (StageType(TRB)); break;
		case 0x1D: AbsoluteX                (StageType(ORA)); break;
		case 0x1E: AbsoluteXRMW             (StageType(ASL)); break;
		case 0x1F: AbsoluteXLong            (StageType(ORA)); break;
		case 0x20: AbsoluteJSR              (StageType(dummyStage)); break;//JSR
		case 0x21: DirectXIndirect          (StageType(AND)); break;
		case 0x22: AbsoluteLongJSL          (StageType(dummyStage)); break;//JSL
		case 0x24: Direct                   (StageType(BIT)); break;
		case 0x25: Direct                   (StageType(AND)); break;
		case 0x26: DirectRMW                (StageType(ROL)); break;
		case 0x27: DirectIndirectLong       (StageType(AND)); break;
		case 0x29: Immediate                (StageType(AND)); break;
		case 0x2A: Accumulator              (StageType(ROL)); break;//ROL A
		case 0x2C: Absolute                 (StageType(BIT)); break;
		case 0x2D: Absolute                 (StageType(AND)); break;
		case 0x2E: AbsoluteRMW              (StageType(ROL)); break;
		case 0x2F: AbsoluteLong             (StageType(AND)); break;
		case 0x30: RelativeBranch           (StageType(BMI)); break;
		case 0x31: DirectIndirectY          (StageType(AND)); break;
		case 0x32: DirectIndirect           (StageType(AND)); break;
		case 0x34: DirectX                  (StageType(BIT)); break;
		case 0x35: DirectX                  (StageType(AND)); break;
		case 0x36: DirectX                  (StageType(ROL)); break;
		case 0x37: DirectIndirectYLong      (StageType(AND)); break;
		case 0x38: Implied                  (StageType(SEC)); break;
		case 0x39: AbsoluteY                (StageType(AND)); break;
		case 0x3A: Accumulator              (StageType(DEC)); break;
		case 0x3B: Implied                  (StageType(TSC)); break;
		case 0x3C: AbsoluteX                (StageType(BIT)); break;
		case 0x3D: AbsoluteX                (StageType(AND)); break;
		case 0x3E: AbsoluteXRMW             (StageType(ROL)); break;
		case 0x3F: AbsoluteXLong            (StageType(AND)); break;
		case 0x41: DirectXIndirect          (StageType(EOR)); break;
		case 0x42: Immediate                (StageType(dummyStage)); break;//WDM
		case 0x45: Direct                   (StageType(EOR)); break;
		case 0x46: DirectRMW                (StageType(LSR)); break;
		case 0x47: DirectIndirectLong       (StageType(EOR)); break;
		case 0x49: Immediate                (StageType(EOR)); break;
		case 0x4A: Accumulator              (StageType(LSR)); break;
		case 0x4C: AbsoluteJMP              (StageType(dummyStage)); break;//JMP
		case 0x4D: Absolute                 (StageType(EOR)); break;
		case 0x4E: AbsoluteRMW              (StageType(LSR)); break;
		case 0x4F: AbsoluteLong             (StageType(EOR)); break;
		case 0x50: RelativeBranch           (StageType(BVC)); break;
		case 0x51: DirectIndirectY          (StageType(EOR)); break;
		case 0x52: DirectIndirect           (StageType(EOR)); break;
		case 0x55: DirectX                  (StageType(EOR)); break;
		case 0x56: DirectXRMW               (StageType(LSR)); break;
		case 0x57: DirectIndirectYLong      (StageType(EOR)); break;
		case 0x58: Implied                  (StageType(CLI)); break;
		case 0x59: AbsoluteY                (StageType(EOR)); break;
		case 0x5B: Implied                  (StageType(TCD)); break;
		case 0x5C: AbsoluteLongJMP          (StageType(dummyStage)); break;//JMP
		case 0x5D: AbsoluteX                (StageType(EOR)); break;
		case 0x5E: AbsoluteXRMW             (StageType(LSR)); break;
		case 0x5F: AbsoluteXLong            (StageType(EOR)); break;
		case 0x61: DirectXIndirect          (StageType(ADC)); break;
		case 0x64: DirectWrite              (StageType(STZ)); break;
		case 0x65: Direct                   (StageType(ADC)); break;
		case 0x66: DirectRMW                (StageType(ROR)); break;
		case 0x67: DirectIndirectLong       (StageType(ADC)); break;
		case 0x69: Immediate                (StageType(ADC)); break;
		case 0x6A: Accumulator              (StageType(ROR)); break;
		case 0x6C: AbsoluteIndirectJMP      (StageType(dummyStage)); break;//JMP
		case 0x6D: Absolute                 (StageType(ADC)); break;
		case 0x6E: AbsoluteRMW              (StageType(ROR)); break;
		case 0x6F: AbsoluteLong             (StageType(ADC)); break;
		case 0x70: RelativeBranch           (StageType(BVS)); break;
		case 0x71: DirectIndirectY          (StageType(ADC)); break;
		case 0x72: DirectIndirect           (StageType(ADC)); break;
		case 0x74: DirectXWrite             (StageType(STZ)); break;
		case 0x75: DirectX                  (StageType(ADC)); break;
		case 0x76: DirectXRMW               (StageType(ROR)); break;
		case 0x77: DirectIndirectYLong      (StageType(ADC)); break;
		case 0x78: Implied                  (StageType(SEI)); break;
		case 0x79: AbsoluteY                (StageType(ADC)); break;
		case 0x7B: Implied                  (StageType(TDC)); break;
		case 0x7C: AbsoluteXIndirectJMP     (StageType(dummyStage)); break;//JMP
		case 0x7D: AbsoluteX                (StageType(ADC)); break;
		case 0x7E: AbsoluteXRMW             (StageType(ROR)); break;
		case 0x7F: AbsoluteXLong            (StageType(ADC)); break;
		case 0x80: RelativeBranch           (StageType(BRA)); break;
		case 0x81: DirectXIndirectWrite     (StageType(STA)); break;
		case 0x82: RelativeBranchLong       (StageType(dummyStage)); break;//BRL
		case 0x84: isIndexRelated=true;DirectWrite(StageType(STY)); break;
		case 0x85: DirectWrite              (StageType(STA)); break;
		case 0x86: isIndexRelated=true;DirectWrite(StageType(STX)); break;
		case 0x87: DirectIndirectLongWrite  (StageType(STA)); break;
		case 0x88: Implied                  (StageType(DEY)); break;
		case 0x89: Immediate                (StageType(BIT)); break;
		case 0x8A: Implied                  (StageType(TXA)); break;
		case 0x8C: isIndexRelated=true;AbsoluteWrite(StageType(STY)); break;
		case 0x8D: AbsoluteWrite            (StageType(STA)); break;
		case 0x8E: isIndexRelated=true;AbsoluteWrite(StageType(STX)); break;
		case 0x8F: AbsoluteLongWrite        (StageType(STA)); break;
		case 0x90: RelativeBranch           (StageType(BCC)); break;
		case 0x91: DirectIndirectYWrite     (StageType(STA)); break;
		case 0x92: DirectIndirectWrite      (StageType(STA)); break;
		case 0x94: isIndexRelated=true;DirectXWrite(StageType(STY)); break;
		case 0x95: DirectXWrite             (StageType(STA)); break;
		case 0x96: isIndexRelated=true;DirectYWrite(StageType(STX)); break;
		case 0x97: DirectIndirectYLongWrite (StageType(STA)); break;
		case 0x98: Implied                  (StageType(TYA)); break;
		case 0x99: AbsoluteYWrite           (StageType(STA)); break;
		case 0x9A: Implied                  (StageType(TXS)); break;
		case 0x9B: Implied                  (StageType(TXY)); break;
		case 0x9C: AbsoluteWrite            (StageType(STZ)); break;
		case 0x9D: AbsoluteXWrite           (StageType(STA)); break;
		case 0x9E: AbsoluteXWrite           (StageType(STZ)); break;
		case 0x9F: AbsoluteXLongWrite       (StageType(STA)); break;
		case 0xA0: isIndexRelated=true;Immediate(StageType(LDY)); break;
		case 0xA1: DirectXIndirect          (StageType(LDA)); break;
		case 0xA2: isIndexRelated=true;Immediate(StageType(LDX)); break;
		case 0xA4: isIndexRelated=true;Direct(StageType(LDY)); break;
		case 0xA5: Direct                   (StageType(LDA)); break;
		case 0xA6: isIndexRelated=true;Direct(StageType(LDX)); break;
		case 0xA7: DirectIndirectLong       (StageType(LDA)); break;
		case 0xA8: Implied                  (StageType(TAY)); break;
		case 0xA9: Immediate                (StageType(LDA)); break;
		case 0xAA: Implied                  (StageType(TAX)); break;
		case 0xAC: isIndexRelated=true;Absolute(StageType(LDY)); break;//this is the worst line of code ever wrote in all history
		case 0xAD: Absolute                 (StageType(LDA)); break;
		case 0xAE: isIndexRelated=true;Absolute(StageType(LDX)); break;
		case 0xAF: AbsoluteLong             (StageType(LDA)); break;
		case 0xB0: RelativeBranch           (StageType(BCS)); break;
		case 0xB1: DirectIndirectY          (StageType(LDA)); break;
		case 0xB2: DirectIndirect           (StageType(LDA)); break;
		case 0xB4: isIndexRelated=true;DirectX(StageType(LDY)); break;
		case 0xB5: DirectX                  (StageType(LDA)); break;
		case 0xB6: isIndexRelated=true;DirectY(StageType(LDX)); break;
		case 0xB7: DirectIndirectYLong      (StageType(LDA)); break;
		case 0xB8: Implied                  (StageType(CLV)); break;
		case 0xB9: AbsoluteY                (StageType(LDA)); break;
		case 0xBA: Implied                  (StageType(TSX)); break;
		case 0xBB: Implied                  (StageType(TYX)); break;
		case 0xBC: isIndexRelated=true;AbsoluteX(StageType(LDY)); break;
		case 0xBD: AbsoluteX                (StageType(LDA)); break;
		case 0xBE: isIndexRelated=true;AbsoluteY(StageType(LDX)); break;
		case 0xBF: AbsoluteXLong            (StageType(LDA)); break;
		case 0xC0: isIndexRelated=true;Immediate(StageType(CPY)); break;
		case 0xC1: DirectXIndirect          (StageType(CMP)); break;
		case 0xC2: ImmediateSpecial         (StageType(REP)); break;
		case 0xC4: isIndexRelated=true;Direct(StageType(CPY)); break;
		case 0xC5: Direct                   (StageType(CMP)); break;
		case 0xC6: DirectRMW                (StageType(DEC)); break;
		case 0xC7: DirectIndirectLong       (StageType(CMP)); break;
		case 0xC8: Implied                  (StageType(INY)); break;
		case 0xC9: Immediate                (StageType(CMP)); break;
		case 0xCA: Implied                  (StageType(DEX)); break;
		case 0xCB: ImpliedSpecial           (StageType(WAI)); break;
		case 0xCC: isIndexRelated=true;Absolute(StageType(CPY)); break;
		case 0xCD: Absolute                 (StageType(CMP)); break;
		case 0xCE: AbsoluteRMW              (StageType(DEC)); break;
		case 0xCF: AbsoluteLong             (StageType(CMP)); break;
		case 0xD0: RelativeBranch           (StageType(BNE)); break;
		case 0xD1: DirectIndirectY          (StageType(CMP)); break;
		case 0xD2: DirectIndirect           (StageType(CMP)); break;
		case 0xD5: DirectX                  (StageType(CMP)); break;
		case 0xD6: DirectXRMW               (StageType(DEC)); break;
		case 0xD7: DirectIndirectYLong      (StageType(CMP)); break;
		case 0xD8: Implied                  (StageType(CLD)); break;
		case 0xD9: AbsoluteY                (StageType(CMP)); break;
		case 0xDB: ImpliedSpecial           (StageType(STP)); break;
		case 0xDC: AbsoluteIndirectJML      (StageType(dummyStage)); break;//JML
		case 0xDD: AbsoluteX                (StageType(CMP)); break;
		case 0xDE: AbsoluteXRMW             (StageType(DEC)); break;
		case 0xDF: AbsoluteXLong            (StageType(CMP)); break;
		case 0xE0: isIndexRelated=true;Immediate(StageType(CPX)); break;
		case 0xE1: DirectXIndirect          (StageType(SBC)); break;
		case 0xE2: ImmediateSpecial         (StageType(SEP)); break;
		case 0xE4: isIndexRelated=true;Direct(StageType(CPX)); break;
		case 0xE5: Direct                   (StageType(SBC)); break;
		case 0xE6: DirectRMW                (StageType(INC)); break;
		case 0xE7: DirectIndirectLong       (StageType(SBC)); break;
		case 0xE8: Implied                  (StageType(INX)); break;
		case 0xE9: Immediate                (StageType(SBC)); break;
		case 0xEA: Implied                  (StageType(dummyStage)); break;//NOP
		case 0xEB: ImpliedSpecial           (StageType(XBA)); break;
		case 0xEC: isIndexRelated=true;Absolute(StageType(CPX)); break;
		case 0xED: Absolute                 (StageType(SBC)); break;
		case 0xEE: AbsoluteRMW              (StageType(INC)); break;
		case 0xEF: AbsoluteLong             (StageType(SBC)); break;
		case 0xF0: RelativeBranch           (StageType(BEQ)); break;
		case 0xF1: DirectIndirectY          (StageType(SBC)); break;
		case 0xF2: DirectIndirect           (StageType(SBC)); break;
		case 0xF5: DirectX                  (StageType(SBC)); break;
		case 0xF6: DirectXRMW               (StageType(INC)); break;
		case 0xF7: DirectIndirectYLong      (StageType(SBC)); break;
		case 0xF8: Implied                  (StageType(SED)); break;
		case 0xF9: AbsoluteY                (StageType(SBC)); break;
		case 0xFB: Implied                  (StageType(XCE)); break;
		case 0xFC: AbsoluteXIndirectJSR     (StageType(dummyStage)); break;//JSR
		case 0xFD: AbsoluteX                (StageType(SBC)); break;
		case 0xFE: AbsoluteXRMW             (StageType(INC)); break;
		case 0xFF: AbsoluteXLong            (StageType(SBC)); break;
		}
	}
	else
	{
		switch(pipelineContent)
		{
			case IRQ_INTERUPT:      StackInterupt(StageType(IRQ));  break;
			case NMI_INTERUPT:      StackInterupt(StageType(NMI));  break;
			case RESET_INTERUPT:    StackInterupt(StageType(RESET));break;
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

void W65816::AbsoluteJMP(StageType&& inst)
{
	if(preDecodeStage)
	{
		lastPipelineStage = inst;
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		fetch(&pc,&pc.high);
		moveReg8(&adr.low,&pc.low);
	}
}

void W65816::AbsoluteJSR(StageType&& inst)
{
	if(preDecodeStage)
	{
		lastPipelineStage = inst;
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		fetch(&pc,&adr.high);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		dummyFetchLast();
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		push(&pc.high);
		moveReg8(&adr.high,&pc.high);
	}
	if(isStageEnabled(3,SIG_ALWAYS))
	{
		push(&pc.low);
		moveReg8(&adr.low,&pc.low);
	}
}

void W65816::AbsoluteLong(StageType&& inst)
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
		fetchInc(&pc,&tmpBank);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetchIncLong(&tmpBank,&adr,&idb.low);
	}
	if(isStageEnabled(3,SIG_MODE16_ONLY))
	{
		fetchLong(&tmpBank,&adr,&idb.high);
	}
}

void W65816::AbsoluteLongWrite(StageType&& inst)
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
		fetchInc(&pc,&tmpBank);
		inst(this);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		writeIncLong(&tmpBank,&adr,&idb.low);
	}
	if(isStageEnabled(3,SIG_MODE16_ONLY))
	{
		writeLong(&tmpBank,&adr,&idb.high);
	}
}

void W65816::AbsoluteLongJMP(StageType&& inst)
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
		fetchInc(&pc,&pbr);
		moveReg16(&adr,&pc);
	}
}

void W65816::AbsoluteLongJSL(StageType&& inst)
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
		push(&pbr);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		dummyFetchLast();
	}
	if(isStageEnabled(3,SIG_ALWAYS))
	{
		fetch(&pc,&pbr);
	}
	if(isStageEnabled(4,SIG_ALWAYS))
	{
		push(&pc.high);
	}
	if(isStageEnabled(5,SIG_ALWAYS))
	{
		push(&pc.low);
		moveReg16(&adr,&pc);
	}
}

void W65816::AbsoluteXLong(StageType&& inst)
{
	if(preDecodeStage)
	{
		lastPipelineStage = inst;
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		fetchInc(&pc,&adr.high);
		halfAdd(&adr.low,&x.low);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		fetchInc(&pc,&tmpBank);
		if(enablingSignals[SIG_X_CROSS_PAGE])
			fixCarry(&adr.high,&x.high);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetchIncLong(&tmpBank,&adr,&idb.low);
	}
	if(isStageEnabled(3,SIG_MODE16_ONLY))
	{
		fetchLong(&tmpBank,&adr,&idb.high);
	}
}

void W65816::AbsoluteXLongWrite(StageType&& inst)
{
	if(preDecodeStage)
	{
		;//empty
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		fetchInc(&pc,&adr.high);
		halfAdd(&adr.low,&x.low);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		fetchInc(&pc,&tmpBank);
		if(enablingSignals[SIG_X_CROSS_PAGE])
			fixCarry(&adr.high,&x.high);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		inst(this);
		writeIncLong(&tmpBank,&adr,&idb.low);
	}
	if(isStageEnabled(3,SIG_MODE16_ONLY))
	{
		writeLong(&tmpBank,&adr,&idb.high);
	}
}

void W65816::AbsoluteX(StageType&& inst)
{
	if(preDecodeStage)
	{
		lastPipelineStage = inst;
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		fetchInc(&pc,&adr.high);
		halfAdd(&adr.low,&x.low);
	}
	if(isStageEnabled(1,SIG_X_CROSS_PAGE))
	{
		dummyFetchLong(&dbr,&adr);
		fixCarry(&adr.high,&x.high);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetchInc(&adr,&idb.low);
	}
	if(isStageEnabled(3,SIG_MODE16_ONLY))
	{
		fetch(&adr,&idb.high);
	}
}

void W65816::AbsoluteXWrite(StageType&& inst)
{
	if(preDecodeStage)
	{
		; //empty
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		fetchInc(&pc,&adr.high);
		halfAdd(&adr.low,&x.low);
	}
	if(isStageEnabled(1,SIG_X_CROSS_PAGE))
	{
		dummyFetchLong(&dbr,&adr);
		fixCarry(&adr.high,&x.high);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		inst(this);
		writeInc(&adr,&idb.low);
	}
	if(isStageEnabled(3,SIG_MODE16_ONLY))
	{
		write(&adr,&idb.high);
	}
}

void W65816::AbsoluteXRMW(StageType&& inst)
{
	if(preDecodeStage)
	{
		; //empty
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		fetchInc(&pc,&adr.high);
		halfAdd(&adr.low,&x.low);
	}
	if(isStageEnabled(1,SIG_X_CROSS_PAGE))
	{
		dummyFetchLong(&dbr,&adr);
		fixCarry(&adr.high,&x.high);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetchInc(&adr,&idb.low);
	}
	if(isStageEnabled(3,SIG_MODE16_ONLY))
	{
		fetch(&adr,&idb.high);
	}
	if(isStageEnabled(4,SIG_ALWAYS))
	{
		dummyFetchLast();
		inst(this);
		if(enablingSignals[SIG_MODE8_ONLY])
			decReg(&adr);
	}
	if(isStageEnabled(5,SIG_MODE16_ONLY))
	{
		writeDec(&adr,&idb.high);
	}
	if(isStageEnabled(6,SIG_ALWAYS))
	{
		write(&adr,&idb.low);
	}
}

void W65816::AbsoluteY(StageType&& inst)
{
	if(preDecodeStage)
	{
		lastPipelineStage = inst;
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		fetchInc(&pc,&adr.high);
		halfAdd(&adr.low,&y.low);
	}
	if(isStageEnabled(1,SIG_Y_CROSS_PAGE))
	{
		dummyFetchLong(&dbr,&adr);
		fixCarry(&adr.high,&y.high);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetchInc(&adr,&idb.low);
	}
	if(isStageEnabled(3,SIG_MODE16_ONLY))
	{
		fetch(&adr,&idb.high);
	}
}

void W65816::AbsoluteYWrite(StageType&& inst)
{
	if(preDecodeStage)
	{
		; //empty
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		fetchInc(&pc,&adr.high);
		halfAdd(&adr.low,&y.low);
	}
	if(isStageEnabled(1,SIG_Y_CROSS_PAGE))
	{
		dummyFetchLong(&dbr,&adr);
		fixCarry(&adr.high,&y.high);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		inst(this);
		writeInc(&adr,&idb.low);
	}
	if(isStageEnabled(3,SIG_MODE16_ONLY))
	{
		write(&adr,&idb.high);
	}
}

void W65816::AbsoluteXIndirectJMP(StageType&& inst)
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
		dummyFetchLast();
		fullAdd(&adr,&x);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetchIncLong(&pbr,&adr,&pc.low);
	}
	if(isStageEnabled(3,SIG_ALWAYS))
	{
		fetchLong(&pbr,&adr,&pc.high);
	}
}

void W65816::AbsoluteXIndirectJSR(StageType&& inst)
{
	if(preDecodeStage)
	{
		lastPipelineStage = inst;
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		push(&pc.high);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		push(&pc.low);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetch(&pc,&adr.high);
	}
	if(isStageEnabled(3,SIG_ALWAYS))
	{
		dummyFetchLast();
		fullAdd(&adr,&x);
	}
	if(isStageEnabled(4,SIG_ALWAYS))
	{
		fetchIncLong(&pbr,&adr,&pc.low);
	}
	if(isStageEnabled(5,SIG_ALWAYS))
	{
		fetchLong(&pbr,&adr,&pc.high);
	}
}

void W65816::AbsoluteIndirectJML(StageType&& inst)
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
		fetchIncLong(&ZERO,&adr,&pc.low);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetchIncLong(&ZERO,&adr,&pc.high);
	}
	if(isStageEnabled(3,SIG_ALWAYS))
	{
		fetchLong(&ZERO,&adr,&pbr);
	}
}

void W65816::AbsoluteIndirectJMP(StageType&& inst)
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
		fetchIncLong(&ZERO,&adr,&pc.low);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetchLong(&ZERO,&adr,&pc.high);
	}
}

void W65816::Accumulator(StageType&& inst)
{
	if(preDecodeStage)
	{
		invalidPrefetch();
		accPrefetchInIDB();
		noAutoIncPC();
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		inst(this);
		moveReg8(&idb.low,&acc.low);
		if(enablingSignals[SIG_MODE16_ONLY])
			moveReg8(&idb.high,&acc.high);
	}
}

void W65816::Direct(StageType&& inst)
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
		fetchIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(2,SIG_MODE16_ONLY))
	{
		fetchLong(&ZERO,&adr,&idb.high);
	}
}

void W65816::DirectWrite(StageType&& inst)
{
	if(preDecodeStage)
	{
		dhPrefetchInAdr();
	}

	if(isStageEnabled(0,SIG_DL_NOT_ZERO))
	{
		dummyFetchLast();
		halfAdd(&adr.low,&d.low);
		fixCarry(&adr.high,&ZERO);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		inst(this);
		writeIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(2,SIG_MODE16_ONLY))
	{
		writeLong(&ZERO,&adr,&idb.high);
	}
}	

void W65816::DirectRMW(StageType&& inst)
{
	if(preDecodeStage)
	{
		dhPrefetchInAdr();
	}

	if(isStageEnabled(0,SIG_DL_NOT_ZERO))
	{
		dummyFetchLast();
		halfAdd(&adr.low,&d.low);
		fixCarry(&adr.high,&ZERO);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		fetchIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(2,SIG_MODE16_ONLY))
	{
		fetchLong(&ZERO,&adr,&idb.high);
	}
	if(isStageEnabled(3,SIG_ALWAYS))
	{
		dummyFetchLast();
		inst(this);
		if(enablingSignals[SIG_MODE8_ONLY])
			decReg(&adr);
	}
	if(isStageEnabled(4,SIG_MODE16_ONLY))
	{
		writeDecLong(&ZERO,&adr,&idb.high);
	}
	if(isStageEnabled(5,SIG_ALWAYS))
	{
		writeLong(&ZERO,&adr,&idb.low);
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

void W65816::DirectXIndirectWrite(StageType&& inst)
{
	if(preDecodeStage)
	{
		dhPrefetchInAdr();
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
		inst(this);
		writeIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(5,SIG_MODE16_ONLY))
	{
		writeLong(&ZERO,&adr,&idb.high);
	}
}

void W65816::DirectIndirect(StageType&& inst)
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
		fetchIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetchLong(&ZERO,&adr,&adr.high);
		moveReg8(&idb.low,&adr.low);
	}
	if(isStageEnabled(3,SIG_ALWAYS))
	{
		fetchIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(4,SIG_MODE16_ONLY))
	{
		fetchLong(&ZERO,&adr,&idb.high);
	}
}

void W65816::DirectIndirectWrite(StageType&& inst)
{
	if(preDecodeStage)
	{
		dhPrefetchInAdr();
	}

	if(isStageEnabled(0,SIG_DL_NOT_ZERO))
	{
		dummyFetchLast();
		halfAdd(&adr.low,&d.low);
		fixCarry(&adr.high,&ZERO);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		fetchIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetchLong(&ZERO,&adr,&adr.high);
		moveReg8(&idb.low,&adr.low);
	}
	if(isStageEnabled(3,SIG_ALWAYS))
	{
		inst(this);
		writeIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(4,SIG_MODE16_ONLY))
	{
		writeLong(&ZERO,&adr,&idb.high);
	}
}

void W65816::DirectIndirectY(StageType&& inst)
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
		fetchIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetchLong(&ZERO,&adr,&adr.high);
		moveReg8(&idb.low,&adr.low);
		halfAdd(&adr.low,&y.low);
	}
	if(isStageEnabled(3,SIG_INDIRECT_Y_CROSS_PAGE_OR_X16))
	{
		dummyFetch(&adr);
		fixCarry(&adr.high,&y.high);
	}
	if(isStageEnabled(4,SIG_ALWAYS))
	{
		fetchInc(&adr,&idb.low);
	}
	if(isStageEnabled(5,SIG_MODE16_ONLY))
	{
		fetch(&adr,&idb.high);
	}
}

void W65816::DirectIndirectYWrite(StageType&& inst)
{
	if(preDecodeStage)
	{
		dhPrefetchInAdr();
	}

	if(isStageEnabled(0,SIG_DL_NOT_ZERO))
	{
		dummyFetchLast();
		halfAdd(&adr.low,&d.low);
		fixCarry(&adr.high,&ZERO);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		fetchIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetchLong(&ZERO,&adr,&adr.high);
		moveReg8(&idb.low,&adr.low);
		halfAdd(&adr.low,&y.low);
	}
	if(isStageEnabled(3,SIG_INDIRECT_Y_CROSS_PAGE_OR_X16))
	{
		dummyFetch(&adr);
		fixCarry(&adr.high,&y.high);
	}
	if(isStageEnabled(4,SIG_ALWAYS))
	{
		inst(this);
		writeInc(&adr,&idb.low);
	}
	if(isStageEnabled(5,SIG_MODE16_ONLY))
	{
		write(&adr,&idb.high);
	}
}

void W65816::DirectIndirectYLong(StageType&& inst)
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
		fetchIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetchIncLong(&ZERO,&adr,&idb.high);
		halfAdd(&idb.low,&y.low);
	}
	if(isStageEnabled(3,SIG_ALWAYS))
	{
		fetchLong(&ZERO,&adr,&tmpBank);
		if(enablingSignals[SIG_INDIRECT_Y_CROSS_PAGE_OR_X16])
			fixCarry(&idb.high,&y.high);
	}
	if(isStageEnabled(4,SIG_ALWAYS))
	{
		moveReg16(&idb,&adr);
		fetchIncLong(&tmpBank,&adr,&idb.low);
	}
	if(isStageEnabled(5,SIG_MODE16_ONLY))
	{
		fetchLong(&tmpBank,&adr,&idb.high);
	}
}

void W65816::DirectIndirectYLongWrite(StageType&& inst)
{
	if(preDecodeStage)
	{
		dhPrefetchInAdr();
	}

	if(isStageEnabled(0,SIG_DL_NOT_ZERO))
	{
		dummyFetchLast();
		halfAdd(&adr.low,&d.low);
		fixCarry(&adr.high,&ZERO);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		fetchIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetchIncLong(&ZERO,&adr,&idb.high);
		halfAdd(&idb.low,&y.low);
	}
	if(isStageEnabled(3,SIG_ALWAYS))
	{
		fetchLong(&ZERO,&adr,&tmpBank);
		if(enablingSignals[SIG_INDIRECT_Y_CROSS_PAGE_OR_X16])
			fixCarry(&idb.high,&y.high);
	}
	if(isStageEnabled(4,SIG_ALWAYS))
	{
		moveReg16(&idb,&adr);
		inst(this);
		writeIncLong(&tmpBank,&adr,&idb.low);
	}
	if(isStageEnabled(5,SIG_MODE16_ONLY))
	{
		writeLong(&tmpBank,&adr,&idb.high);
	}
}

void W65816::DirectIndirectLong(StageType&& inst)
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
		fetchIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetchIncLong(&ZERO,&adr,&idb.high);
	}
	if(isStageEnabled(3,SIG_ALWAYS))
	{
		fetchLong(&ZERO,&adr,&tmpBank);
		moveReg16(&idb,&adr);
	}
	if(isStageEnabled(4,SIG_ALWAYS))
	{
		fetchIncLong(&tmpBank,&adr,&idb.low);
	}
	if(isStageEnabled(5,SIG_MODE16_ONLY))
	{
		fetchLong(&tmpBank,&adr,&idb.high);
	}
}

void W65816::DirectIndirectLongWrite(StageType&& inst)
{
	if(preDecodeStage)
	{
		dhPrefetchInAdr();
	}

	if(isStageEnabled(0,SIG_DL_NOT_ZERO))
	{
		dummyFetchLast();
		halfAdd(&adr.low,&d.low);
		fixCarry(&adr.high,&ZERO);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		fetchIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetchIncLong(&ZERO,&adr,&idb.high);
	}
	if(isStageEnabled(3,SIG_ALWAYS))
	{
		fetchLong(&ZERO,&adr,&tmpBank);
		moveReg16(&idb,&adr);
	}
	if(isStageEnabled(4,SIG_ALWAYS))
	{
		inst(this);
		writeIncLong(&tmpBank,&adr,&idb.low);
	}
	if(isStageEnabled(5,SIG_MODE16_ONLY))
	{
		writeLong(&tmpBank,&adr,&idb.high);
	}
}

void W65816::DirectX(StageType&& inst)
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
	if(isStageEnabled(3,SIG_MODE16_ONLY))
	{
		fetchLong(&ZERO,&adr,&idb.high);
	}
}

void W65816::DirectXWrite(StageType&& inst)
{
	if(preDecodeStage)
	{
		dhPrefetchInAdr();
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
		inst(this);
		writeIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(3,SIG_MODE16_ONLY))
	{
		writeLong(&ZERO,&adr,&idb.high);
	}
}

void W65816::DirectXRMW(StageType&& inst)
{
	if(preDecodeStage)
	{
		dhPrefetchInAdr();
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
	if(isStageEnabled(3,SIG_MODE16_ONLY))
	{
		fetchLong(&ZERO,&adr,&idb.high);
	}
	if(isStageEnabled(4,SIG_ALWAYS))
	{
		dummyFetchLast();
		inst(this);
		if(enablingSignals[SIG_MODE8_ONLY])
			decReg(&adr);
	}
	if(isStageEnabled(5,SIG_MODE16_ONLY))
	{
		writeDecLong(&ZERO,&adr,&idb.high);
	}
	if(isStageEnabled(6,SIG_ALWAYS))
	{
		writeLong(&ZERO,&adr,&idb.low);
	}
}

void W65816::DirectY(StageType&& inst)
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
		fullAdd(&adr,&y);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetchIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(3,SIG_MODE16_ONLY))
	{
		fetchLong(&ZERO,&adr,&idb.high);
	}
}

void W65816::DirectYWrite(StageType&& inst)
{
	if(preDecodeStage)
	{
		dhPrefetchInAdr();
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
		fullAdd(&adr,&y);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		inst(this);
		writeIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(3,SIG_MODE16_ONLY))
	{
		writeLong(&ZERO,&adr,&idb.high);
	}
}

void W65816::Immediate(StageType&& inst)
{
	if(preDecodeStage)
	{
		opPrefetchInIDB();
		lastPipelineStage = inst;
	}

	if(isStageEnabled(0,SIG_MODE16_ONLY))
	{
		fetchInc(&pc,&idb.high);
	}
}

void W65816::ImmediateSpecial(StageType&& inst)
{
	if(preDecodeStage)
	{
		opPrefetchInIDB();
		noAutoIncPC();
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		inst(this);
		++pc;//used to be incPC(2) signal
	}
}

void W65816::Implied(StageType&& inst)
{
	if(preDecodeStage)
	{
		invalidPrefetch();
		noAutoIncPC();
		lastPipelineStage = inst;
		enabledStages.resize(0);//no stages in pipeline
	}
}
	
void W65816::ImpliedSpecial(StageType&& inst)
{
	if(preDecodeStage)
	{
		invalidPrefetch();
		noAutoIncPC();
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		inst(this);
	}
}	
	
	
	
	
/////////////
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
