#include "W65816.h"

void W65816::initializeOpcodes()
{
	decodingTable[0x03] = Instruction("ORA", StackRelative, ORA);
	decodingTable[0x13] = Instruction("ORA", StackRelativeIndirectY, ORA);
	decodingTable[0x23] = Instruction("AND", StackRelative, AND);
	decodingTable[0x33] = Instruction("AND", StackRelativeIndirectY, AND);
	decodingTable[0x40] = Instruction("RTI", StackRTI, dummyStage);
	decodingTable[0x43] = Instruction("EOR", StackRelative, EOR);
	decodingTable[0x44] = Instruction("MVP", BlockMoveP, dummyStage);
	decodingTable[0x53] = Instruction("EOR", StackRelativeIndirectY, EOR);
	decodingTable[0x54] = Instruction("MVN", BlockMoveN, dummyStage);
	decodingTable[0x60] = Instruction("RTS", StackRTS, dummyStage);
	decodingTable[0x62] = Instruction("PER", StackPER, dummyStage);
	decodingTable[0x63] = Instruction("ADC", StackRelative, ADC);
	decodingTable[0x6B] = Instruction("RTL", StackRTL, dummyStage);
	decodingTable[0x73] = Instruction("ADC", StackRelativeIndirectY, ADC);
	decodingTable[0x83] = Instruction("STA", StackRelativeWrite, STA);
	decodingTable[0x93] = Instruction("STA", StackRelativeIndirectYWrite, STA);
	decodingTable[0xA3] = Instruction("LDA", StackRelative, LDA);
	decodingTable[0xB3] = Instruction("LDA", StackRelativeIndirectY, LDA);
	decodingTable[0xC3] = Instruction("CMP", StackRelative, CMP);
	decodingTable[0xD3] = Instruction("CMP", StackRelativeIndirectY, CMP);
	decodingTable[0xD4] = Instruction("PEI", StackPEI, dummyStage);
	decodingTable[0xE3] = Instruction("SBC", StackRelative, SBC);
	decodingTable[0xF3] = Instruction("SBC", StackRelativeIndirectY, SBC);
	decodingTable[0xF4] = Instruction("PEA", StackPEA, dummyStage);
}
