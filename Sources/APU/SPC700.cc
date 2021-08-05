#include "SPC700.h"

#include "SNES_APU.h"

SPC700::SPC700()
{
	//ctor
}

void SPC700::attachBus(SNES_APU * bus)
{
	apu_bus = bus;
}

uint8_t SPC700::read(uint16_t adr)
{
	return apu_bus->read(adr);
}

void SPC700::write(uint16_t adr, uint8_t data)
{
	apu_bus->write(adr, data);
}

void SPC700::reset()
{
	pc = read(0xFFFE);
	pc |= (uint16_t(read(0xFFFF))<<8);

	a = x = y = sp = psw.val = 0;
	halt_cpu = false;
	idb8_ext = 0;
	idb8 = 0;
	inst_cycles = inst_length = currentCycle = 0;
}

void SPC700::updateNZflags(uint8_t data)
{
	psw.setN((data>>7)&1);
	psw.setZ(data == 0);
}

void SPC700::updateNZflags(uint8_t high, uint8_t data)
{
	psw.setN((high>>7)&1);
	psw.setZ(data == 0 && high == 0);
}

uint16_t SPC700::directAddress(uint8_t adr)
{
	return (uint16_t(psw.P())<<8)|adr;
}

void SPC700::doMemOperation(MemoryDirection dir, uint8_t* left, uint8_t* right)
{
	if(dir == ReadMemory)
		*left = *right;
	else
		*right = *left;
}

void SPC700::DummyInst()
{
	; //Empty
}

uint8_t SPC700::CarryFromBit(uint16_t a,uint16_t b,uint16_t res,uint8_t bit)
{
	uint16_t mask = 1 << bit;
	a &= mask;
	b &= mask;
	res &= mask;
	if(a & b)
		return 1;
	if(a^b)
		return ((~res)>>bit)&1;
	return 0;
}

uint8_t SPC700::BorrowFromBit(uint16_t a, uint16_t b, uint16_t res, uint8_t bit)
{
	//a-b=res
	uint16_t mask = 1 << bit;
	a &= mask;
	b &= mask;
	res &= mask;
	if(a==0 && b == 0)
		return res>>bit; //set it back to bit 0 pos
	if(a & b)
		return res>>bit;
	if(a==0 && b!=0)
		return 1;
	return 0;
}

void SPC700::checkSignedOverflow(int a, int b, int c)
{
	psw.setV((a == 0 && b == 0 && c == 1) || (a == 1 && b == 1 && c == 0));
}

void SPC700::push(uint8_t data)
{
	write(make16(0x01,sp),data);
	--sp;
}

uint8_t SPC700::pop()
{
	++sp;
	return read(make16(0x01,sp));
}

uint16_t SPC700::make16(uint8_t high, uint8_t low)
{
	uint16_t word = low;
	word |= (uint16_t(high)<<8);
	return word;
}

