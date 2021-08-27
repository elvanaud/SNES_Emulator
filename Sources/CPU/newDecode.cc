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
		asmLine = "if you see this there is a problem... a big problem";

		preDecodeStage = true;
		fetchInc(&pc,&ir);
		//asm disassembly should be accessible here
		//move decode(true) here ??
		decode(true);//instruction specific predecode signals here
		for(int i = 0; i < EnablingCondition::last; i++) enablingSignals[i] = isStageEnabled(EnablingCondition(i)); //evaluate after predecode signals

		int i = 0;
		for(i = enabledStages.size(); i > 0 && !enabledStages[i-1]; i--);
		pipelineSize = i + 2;
	}
	else if(tcycle == 1)
	{
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
		enabledStages.resize(cycle+1,false);
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

			if(effectiveInterupt) 
			{
				--pc; 
				invalidPrefetch(); 
				vda = vpa = false; //force internal operation for this cycle too (not the expected behavior but I need it for tracing the asm)
			} 
			executeInterupt = false;
		}
	}
	const int STARTING_CYCLE = 2;
	tcycle-=STARTING_CYCLE; //move that in isStageEnabled
	if(pipelineContent == REGULAR_INST)
	{
		switch(ir)
		{
		case 0x00: StackInterupt                (StageType(BRK)); break;
		case 0x01: DirectXIndirect              (StageType(ORA)); break;
		case 0x02: StackInterupt                (StageType(COP)); break;
		case 0x03: StackRelative                (StageType(ORA)); break;
		case 0x04: DirectRMW                    (StageType(TSB)); break;
		case 0x05: Direct                       (StageType(ORA)); break;
		case 0x06: DirectRMW                    (StageType(ASL)); break;
		case 0x07: DirectIndirectLong           (StageType(ORA)); break;
		case 0x08: StackPush8                   (StageType(PHP)); break;
		case 0x09: Immediate                    (StageType(ORA)); break;
		case 0x0A: Accumulator                  (StageType(ASL)); break;
		case 0x0B: StackPush16                  (StageType(PHD)); break;
		case 0x0C: AbsoluteRMW                  (StageType(TSB)); break;
		case 0x0D: Absolute                     (StageType(ORA)); break;
		case 0x0E: AbsoluteRMW                  (StageType(ASL)); break;
		case 0x0F: AbsoluteLong                 (StageType(ORA)); break;
		case 0x10: RelativeBranch               (StageType(BPL)); break;
		case 0x11: DirectIndirectY              (StageType(ORA)); break;
		case 0x12: DirectIndirect               (StageType(ORA)); break;
		case 0x13: StackRelativeIndirectY       (StageType(ORA)); break;
		case 0x14: DirectRMW                    (StageType(TRB)); break;
		case 0x15: DirectX                      (StageType(ORA)); break;
		case 0x16: DirectXRMW                   (StageType(ASL)); break;
		case 0x17: DirectIndirectYLong          (StageType(ORA)); break;
		case 0x18: Implied                      (StageType(CLC)); break;
		case 0x19: AbsoluteY                    (StageType(ORA)); break;
		case 0x1A: Accumulator                  (StageType(INC)); break;
		case 0x1B: Implied                      (StageType(TCS)); break;
		case 0x1C: AbsoluteRMW                  (StageType(TRB)); break;
		case 0x1D: AbsoluteX                    (StageType(ORA)); break;
		case 0x1E: AbsoluteXRMW                 (StageType(ASL)); break;
		case 0x1F: AbsoluteXLong                (StageType(ORA)); break;
		case 0x20: AbsoluteJSR                  (StageType(JSR)); break;
		case 0x21: DirectXIndirect              (StageType(AND)); break;
		case 0x22: AbsoluteLongJSL              (StageType(JSL)); break;
		case 0x23: StackRelative                (StageType(AND)); break;
		case 0x24: Direct                       (StageType(BIT)); break;
		case 0x25: Direct                       (StageType(AND)); break;
		case 0x26: DirectRMW                    (StageType(ROL)); break;
		case 0x27: DirectIndirectLong           (StageType(AND)); break;
		case 0x28: StackPop8                    (StageType(PLP)); break;
		case 0x29: Immediate                    (StageType(AND)); break;
		case 0x2A: Accumulator                  (StageType(ROL)); break;
		case 0x2B: StackPop16                   (StageType(PLD)); break;
		case 0x2C: Absolute                     (StageType(BIT)); break;
		case 0x2D: Absolute                     (StageType(AND)); break;
		case 0x2E: AbsoluteRMW                  (StageType(ROL)); break;
		case 0x2F: AbsoluteLong                 (StageType(AND)); break;
		case 0x30: RelativeBranch               (StageType(BMI)); break;
		case 0x31: DirectIndirectY              (StageType(AND)); break;
		case 0x32: DirectIndirect               (StageType(AND)); break;
		case 0x33: StackRelativeIndirectY       (StageType(AND)); break;
		case 0x34: DirectX                      (StageType(BIT)); break;
		case 0x35: DirectX                      (StageType(AND)); break;
		case 0x36: DirectX                      (StageType(ROL)); break;
		case 0x37: DirectIndirectYLong          (StageType(AND)); break;
		case 0x38: Implied                      (StageType(SEC)); break;
		case 0x39: AbsoluteY                    (StageType(AND)); break;
		case 0x3A: Accumulator                  (StageType(DEC)); break;
		case 0x3B: Implied                      (StageType(TSC)); break;
		case 0x3C: AbsoluteX                    (StageType(BIT)); break;
		case 0x3D: AbsoluteX                    (StageType(AND)); break;
		case 0x3E: AbsoluteXRMW                 (StageType(ROL)); break;
		case 0x3F: AbsoluteXLong                (StageType(AND)); break;
		case 0x40: StackRTI                     (StageType(RTI)); break;
		case 0x41: DirectXIndirect              (StageType(EOR)); break;
		case 0x42: Immediate                    (StageType(WDM)); break;
		case 0x43: StackRelative                (StageType(EOR)); break;
		case 0x44: BlockMoveP                   (StageType(MVP)); break;
		case 0x45: Direct                       (StageType(EOR)); break;
		case 0x46: DirectRMW                    (StageType(LSR)); break;
		case 0x47: DirectIndirectLong           (StageType(EOR)); break;
		case 0x48: StackPush                    (StageType(PHA)); break;
		case 0x49: Immediate                    (StageType(EOR)); break;
		case 0x4A: Accumulator                  (StageType(LSR)); break;
		case 0x4B: StackPush8                   (StageType(PHK)); break;
		case 0x4C: AbsoluteJMP                  (StageType(JMP)); break;
		case 0x4D: Absolute                     (StageType(EOR)); break;
		case 0x4E: AbsoluteRMW                  (StageType(LSR)); break;
		case 0x4F: AbsoluteLong                 (StageType(EOR)); break;
		case 0x50: RelativeBranch               (StageType(BVC)); break;
		case 0x51: DirectIndirectY              (StageType(EOR)); break;
		case 0x52: DirectIndirect               (StageType(EOR)); break;
		case 0x53: StackRelativeIndirectY       (StageType(EOR)); break;
		case 0x54: BlockMoveN                   (StageType(MVN)); break;
		case 0x55: DirectX                      (StageType(EOR)); break;
		case 0x56: DirectXRMW                   (StageType(LSR)); break;
		case 0x57: DirectIndirectYLong          (StageType(EOR)); break;
		case 0x58: Implied                      (StageType(CLI)); break;
		case 0x59: AbsoluteY                    (StageType(EOR)); break;
		case 0x5A: StackPush                    (StageType(PHY)); break;
		case 0x5B: Implied                      (StageType(TCD)); break;
		case 0x5C: AbsoluteLongJMP              (StageType(JMP)); break;
		case 0x5D: AbsoluteX                    (StageType(EOR)); break;
		case 0x5E: AbsoluteXRMW                 (StageType(LSR)); break;
		case 0x5F: AbsoluteXLong                (StageType(EOR)); break;
		case 0x60: StackRTS                     (StageType(RTS)); break;
		case 0x61: DirectXIndirect              (StageType(ADC)); break;
		case 0x62: StackPER                     (StageType(PER)); break;
		case 0x63: StackRelative                (StageType(ADC)); break;
		case 0x64: DirectWrite                  (StageType(STZ)); break;
		case 0x65: Direct                       (StageType(ADC)); break;
		case 0x66: DirectRMW                    (StageType(ROR)); break;
		case 0x67: DirectIndirectLong           (StageType(ADC)); break;
		case 0x68: StackPop                     (StageType(PLA)); break;
		case 0x69: Immediate                    (StageType(ADC)); break;
		case 0x6A: Accumulator                  (StageType(ROR)); break;
		case 0x6B: StackRTL                     (StageType(RTL)); break;
		case 0x6C: AbsoluteIndirectJMP          (StageType(JMP)); break;
		case 0x6D: Absolute                     (StageType(ADC)); break;
		case 0x6E: AbsoluteRMW                  (StageType(ROR)); break;
		case 0x6F: AbsoluteLong                 (StageType(ADC)); break;
		case 0x70: RelativeBranch               (StageType(BVS)); break;
		case 0x71: DirectIndirectY              (StageType(ADC)); break;
		case 0x72: DirectIndirect               (StageType(ADC)); break;
		case 0x73: StackRelativeIndirectY       (StageType(ADC)); break;
		case 0x74: DirectXWrite                 (StageType(STZ)); break;
		case 0x75: DirectX                      (StageType(ADC)); break;
		case 0x76: DirectXRMW                   (StageType(ROR)); break;
		case 0x77: DirectIndirectYLong          (StageType(ADC)); break;
		case 0x78: Implied                      (StageType(SEI)); break;
		case 0x79: AbsoluteY                    (StageType(ADC)); break;
		case 0x7A: StackPop                     (StageType(PLY)); break;
		case 0x7B: Implied                      (StageType(TDC)); break;
		case 0x7C: AbsoluteXIndirectJMP         (StageType(JMP)); break;
		case 0x7D: AbsoluteX                    (StageType(ADC)); break;
		case 0x7E: AbsoluteXRMW                 (StageType(ROR)); break;
		case 0x7F: AbsoluteXLong                (StageType(ADC)); break;
		case 0x80: RelativeBranch               (StageType(BRA)); break;
		case 0x81: DirectXIndirectWrite         (StageType(STA)); break;
		case 0x82: RelativeBranchLong           (StageType(BRL)); break;
		case 0x83: StackRelativeWrite           (StageType(STA)); break;
		case 0x84: DirectWrite                  (StageType(STY)); break;
		case 0x85: DirectWrite                  (StageType(STA)); break;
		case 0x86: DirectWrite                  (StageType(STX)); break;
		case 0x87: DirectIndirectLongWrite      (StageType(STA)); break;
		case 0x88: Implied                      (StageType(DEY)); break;
		case 0x89: Immediate                    (StageType(BIT)); break;
		case 0x8A: Implied                      (StageType(TXA)); break;
		case 0x8B: StackPush8                   (StageType(PHB)); break;
		case 0x8C: AbsoluteWrite                (StageType(STY)); break;
		case 0x8D: AbsoluteWrite                (StageType(STA)); break;
		case 0x8E: AbsoluteWrite                (StageType(STX)); break;
		case 0x8F: AbsoluteLongWrite            (StageType(STA)); break;
		case 0x90: RelativeBranch               (StageType(BCC)); break;
		case 0x91: DirectIndirectYWrite         (StageType(STA)); break;
		case 0x92: DirectIndirectWrite          (StageType(STA)); break;
		case 0x93: StackRelativeIndirectYWrite  (StageType(STA)); break;
		case 0x94: DirectXWrite                 (StageType(STY)); break;
		case 0x95: DirectXWrite                 (StageType(STA)); break;
		case 0x96: DirectYWrite                 (StageType(STX)); break;
		case 0x97: DirectIndirectYLongWrite     (StageType(STA)); break;
		case 0x98: Implied                      (StageType(TYA)); break;
		case 0x99: AbsoluteYWrite               (StageType(STA)); break;
		case 0x9A: Implied                      (StageType(TXS)); break;
		case 0x9B: Implied                      (StageType(TXY)); break;
		case 0x9C: AbsoluteWrite                (StageType(STZ)); break;
		case 0x9D: AbsoluteXWrite               (StageType(STA)); break;
		case 0x9E: AbsoluteXWrite               (StageType(STZ)); break;
		case 0x9F: AbsoluteXLongWrite           (StageType(STA)); break;
		case 0xA0: Immediate                    (StageType(LDY)); break;
		case 0xA1: DirectXIndirect              (StageType(LDA)); break;
		case 0xA2: Immediate                    (StageType(LDX)); break;
		case 0xA3: StackRelative                (StageType(LDA)); break;
		case 0xA4: Direct                       (StageType(LDY)); break;
		case 0xA5: Direct                       (StageType(LDA)); break;
		case 0xA6: Direct                       (StageType(LDX)); break;
		case 0xA7: DirectIndirectLong           (StageType(LDA)); break;
		case 0xA8: Implied                      (StageType(TAY)); break;
		case 0xA9: Immediate                    (StageType(LDA)); break;
		case 0xAA: Implied                      (StageType(TAX)); break;
		case 0xAB: StackPop8                    (StageType(PLB)); break;
		case 0xAC: Absolute                     (StageType(LDY)); break;
		case 0xAD: Absolute                     (StageType(LDA)); break;
		case 0xAE: Absolute                     (StageType(LDX)); break;
		case 0xAF: AbsoluteLong                 (StageType(LDA)); break;
		case 0xB0: RelativeBranch               (StageType(BCS)); break;
		case 0xB1: DirectIndirectY              (StageType(LDA)); break;
		case 0xB2: DirectIndirect               (StageType(LDA)); break;
		case 0xB3: StackRelativeIndirectY       (StageType(LDA)); break;
		case 0xB4: DirectX                      (StageType(LDY)); break;
		case 0xB5: DirectX                      (StageType(LDA)); break;
		case 0xB6: DirectY                      (StageType(LDX)); break;
		case 0xB7: DirectIndirectYLong          (StageType(LDA)); break;
		case 0xB8: Implied                      (StageType(CLV)); break;
		case 0xB9: AbsoluteY                    (StageType(LDA)); break;
		case 0xBA: Implied                      (StageType(TSX)); break;
		case 0xBB: Implied                      (StageType(TYX)); break;
		case 0xBC: AbsoluteX                    (StageType(LDY)); break;
		case 0xBD: AbsoluteX                    (StageType(LDA)); break;
		case 0xBE: AbsoluteY                    (StageType(LDX)); break;
		case 0xBF: AbsoluteXLong                (StageType(LDA)); break;
		case 0xC0: Immediate                    (StageType(CPY)); break;
		case 0xC1: DirectXIndirect              (StageType(CMP)); break;
		case 0xC2: ImmediateSpecial             (StageType(REP)); break;
		case 0xC3: StackRelative                (StageType(CMP)); break;
		case 0xC4: Direct                       (StageType(CPY)); break;
		case 0xC5: Direct                       (StageType(CMP)); break;
		case 0xC6: DirectRMW                    (StageType(DEC)); break;
		case 0xC7: DirectIndirectLong           (StageType(CMP)); break;
		case 0xC8: Implied                      (StageType(INY)); break;
		case 0xC9: Immediate                    (StageType(CMP)); break;
		case 0xCA: Implied                      (StageType(DEX)); break;
		case 0xCB: ImpliedSpecial               (StageType(WAI)); break;
		case 0xCC: Absolute                     (StageType(CPY)); break;
		case 0xCD: Absolute                     (StageType(CMP)); break;
		case 0xCE: AbsoluteRMW                  (StageType(DEC)); break;
		case 0xCF: AbsoluteLong                 (StageType(CMP)); break;
		case 0xD0: RelativeBranch               (StageType(BNE)); break;
		case 0xD1: DirectIndirectY              (StageType(CMP)); break;
		case 0xD2: DirectIndirect               (StageType(CMP)); break;
		case 0xD3: StackRelativeIndirectY       (StageType(CMP)); break;
		case 0xD4: StackPEI                     (StageType(PEI)); break;
		case 0xD5: DirectX                      (StageType(CMP)); break;
		case 0xD6: DirectXRMW                   (StageType(DEC)); break;
		case 0xD7: DirectIndirectYLong          (StageType(CMP)); break;
		case 0xD8: Implied                      (StageType(CLD)); break;
		case 0xD9: AbsoluteY                    (StageType(CMP)); break;
		case 0xDA: StackPush                    (StageType(PHX)); break;
		case 0xDB: ImpliedSpecial               (StageType(STP)); break;
		case 0xDC: AbsoluteIndirectJML          (StageType(JML)); break;
		case 0xDD: AbsoluteX                    (StageType(CMP)); break;
		case 0xDE: AbsoluteXRMW                 (StageType(DEC)); break;
		case 0xDF: AbsoluteXLong                (StageType(CMP)); break;
		case 0xE0: Immediate                    (StageType(CPX)); break;
		case 0xE1: DirectXIndirect              (StageType(SBC)); break;
		case 0xE2: ImmediateSpecial             (StageType(SEP)); break;
		case 0xE3: StackRelative                (StageType(SBC)); break;
		case 0xE4: Direct                       (StageType(CPX)); break;
		case 0xE5: Direct                       (StageType(SBC)); break;
		case 0xE6: DirectRMW                    (StageType(INC)); break;
		case 0xE7: DirectIndirectLong           (StageType(SBC)); break;
		case 0xE8: Implied                      (StageType(INX)); break;
		case 0xE9: Immediate                    (StageType(SBC)); break;
		case 0xEA: Implied                      (StageType(NOP)); break;
		case 0xEB: ImpliedSpecial               (StageType(XBA)); break;
		case 0xEC: Absolute                     (StageType(CPX)); break;
		case 0xED: Absolute                     (StageType(SBC)); break;
		case 0xEE: AbsoluteRMW                  (StageType(INC)); break;
		case 0xEF: AbsoluteLong                 (StageType(SBC)); break;
		case 0xF0: RelativeBranch               (StageType(BEQ)); break;
		case 0xF1: DirectIndirectY              (StageType(SBC)); break;
		case 0xF2: DirectIndirect               (StageType(SBC)); break;
		case 0xF3: StackRelativeIndirectY       (StageType(SBC)); break;
		case 0xF4: StackPEA                     (StageType(PEA)); break;
		case 0xF5: DirectX                      (StageType(SBC)); break;
		case 0xF6: DirectXRMW                   (StageType(INC)); break;
		case 0xF7: DirectIndirectYLong          (StageType(SBC)); break;
		case 0xF8: Implied                      (StageType(SED)); break;
		case 0xF9: AbsoluteY                    (StageType(SBC)); break;
		case 0xFA: StackPop                     (StageType(PLX)); break;
		case 0xFB: Implied                      (StageType(XCE)); break;
		case 0xFC: AbsoluteXIndirectJSR         (StageType(JSR)); break;
		case 0xFD: AbsoluteX                    (StageType(SBC)); break;
		case 0xFE: AbsoluteXRMW                 (StageType(INC)); break;
		case 0xFF: AbsoluteXLong                (StageType(SBC)); break;
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

string W65816::getASM()
{
	return asmLine;
}