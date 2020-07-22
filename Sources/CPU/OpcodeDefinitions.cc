#include "W65816.h"

void W65816::initializeOpcodes()
{
    decodingTable[0x09] = Instruction("ORA", Immediate, ORA);
    decodingTable[0x0D] = Instruction("ORA", Absolute, ORA);
    decodingTable[0x18] = Instruction("CLC", Implied, CLC);
    decodingTable[0x1B] = Instruction("TCS", Implied, TCS);
    decodingTable[0x29] = Instruction("AND", Immediate, AND);
    decodingTable[0x2C] = Instruction("BIT", Absolute, BIT);
    decodingTable[0x2D] = Instruction("AND", Absolute, AND);
    decodingTable[0x38] = Instruction("SEC", Implied, SEC);
    decodingTable[0x3B] = Instruction("TSC", Implied, TSC);
    decodingTable[0x49] = Instruction("EOR", Immediate, EOR);
    decodingTable[0x4D] = Instruction("EOR", Absolute, EOR);
    decodingTable[0x58] = Instruction("CLI", Implied, CLI);
    decodingTable[0x5B] = Instruction("TCD", Implied, TCD);
    decodingTable[0x6D] = Instruction("ADC", Absolute, ADC);
    decodingTable[0x69] = Instruction("ADC", Immediate, ADC);
    decodingTable[0x78] = Instruction("SEI", Implied, SEI);
    decodingTable[0x7B] = Instruction("TDC", Implied, TDC);
    decodingTable[0x88] = Instruction("DEY", Implied, DEY);
    decodingTable[0x89] = Instruction("BIT", Immediate, BIT);
    decodingTable[0x8A] = Instruction("TXA", Implied, TXA);
    decodingTable[0x98] = Instruction("TYA", Implied, TYA);
    decodingTable[0x9A] = Instruction("TXS", Implied, TXS);
    decodingTable[0x9B] = Instruction("TXY", Implied, TXY);
    decodingTable[0xA0] = Instruction("LDY", Immediate, LDY); decodingTable[0xA0].setIsIndexRelated(true);
    decodingTable[0xA2] = Instruction("LDX", Immediate, LDX); decodingTable[0xA2].setIsIndexRelated(true);
    decodingTable[0xA9] = Instruction("LDA", Immediate, LDA);
    decodingTable[0xAA] = Instruction("TAX", Implied, TAX);
    decodingTable[0xAB] = Instruction("TAY", Implied, TAY);
    decodingTable[0xAC] = Instruction("LDY", Absolute, LDY); decodingTable[0xAC].setIsIndexRelated(true);
    decodingTable[0xAD] = Instruction("LDA", Absolute, LDA);
    decodingTable[0xAE] = Instruction("LDX", Absolute, LDX); decodingTable[0xAE].setIsIndexRelated(true);
    decodingTable[0xB8] = Instruction("CLV", Implied, CLV);
    decodingTable[0xBA] = Instruction("TSX", Implied, TSX);
    decodingTable[0xBB] = Instruction("TYX", Implied, TYX);
    decodingTable[0xC0] = Instruction("CPY", Immediate, CPY); decodingTable[0xC0].setIsIndexRelated(true);
    decodingTable[0xC2] = Instruction("REP", ImmediateSpecial, REP);
    decodingTable[0xC8] = Instruction("INY", Implied, INY);
    decodingTable[0xC9] = Instruction("CMP", Immediate, CMP);
    decodingTable[0xCA] = Instruction("DEX", Implied, DEX);
    decodingTable[0xCC] = Instruction("CPY", Absolute, CPY); decodingTable[0xCC].setIsIndexRelated(true);
    decodingTable[0xCD] = Instruction("CMP", Absolute, CMP);
    decodingTable[0xD8] = Instruction("CLD", Implied, CLD);
    decodingTable[0xE0] = Instruction("CPX", Immediate, CPX); decodingTable[0xE0].setIsIndexRelated(true);
    decodingTable[0xE2] = Instruction("SEP", ImmediateSpecial, SEP);
    decodingTable[0xE8] = Instruction("INX", Implied, INX);
    decodingTable[0xE9] = Instruction("SBC", Immediate, SBC);
    decodingTable[0xEA] = Instruction("NOP", Implied, dummyStage);
    decodingTable[0xEB] = Instruction("XBA", ImpliedSpecial, XBA);
    decodingTable[0xEC] = Instruction("CPX", Absolute, CPX); decodingTable[0xEC].setIsIndexRelated(true);
    decodingTable[0xED] = Instruction("SBC", Absolute, SBC);
    decodingTable[0xF8] = Instruction("SED", Implied, SED);
    decodingTable[0xFB] = Instruction("XCE", Implied, XCE);
}