void SPC700::tick()
{
	if(halt_cpu) return;
	if(currentCycle != 0) //Waiting for end of current instruction
	{
		--currentCycle;
		return;
	}

	//Decoding
	inst_cycles = 0;
	inst_length = 0;
	memoryDirection = ReadMemory;

	uint8_t opcode = read(pc);

	uint8_t right = opcode&0x0F;
	uint8_t left = opcode >> 4;
	if(left <= 0xB && right >= 0x04 && right <= 0x09) //Manual decode
	{
		bool leftOdd = left&1;
		if(leftOdd) --left;

		auto operation = OR; //Dummy init
		switch(left)
		{
		case 0x00: operation = OR;  break;
		case 0x02: operation = AND; break;
		case 0x04: operation = EOR; break;
		case 0x06: operation = CMP; break;
		case 0x08: operation = ADC; break;
		case 0x0A: operation = SBC; break;
		default: cout << "[APU][SPC700] error manual decode !"<<endl;
		}

		if(leftOdd)
		{
			switch(right)
			{
				case 0x04: DirectIndexedX(operation);           break;
				case 0x05: AbsoluteIndexedX(operation);         break;
				case 0x06: AbsoluteIndexedY(operation);         break;
				case 0x07: DirectIndirectIndexed(operation);    break;
				case 0x08: Direct_Immediate(operation);         break;
				case 0x09: DirectX_DirectY(operation);          break;
			}
		}
		else
		{
			switch(right)
			{
				case 0x04: Direct(operation);                   break;
				case 0x05: Absolute(operation);                 break;
				case 0x06: DirectX(operation);                  break;
				case 0x07: DirectIndexedIndirect(operation);    break;
				case 0x08: Immediate(operation);                break;
				case 0x09: Direct_Direct(operation);            break;
			}
		}
	}
	else if(left <= 0xB && right >= 0x0B && right <= 0x0C)
	{
		bool leftOdd = left&1;
		if(leftOdd) --left;

		auto operation = OR; //Dummy init
		switch(left)
		{
		case 0x00: operation = ASL;  break;
		case 0x02: operation = ROL; break;
		case 0x04: operation = LSR; break;
		case 0x06: operation = ROR; break;
		case 0x08: operation = DEC; break;
		case 0x0A: operation = INC; break;
		default: cout << "[APU][SPC700] error manual decode 2!"<<endl;
		}

		if(leftOdd)
		{
			switch(right)
			{
				case 0x0B: DirectIndexedXRMW(operation);    break;
				case 0x0C: AccRMW(operation);               break;
			}
		}
		else
		{
			switch(right)
			{
				case 0x0B: DirectRMW(operation);            break;
				case 0x0C: AbsoluteRMW(operation);          break;
			}
		}
	}
	else if(right == 0x03)
	{
		if(left&1)//odd
			BranchTestMem(BBC);
		else
			BranchTestMem(BBS);
	}
	else if(right == 0x01)
	{
		Implied(TCALL);
	}
	else if(right == 0x02)
	{
		if(left&1)//odd
			DirectRMW(CLR1);
		else
			DirectRMW(SET1);
	}
	else
	{
		switch(opcode)
		{
		case 0xE8: Immediate(MOVA);                     break;
		case 0xCD: Immediate(MOVX);                     break;
		case 0x8D: Immediate(MOVY);                     break;
		case 0x7D: Implied(MOVAX);                      break;
		case 0x5D: Implied(MOVXA);                      break;
		case 0xDD: Implied(MOVAY);                      break;
		case 0xFD: Implied(MOVYA);                      break;
		case 0x9D: Implied(MOVXSP);                     break;
		case 0xBD: Implied(MOVSPX);                     break;
		case 0xE4: Direct(MOVA);                        break;
		case 0xF4: DirectIndexedX(MOVA);                break;
		case 0xE5: Absolute(MOVA);                      break;
		case 0xF5: AbsoluteIndexedX(MOVA);              break;
		case 0xF6: AbsoluteIndexedY(MOVA);              break;
		case 0xE6: DirectX(MOVA);                       break;
		case 0xBF: DirectXInc(MOVA);                    break;
		case 0xF7: DirectIndirectIndexed(MOVA);         break;
		case 0xE7: DirectIndexedIndirect(MOVA);         break;
		case 0xF8: Direct(MOVX);                        break;
		case 0xF9: DirectIndexedY(MOVX);                break;
		case 0xE9: Absolute(MOVX);                      break;
		case 0xEB: Direct(MOVY);                        break;
		case 0xFB: DirectIndexedX(MOVY);                break;
		case 0xEC: Absolute(MOVY);                      break;
		case 0xBA: Direct16(MOVW_YA);                   break;
		case 0x8F: DirectWriteImmediate(DummyInst);     break;//Different kind
		case 0xFA: DirectTransfer(DummyInst);           break;//Different kind
		case 0xC4: DirectWrite(MOVA);                   break;
		case 0xD8: DirectWrite(MOVX);                   break;
		case 0xCB: DirectWrite(MOVY);                   break;
		case 0xD4: DirectIndexedXWrite(MOVA);           break;
		case 0xDB: DirectIndexedXWrite(MOVY);           break;
		case 0xD9: DirectIndexedYWrite(MOVX);           break;
		case 0xC5: AbsoluteWrite(MOVA);                 break;
		case 0xC9: AbsoluteWrite(MOVX);                 break;
		case 0xCC: AbsoluteWrite(MOVY);                 break;
		case 0xD5: AbsoluteIndexedXWrite(MOVA);         break;
		case 0xD6: AbsoluteIndexedYWrite(MOVA);         break;
		case 0xAF: DirectXIncWrite(MOVA);               break;
		case 0xC6: DirectXWrite(MOVA);                  break;
		case 0xD7: DirectIndirectIndexedWrite(MOVA);    break;
		case 0xC7: DirectIndexedIndirectWrite(MOVA);    break;
		case 0xDA: Direct16Write(MOVW_YA);              break;
		case 0x2D: Push(MOVA);                          break;
		case 0x4D: Push(MOVX);                          break;
		case 0x6D: Push(MOVY);                          break;
		case 0x0D: Push(MOV_PSW);                       break;
		case 0xAE: Pop(MOVA);                           break;
		case 0xCE: Pop(MOVX);                           break;
		case 0xEE: Pop(MOVY);                           break;
		case 0x8E: Pop(MOV_PSW);                        break;
		case 0xC8: Immediate(CMPX);                     break;
		case 0x3E: Direct(CMPX);                        break;
		case 0x1E: Absolute(CMPX);                      break;
		case 0xAD: Immediate(CMPY);                     break;
		case 0x7E: Direct(CMPY);                        break;
		case 0x5E: Absolute(CMPY);                      break;
		case 0x3D: XRMW(INC);                           break;
		case 0xFC: YRMW(INC);                           break;
		case 0x1D: XRMW(DEC);                           break;
		case 0xDC: YRMW(DEC);                           break;
		case 0x10: Branch(BPL);                         break;
		case 0x30: Branch(BMI);                         break;
		case 0x50: Branch(BVC);                         break;
		case 0x70: Branch(BVS);                         break;
		case 0x90: Branch(BCC);                         break;
		case 0xB0: Branch(BCS);                         break;
		case 0xD0: Branch(BNE);                         break;
		case 0xF0: Branch(BEQ);                         break;
		case 0x2E: BranchTestMem(CBNE);                 break;
		case 0xDE: BranchTestMemIndexedX(CBNE);         break;
		case 0xFE: Branch(DBNZ);                        break;
		case 0x6E: BranchTestMem(DBNZ_MEM);             break;
		case 0x2F: Branch(BRA);                         break;
		case 0x5F: Immediate16(JMP);                    break;
		case 0x1F: JumpAbsoluteIndexedX(DummyInst);     break;
		case 0x3F: Immediate16(CALL);                   break;
		case 0x4F: Implied(PCALL);                      break; //Actually it's Immediate but it doesn't change the psw reg, so the operand will be handled in the inst
		case 0x6F: Implied(RET);                        break;
		case 0x7F: Implied(RETI);                       break;
		case 0x0F: Implied(BRK);                        break;
		case 0x00: Implied(NOP);                        break;
		case 0x20: Implied(CLRP);                       break;
		case 0x40: Implied(SETP);                       break;
		case 0xA0: Implied(EI);                         break;
		case 0xC0: Implied(DI);                         break;
		case 0xEF: Implied(SLEEP);                      break;
		case 0xFF: Implied(HALT);                       break;
		case 0xEA: MemBitRMW(NOT1);                     break;
		case 0xCA: MemBitRMW(MOV1);                     break;
		case 0xAA: MemBit(MOV1);                        break;
		case 0x0A: MemBit(OR1);                         break;
		case 0x2A: MemBit(OR1_NOT);                     break;
		case 0x4A: MemBit(AND1);                        break;
		case 0x6A: MemBit(AND1_NOT);                    break;
		case 0x8A: MemBit(EOR1);                        break;
		case 0x60: Implied(CLRC);                       break;
		case 0x80: Implied(SETC);                       break;
		case 0xED: Implied(NOTC);                       break;
		case 0xE0: Implied(CLRVH);                      break;
		case 0x9F: Implied(XCN);                        break;
		case 0x4E: AbsoluteRMW(TCLR1);                  break; //TODO:This inst is supposed to ignore bit 15 of the address but that would require a new adrMode so I don't do it
		case 0x0E: AbsoluteRMW(TSET1);                  break;
		case 0xDF: Implied(DAA);                        break;
		case 0xBE: Implied(DAS);                        break;
		case 0x7A: Direct16(ADDW);                      break;
		case 0x9A: Direct16(SUBW);                      break;
		case 0x5A: Direct16(CMPW);                      break;
		case 0x3A: DirectRMW16(INCW);                   break;
		case 0x1A: DirectRMW16(DECW);                   break;
		case 0xCF: Implied(MUL);                        break;
		case 0x9E: Implied(DIV);                        break;
		default:
			cout<<"[APU][SPC700] error: unknown opcode:"<<std::hex<<(int)opcode<<endl;
		}
	}

	pc += inst_length;
	currentCycle = inst_cycles;

	//cout<<"[APU][SPC700] inst:"<<asm_inst<<" pc="<<std::hex<<int(pc)<<endl;
}
