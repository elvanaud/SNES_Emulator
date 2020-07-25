#include "W65816.h"

void W65816::initializeOpcodes()
{
    decodingTable[0x09] = Instruction("ORA", Immediate, ORA);
    decodingTable[0x0A] = Instruction("ASL", Accumulator, ASL);
    decodingTable[0x0C] = Instruction("TSB", AbsoluteRMW, TSB);
    decodingTable[0x0D] = Instruction("ORA", Absolute, ORA);
    decodingTable[0x0E] = Instruction("ASL", AbsoluteRMW, ASL);
    decodingTable[0x0F] = Instruction("ORA", AbsoluteLong, ORA);
    decodingTable[0x18] = Instruction("CLC", Implied, CLC);
    decodingTable[0x19] = Instruction("ORA", AbsoluteY, ORA);
    decodingTable[0x1A] = Instruction("INC", Accumulator, INC);
    decodingTable[0x1B] = Instruction("TCS", Implied, TCS);
    decodingTable[0x1C] = Instruction("TRB", AbsoluteRMW, TRB);
    decodingTable[0x1D] = Instruction("ORA", AbsoluteX, ORA);
    decodingTable[0x1E] = Instruction("ASL", AbsoluteXRMW, ASL);
    decodingTable[0x1F] = Instruction("ORA", AbsoluteXLong, ORA);
    decodingTable[0x20] = Instruction("JSR", AbsoluteJSR, dummyStage);
    decodingTable[0x22] = Instruction("JSL", AbsoluteLongJSL, dummyStage);
    decodingTable[0x29] = Instruction("AND", Immediate, AND);
    decodingTable[0x2A] = Instruction("ROL", Accumulator, ROL);
    decodingTable[0x2C] = Instruction("BIT", Absolute, BIT);
    decodingTable[0x2D] = Instruction("AND", Absolute, AND);
    decodingTable[0x2E] = Instruction("ROL", AbsoluteRMW, ROL);
    decodingTable[0x2F] = Instruction("AND", AbsoluteLong, AND);
    decodingTable[0x38] = Instruction("SEC", Implied, SEC);
    decodingTable[0x39] = Instruction("AND", AbsoluteY, AND);
    decodingTable[0x3A] = Instruction("DEC", Accumulator, DEC);
    decodingTable[0x3B] = Instruction("TSC", Implied, TSC);
    decodingTable[0x3C] = Instruction("BIT", AbsoluteX, BIT);
    decodingTable[0x3D] = Instruction("AND", AbsoluteX, AND);
    decodingTable[0x3E] = Instruction("ROL", AbsoluteXRMW, ROL);
    decodingTable[0x3F] = Instruction("AND", AbsoluteXLong, AND);
    decodingTable[0x49] = Instruction("EOR", Immediate, EOR);
    decodingTable[0x4A] = Instruction("LSR", Accumulator, LSR);
    decodingTable[0x4C] = Instruction("JMP", AbsoluteJMP, dummyStage);
    decodingTable[0x4D] = Instruction("EOR", Absolute, EOR);
    decodingTable[0x4E] = Instruction("LSR", AbsoluteRMW, LSR);
    decodingTable[0x4F] = Instruction("EOR", AbsoluteLong, EOR);
    decodingTable[0x58] = Instruction("CLI", Implied, CLI);
    decodingTable[0x59] = Instruction("EOR", AbsoluteY, EOR);
    decodingTable[0x5B] = Instruction("TCD", Implied, TCD);
    decodingTable[0x5C] = Instruction("JMP", AbsoluteLongJMP, dummyStage);
    decodingTable[0x5D] = Instruction("EOR", AbsoluteX, EOR);
    decodingTable[0x5E] = Instruction("LSR", AbsoluteXRMW, LSR);
    decodingTable[0x5F] = Instruction("EOR", AbsoluteXLong, EOR);
    decodingTable[0x69] = Instruction("ADC", Immediate, ADC);
    decodingTable[0x6A] = Instruction("ROR", Accumulator, ROR);
    decodingTable[0x6C] = Instruction("JMP", AbsoluteIndirectJMP, dummyStage);
    decodingTable[0x6D] = Instruction("ADC", Absolute, ADC);
    decodingTable[0x6E] = Instruction("ROR", AbsoluteRMW, ROR);
    decodingTable[0x6F] = Instruction("ADC", AbsoluteLong, ADC);
    decodingTable[0x78] = Instruction("SEI", Implied, SEI);
    decodingTable[0x79] = Instruction("ADC", AbsoluteY, ADC);
    decodingTable[0x7B] = Instruction("TDC", Implied, TDC);
    decodingTable[0x7C] = Instruction("JMP", AbsoluteXIndirectJMP, dummyStage);
    decodingTable[0x7D] = Instruction("ADC", AbsoluteX, ADC);
    decodingTable[0x7E] = Instruction("ROR", AbsoluteXRMW, ROR);
    decodingTable[0x7F] = Instruction("ADC", AbsoluteXLong, ADC);
    decodingTable[0x88] = Instruction("DEY", Implied, DEY);
    decodingTable[0x89] = Instruction("BIT", Immediate, BIT);
    decodingTable[0x8A] = Instruction("TXA", Implied, TXA);
    decodingTable[0x8C] = Instruction("STY", AbsoluteWrite, STY); decodingTable[0x8C].setIsIndexRelated(true);
    decodingTable[0x8D] = Instruction("STA", AbsoluteWrite, STA);
    decodingTable[0x8E] = Instruction("STX", AbsoluteWrite, STX); decodingTable[0x8E].setIsIndexRelated(true);
    decodingTable[0x8F] = Instruction("STA", AbsoluteLongWrite, STA);
    decodingTable[0x98] = Instruction("TYA", Implied, TYA);
    decodingTable[0x99] = Instruction("STA", AbsoluteYWrite, STA);
    decodingTable[0x9A] = Instruction("TXS", Implied, TXS);
    decodingTable[0x9B] = Instruction("TXY", Implied, TXY);
    decodingTable[0x9C] = Instruction("STZ", AbsoluteWrite, STZ);
    decodingTable[0x9D] = Instruction("STA", AbsoluteXWrite, STA);
    decodingTable[0x9E] = Instruction("STZ", AbsoluteXWrite, STZ);
    decodingTable[0x9F] = Instruction("STA", AbsoluteXLongWrite, STA);
    decodingTable[0xA0] = Instruction("LDY", Immediate, LDY); decodingTable[0xA0].setIsIndexRelated(true);
    decodingTable[0xA2] = Instruction("LDX", Immediate, LDX); decodingTable[0xA2].setIsIndexRelated(true);
    decodingTable[0xA9] = Instruction("LDA", Immediate, LDA);
    decodingTable[0xAA] = Instruction("TAX", Implied, TAX);
    decodingTable[0xAB] = Instruction("TAY", Implied, TAY);
    decodingTable[0xAC] = Instruction("LDY", Absolute, LDY); decodingTable[0xAC].setIsIndexRelated(true);
    decodingTable[0xAD] = Instruction("LDA", Absolute, LDA);
    decodingTable[0xAE] = Instruction("LDX", Absolute, LDX); decodingTable[0xAE].setIsIndexRelated(true);
    decodingTable[0xAF] = Instruction("LDA", AbsoluteLong, LDA);
    decodingTable[0xB8] = Instruction("CLV", Implied, CLV);
    decodingTable[0xB9] = Instruction("LDA", AbsoluteY, LDA);
    decodingTable[0xBA] = Instruction("TSX", Implied, TSX);
    decodingTable[0xBB] = Instruction("TYX", Implied, TYX);
    decodingTable[0xBC] = Instruction("LDY", AbsoluteX, LDY); decodingTable[0xBC].setIsIndexRelated(true);
    decodingTable[0xBD] = Instruction("LDA", AbsoluteX, LDA);
    decodingTable[0xBE] = Instruction("LDX", AbsoluteY, LDX); decodingTable[0xBE].setIsIndexRelated(true);
    decodingTable[0xBF] = Instruction("LDA", AbsoluteXLong, LDA);
    decodingTable[0xC0] = Instruction("CPY", Immediate, CPY); decodingTable[0xC0].setIsIndexRelated(true);
    decodingTable[0xC2] = Instruction("REP", ImmediateSpecial, REP);
    decodingTable[0xC8] = Instruction("INY", Implied, INY);
    decodingTable[0xC9] = Instruction("CMP", Immediate, CMP);
    decodingTable[0xCA] = Instruction("DEX", Implied, DEX);
    decodingTable[0xCC] = Instruction("CPY", Absolute, CPY); decodingTable[0xCC].setIsIndexRelated(true);
    decodingTable[0xCD] = Instruction("CMP", Absolute, CMP);
    decodingTable[0xCE] = Instruction("DEC", AbsoluteRMW, DEC);
    decodingTable[0xCF] = Instruction("CMP", AbsoluteLong, CMP);
    decodingTable[0xD8] = Instruction("CLD", Implied, CLD);
    decodingTable[0xD9] = Instruction("CMP", AbsoluteY, CMP);
    decodingTable[0xDC] = Instruction("JML", AbsoluteIndirectJML, dummyStage);
    decodingTable[0xDD] = Instruction("CMP", AbsoluteX, CMP);
    decodingTable[0xDE] = Instruction("DEC", AbsoluteXRMW, DEC);
    decodingTable[0xDF] = Instruction("CMP", AbsoluteXLong, CMP);
    decodingTable[0xE0] = Instruction("CPX", Immediate, CPX); decodingTable[0xE0].setIsIndexRelated(true);
    decodingTable[0xE2] = Instruction("SEP", ImmediateSpecial, SEP);
    decodingTable[0xE8] = Instruction("INX", Implied, INX);
    decodingTable[0xE9] = Instruction("SBC", Immediate, SBC);
    decodingTable[0xEA] = Instruction("NOP", Implied, dummyStage);
    decodingTable[0xEB] = Instruction("XBA", ImpliedSpecial, XBA);
    decodingTable[0xEC] = Instruction("CPX", Absolute, CPX); decodingTable[0xEC].setIsIndexRelated(true);
    decodingTable[0xED] = Instruction("SBC", Absolute, SBC);
    decodingTable[0xEE] = Instruction("INC", AbsoluteRMW, INC);
    decodingTable[0xEF] = Instruction("SBC", AbsoluteLong, SBC);
    decodingTable[0xF8] = Instruction("SED", Implied, SED);
    decodingTable[0xF9] = Instruction("SBC", AbsoluteY, SBC);
    decodingTable[0xFB] = Instruction("XCE", Implied, XCE);
    decodingTable[0xFC] = Instruction("JSR", AbsoluteXIndirectJSR, dummyStage);
    decodingTable[0xFD] = Instruction("SBC", AbsoluteX, SBC);
    decodingTable[0xFE] = Instruction("INC", AbsoluteXRMW, INC);
    decodingTable[0xFF] = Instruction("SBC", AbsoluteXLong, SBC);
}
