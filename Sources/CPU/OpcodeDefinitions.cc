#include "W65816.h"

void W65816::initializeOpcodes()
{
    decodingTable[0x03] = Instruction("ORA", StackRelative, ORA);
    decodingTable[0x08] = Instruction("PHP", StackPush8, PHP);
    decodingTable[0x0B] = Instruction("PHD", StackPush16, PHD);
    decodingTable[0x13] = Instruction("ORA", StackRelativeIndirectY, ORA);
    decodingTable[0x23] = Instruction("AND", StackRelative, AND);
    decodingTable[0x28] = Instruction("PLP", StackPop8, PLP);
    decodingTable[0x2B] = Instruction("PLD", StackPop16, PLD);
    decodingTable[0x33] = Instruction("AND", StackRelativeIndirectY, AND);
    decodingTable[0x40] = Instruction("RTI", StackRTI, dummyStage);
    decodingTable[0x43] = Instruction("EOR", StackRelative, EOR);
    decodingTable[0x44] = Instruction("MVP", BlockMoveP, dummyStage);
    decodingTable[0x48] = Instruction("PHA", StackPush, PHA);
    decodingTable[0x4B] = Instruction("PHK", StackPush8, PHK);
    decodingTable[0x53] = Instruction("EOR", StackRelativeIndirectY, EOR);
    decodingTable[0x54] = Instruction("MVN", BlockMoveN, dummyStage);
    decodingTable[0x5A] = Instruction("PHY", StackPush, PHY); decodingTable[0x5A].setIsIndexRelated(true);
    decodingTable[0x60] = Instruction("RTS", StackRTS, dummyStage);
    decodingTable[0x62] = Instruction("PER", StackPER, dummyStage);
    decodingTable[0x63] = Instruction("ADC", StackRelative, ADC);
    decodingTable[0x68] = Instruction("PLA", StackPop, PLA);
    decodingTable[0x6B] = Instruction("RTL", StackRTL, dummyStage);
    decodingTable[0x73] = Instruction("ADC", StackRelativeIndirectY, ADC);
    decodingTable[0x7A] = Instruction("PLY", StackPop, PLY); decodingTable[0x7A].setIsIndexRelated(true);
    decodingTable[0x83] = Instruction("STA", StackRelativeWrite, STA);
    decodingTable[0x8B] = Instruction("PHB", StackPush8, PHB);
    decodingTable[0x93] = Instruction("STA", StackRelativeIndirectYWrite, STA);
    decodingTable[0xA3] = Instruction("LDA", StackRelative, LDA);
    decodingTable[0xAB] = Instruction("PLB", StackPop8, PLB);
    decodingTable[0xB3] = Instruction("LDA", StackRelativeIndirectY, LDA);
    decodingTable[0xC3] = Instruction("CMP", StackRelative, CMP);
    decodingTable[0xCB] = Instruction("WAI", ImpliedSpecial, WAI);
    decodingTable[0xD3] = Instruction("CMP", StackRelativeIndirectY, CMP);
    decodingTable[0xD4] = Instruction("PEI", StackPEI, dummyStage);
    decodingTable[0xDA] = Instruction("PHX", StackPush, PHX); decodingTable[0xDA].setIsIndexRelated(true);
    decodingTable[0xDB] = Instruction("STP", ImpliedSpecial, STP);
    decodingTable[0xE3] = Instruction("SBC", StackRelative, SBC);
    decodingTable[0xEB] = Instruction("XBA", ImpliedSpecial, XBA);
    decodingTable[0xF3] = Instruction("SBC", StackRelativeIndirectY, SBC);
    decodingTable[0xF4] = Instruction("PEA", StackPEA, dummyStage);
    decodingTable[0xFA] = Instruction("PLX", StackPop, PLX); decodingTable[0xFA].setIsIndexRelated(true);
}
