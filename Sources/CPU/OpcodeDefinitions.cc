#include "W65816.h"

void W65816::initializeOpcodes()
{
    decodingTable[0x00] = Instruction("BRK", StackInterupt, BRK);
    decodingTable[0x02] = Instruction("COP", StackInterupt, COP);
    decodingTable[0x03] = Instruction("ORA", StackRelative, ORA);
    decodingTable[0x04] = Instruction("TSB", DirectRMW, TSB);
    decodingTable[0x05] = Instruction("ORA", Direct, ORA);
    decodingTable[0x06] = Instruction("ASL", DirectRMW, ASL);
    decodingTable[0x07] = Instruction("ORA", DirectIndirectLong, ORA);
    decodingTable[0x08] = Instruction("PHP", StackPush8, PHP);
    decodingTable[0x09] = Instruction("ORA", Immediate, ORA);
    decodingTable[0x0A] = Instruction("ASL", Accumulator, ASL);
    decodingTable[0x0B] = Instruction("PHD", StackPush16, PHD);
    decodingTable[0x0C] = Instruction("TSB", AbsoluteRMW, TSB);
    decodingTable[0x0D] = Instruction("ORA", Absolute, ORA);
    decodingTable[0x0E] = Instruction("ASL", AbsoluteRMW, ASL);
    decodingTable[0x0F] = Instruction("ORA", AbsoluteLong, ORA);
    decodingTable[0x10] = Instruction("BPL", RelativeBranch, BPL);
    decodingTable[0x11] = Instruction("ORA", DirectIndirectY, ORA);
    decodingTable[0x12] = Instruction("ORA", DirectIndirect, ORA);
    decodingTable[0x13] = Instruction("ORA", StackRelativeIndirectY, ORA);
    decodingTable[0x14] = Instruction("TRB", DirectRMW, TRB);
    decodingTable[0x15] = Instruction("ORA", DirectX, ORA);
    decodingTable[0x16] = Instruction("ASL", DirectXRMW, ASL);
    decodingTable[0x17] = Instruction("ORA", DirectIndirectYLong, ORA);
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
    decodingTable[0x23] = Instruction("AND", StackRelative, AND);
    decodingTable[0x24] = Instruction("BIT", Direct, BIT);
    decodingTable[0x25] = Instruction("AND", Direct, AND);
    decodingTable[0x26] = Instruction("ROL", DirectRMW, ROL);
    decodingTable[0x27] = Instruction("AND", DirectIndirectLong, AND);
    decodingTable[0x28] = Instruction("PLP", StackPop8, PLP);
    decodingTable[0x29] = Instruction("AND", Immediate, AND);
    decodingTable[0x2A] = Instruction("ROL A", Accumulator, ROL);
    decodingTable[0x2B] = Instruction("PLD", StackPop16, PLD);
    decodingTable[0x2C] = Instruction("BIT", Absolute, BIT);
    decodingTable[0x2D] = Instruction("AND", Absolute, AND);
    decodingTable[0x2E] = Instruction("ROL", AbsoluteRMW, ROL);
    decodingTable[0x2F] = Instruction("AND", AbsoluteLong, AND);
    decodingTable[0x30] = Instruction("BMI", RelativeBranch, BMI);
    decodingTable[0x31] = Instruction("AND", DirectIndirectY, AND);
    decodingTable[0x32] = Instruction("AND", DirectIndirect, AND);
    decodingTable[0x33] = Instruction("AND", StackRelativeIndirectY, AND);
    decodingTable[0x34] = Instruction("BIT", DirectX, BIT);
    decodingTable[0x35] = Instruction("AND", DirectX, AND);
    decodingTable[0x36] = Instruction("ROL", DirectX, ROL);
    decodingTable[0x37] = Instruction("AND", DirectIndirectYLong, AND);
    decodingTable[0x38] = Instruction("SEC", Implied, SEC);
    decodingTable[0x39] = Instruction("AND", AbsoluteY, AND);
    decodingTable[0x3A] = Instruction("DEC", Accumulator, DEC);
    decodingTable[0x3B] = Instruction("TSC", Implied, TSC);
    decodingTable[0x3C] = Instruction("BIT", AbsoluteX, BIT);
    decodingTable[0x3D] = Instruction("AND", AbsoluteX, AND);
    decodingTable[0x3E] = Instruction("ROL", AbsoluteXRMW, ROL);
    decodingTable[0x3F] = Instruction("AND", AbsoluteXLong, AND);
    decodingTable[0x40] = Instruction("RTI", StackRTI, dummyStage);
    decodingTable[0x42] = Instruction("WDM", Immediate, dummyStage);
    decodingTable[0x43] = Instruction("EOR", StackRelative, EOR);
    decodingTable[0x44] = Instruction("MVP", BlockMoveP, dummyStage);
    decodingTable[0x45] = Instruction("EOR", Direct, EOR);
    decodingTable[0x46] = Instruction("LSR", DirectRMW, LSR);
    decodingTable[0x47] = Instruction("EOR", DirectIndirectLong, EOR);
    decodingTable[0x48] = Instruction("PHA", StackPush, PHA);
    decodingTable[0x49] = Instruction("EOR", Immediate, EOR);
    decodingTable[0x4A] = Instruction("LSR", Accumulator, LSR);
    decodingTable[0x4B] = Instruction("PHK", StackPush8, PHK);
    decodingTable[0x4C] = Instruction("JMP", AbsoluteJMP, dummyStage);
    decodingTable[0x4D] = Instruction("EOR", Absolute, EOR);
    decodingTable[0x4E] = Instruction("LSR", AbsoluteRMW, LSR);
    decodingTable[0x4F] = Instruction("EOR", AbsoluteLong, EOR);
    decodingTable[0x50] = Instruction("BVC", RelativeBranch, BVC);
    decodingTable[0x51] = Instruction("EOR", DirectIndirectY, EOR);
    decodingTable[0x52] = Instruction("EOR", DirectIndirect, EOR);
    decodingTable[0x53] = Instruction("EOR", StackRelativeIndirectY, EOR);
    decodingTable[0x54] = Instruction("MVN", BlockMoveN, dummyStage);
    decodingTable[0x55] = Instruction("EOR", DirectX, EOR);
    decodingTable[0x56] = Instruction("LSR", DirectXRMW, LSR);
    decodingTable[0x57] = Instruction("EOR", DirectIndirectYLong, EOR);
    decodingTable[0x58] = Instruction("CLI", Implied, CLI);
    decodingTable[0x59] = Instruction("EOR", AbsoluteY, EOR);
    decodingTable[0x5A] = Instruction("PHY", StackPush, PHY); decodingTable[0x5A].setIsIndexRelated(true);
    decodingTable[0x5B] = Instruction("TCD", Implied, TCD);
    decodingTable[0x5C] = Instruction("JMP", AbsoluteLongJMP, dummyStage);
    decodingTable[0x5D] = Instruction("EOR", AbsoluteX, EOR);
    decodingTable[0x5E] = Instruction("LSR", AbsoluteXRMW, LSR);
    decodingTable[0x5F] = Instruction("EOR", AbsoluteXLong, EOR);
    decodingTable[0x60] = Instruction("RTS", StackRTS, dummyStage);
    decodingTable[0x62] = Instruction("PER", StackPER, dummyStage);
    decodingTable[0x63] = Instruction("ADC", StackRelative, ADC);
    decodingTable[0x64] = Instruction("STZ", DirectWrite, STZ);
    decodingTable[0x65] = Instruction("ADC", Direct, ADC);
    decodingTable[0x66] = Instruction("ROR", DirectRMW, ROR);
    decodingTable[0x67] = Instruction("ADC", DirectIndirectLong, ADC);
    decodingTable[0x68] = Instruction("PLA", StackPop, PLA);
    decodingTable[0x69] = Instruction("ADC", Immediate, ADC);
    decodingTable[0x6A] = Instruction("ROR", Accumulator, ROR);
    decodingTable[0x6B] = Instruction("RTL", StackRTL, dummyStage);
    decodingTable[0x6C] = Instruction("JMP", AbsoluteIndirectJMP, dummyStage);
    decodingTable[0x6D] = Instruction("ADC", Absolute, ADC);
    decodingTable[0x6E] = Instruction("ROR", AbsoluteRMW, ROR);
    decodingTable[0x6F] = Instruction("ADC", AbsoluteLong, ADC);
    decodingTable[0x70] = Instruction("BVS", RelativeBranch, BVS);
    decodingTable[0x71] = Instruction("ADC", DirectIndirectY, ADC);
    decodingTable[0x72] = Instruction("ADC", DirectIndirect, ADC);
    decodingTable[0x73] = Instruction("ADC", StackRelativeIndirectY, ADC);
    decodingTable[0x74] = Instruction("STZ", DirectXWrite, STZ);
    decodingTable[0x75] = Instruction("ADC", DirectX, ADC);
    decodingTable[0x76] = Instruction("ROR", DirectXRMW, ROR);
    decodingTable[0x77] = Instruction("ADC", DirectIndirectYLong, ADC);
    decodingTable[0x78] = Instruction("SEI", Implied, SEI);
    decodingTable[0x79] = Instruction("ADC", AbsoluteY, ADC);
    decodingTable[0x7A] = Instruction("PLY", StackPop, PLY); decodingTable[0x7A].setIsIndexRelated(true);
    decodingTable[0x7B] = Instruction("TDC", Implied, TDC);
    decodingTable[0x7C] = Instruction("JMP", AbsoluteXIndirectJMP, dummyStage);
    decodingTable[0x7D] = Instruction("ADC", AbsoluteX, ADC);
    decodingTable[0x7E] = Instruction("ROR", AbsoluteXRMW, ROR);
    decodingTable[0x7F] = Instruction("ADC", AbsoluteXLong, ADC);
    decodingTable[0x80] = Instruction("BRA", RelativeBranch, BRA);
    decodingTable[0x81] = Instruction("STA", DirectXIndirectWrite, STA);
    decodingTable[0x82] = Instruction("BRL", RelativeBranchLong, dummyStage);
    decodingTable[0x83] = Instruction("STA", StackRelativeWrite, STA);
    decodingTable[0x84] = Instruction("STY", DirectWrite, STY); decodingTable[0x84].setIsIndexRelated(true);
    decodingTable[0x85] = Instruction("STA", DirectWrite, STA);
    decodingTable[0x86] = Instruction("STX", DirectWrite, STX); decodingTable[0x86].setIsIndexRelated(true);
    decodingTable[0x87] = Instruction("STA", DirectIndirectLongWrite, STA);
    decodingTable[0x88] = Instruction("DEY", Implied, DEY);
    decodingTable[0x89] = Instruction("BIT", Immediate, BIT);
    decodingTable[0x8A] = Instruction("TXA", Implied, TXA);
    decodingTable[0x8B] = Instruction("PHB", StackPush8, PHB);
    decodingTable[0x8C] = Instruction("STY", AbsoluteWrite, STY); decodingTable[0x8C].setIsIndexRelated(true);
    decodingTable[0x8D] = Instruction("STA", AbsoluteWrite, STA);
    decodingTable[0x8E] = Instruction("STX", AbsoluteWrite, STX); decodingTable[0x8E].setIsIndexRelated(true);
    decodingTable[0x8F] = Instruction("STA", AbsoluteLongWrite, STA);
    decodingTable[0x90] = Instruction("BCC", RelativeBranch, BCC);
    decodingTable[0x91] = Instruction("STA", DirectIndirectYWrite, STA);
    decodingTable[0x92] = Instruction("STA", DirectIndirectWrite, STA);
    decodingTable[0x93] = Instruction("STA", StackRelativeIndirectYWrite, STA);
    decodingTable[0x94] = Instruction("STY", DirectXWrite, STY); decodingTable[0x94].setIsIndexRelated(true);
    decodingTable[0x95] = Instruction("STA", DirectXWrite, STA);
    decodingTable[0x96] = Instruction("STX", DirectYWrite, STX); decodingTable[0x96].setIsIndexRelated(true);
    decodingTable[0x97] = Instruction("STA", DirectIndirectYLongWrite, STA);
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
    decodingTable[0xA3] = Instruction("LDA", StackRelative, LDA);
    decodingTable[0xA4] = Instruction("LDY", Direct, LDY); decodingTable[0xA4].setIsIndexRelated(true);
    decodingTable[0xA5] = Instruction("LDA", Direct, LDA);
    decodingTable[0xA6] = Instruction("LDX", Direct, LDX); decodingTable[0xA6].setIsIndexRelated(true);
    decodingTable[0xA7] = Instruction("LDA", DirectIndirectLong, LDA);
    decodingTable[0xA8] = Instruction("TAY", Implied, TAY);
    decodingTable[0xA9] = Instruction("LDA", Immediate, LDA);
    decodingTable[0xAA] = Instruction("TAX", Implied, TAX);
    decodingTable[0xAB] = Instruction("PLB", StackPop8, PLB);
    decodingTable[0xAC] = Instruction("LDY", Absolute, LDY); decodingTable[0xAC].setIsIndexRelated(true);
    decodingTable[0xAD] = Instruction("LDA", Absolute, LDA);
    decodingTable[0xAE] = Instruction("LDX", Absolute, LDX); decodingTable[0xAE].setIsIndexRelated(true);
    decodingTable[0xAF] = Instruction("LDA", AbsoluteLong, LDA);
    decodingTable[0xB0] = Instruction("BCS", RelativeBranch, BCS);
    decodingTable[0xB1] = Instruction("LDA", DirectIndirectY, LDA);
    decodingTable[0xB2] = Instruction("LDA", DirectIndirect, LDA);
    decodingTable[0xB3] = Instruction("LDA", StackRelativeIndirectY, LDA);
    decodingTable[0xB4] = Instruction("LDY", DirectX, LDY); decodingTable[0xB4].setIsIndexRelated(true);
    decodingTable[0xB5] = Instruction("LDA", DirectX, LDA);
    decodingTable[0xB6] = Instruction("LDX", DirectY, LDX); decodingTable[0xB6].setIsIndexRelated(true);
    decodingTable[0xB7] = Instruction("LDA", DirectIndirectYLong, LDA);
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
    decodingTable[0xC3] = Instruction("CMP", StackRelative, CMP);
    decodingTable[0xC4] = Instruction("CPY", Direct, CPY); decodingTable[0xC4].setIsIndexRelated(true);
    decodingTable[0xC5] = Instruction("CMP", Direct, CMP);
    decodingTable[0xC6] = Instruction("DEC", DirectRMW, DEC);
    decodingTable[0xC7] = Instruction("CMP", DirectIndirectLong, CMP);
    decodingTable[0xC8] = Instruction("INY", Implied, INY);
    decodingTable[0xC9] = Instruction("CMP", Immediate, CMP);
    decodingTable[0xCA] = Instruction("DEX", Implied, DEX);
    decodingTable[0xCB] = Instruction("WAI", ImpliedSpecial, WAI);
    decodingTable[0xCC] = Instruction("CPY", Absolute, CPY); decodingTable[0xCC].setIsIndexRelated(true);
    decodingTable[0xCD] = Instruction("CMP", Absolute, CMP);
    decodingTable[0xCE] = Instruction("DEC", AbsoluteRMW, DEC);
    decodingTable[0xCF] = Instruction("CMP", AbsoluteLong, CMP);
    decodingTable[0xD0] = Instruction("BNE", RelativeBranch, BNE);
    decodingTable[0xD1] = Instruction("CMP", DirectIndirectY, CMP);
    decodingTable[0xD2] = Instruction("CMP", DirectIndirect, CMP);
    decodingTable[0xD3] = Instruction("CMP", StackRelativeIndirectY, CMP);
    decodingTable[0xD4] = Instruction("PEI", StackPEI, dummyStage);
    decodingTable[0xD5] = Instruction("CMP", DirectX, CMP);
    decodingTable[0xD6] = Instruction("DEC", DirectXRMW, DEC);
    decodingTable[0xD7] = Instruction("CMP", DirectIndirectYLong, CMP);
    decodingTable[0xD8] = Instruction("CLD", Implied, CLD);
    decodingTable[0xD9] = Instruction("CMP", AbsoluteY, CMP);
    decodingTable[0xDA] = Instruction("PHX", StackPush, PHX); decodingTable[0xDA].setIsIndexRelated(true);
    decodingTable[0xDB] = Instruction("STP", ImpliedSpecial, STP);
    decodingTable[0xDC] = Instruction("JML", AbsoluteIndirectJML, dummyStage);
    decodingTable[0xDD] = Instruction("CMP", AbsoluteX, CMP);
    decodingTable[0xDE] = Instruction("DEC", AbsoluteXRMW, DEC);
    decodingTable[0xDF] = Instruction("CMP", AbsoluteXLong, CMP);
    decodingTable[0xE0] = Instruction("CPX", Immediate, CPX); decodingTable[0xE0].setIsIndexRelated(true);
    decodingTable[0xE2] = Instruction("SEP", ImmediateSpecial, SEP);
    decodingTable[0xE3] = Instruction("SBC", StackRelative, SBC);
    decodingTable[0xE4] = Instruction("CPX", Direct, CPX); decodingTable[0xE4].setIsIndexRelated(true);
    decodingTable[0xE5] = Instruction("SBC", Direct, SBC);
    decodingTable[0xE6] = Instruction("INC", DirectRMW, INC);
    decodingTable[0xE7] = Instruction("SBC", DirectIndirectLong, SBC);
    decodingTable[0xE8] = Instruction("INX", Implied, INX);
    decodingTable[0xE9] = Instruction("SBC", Immediate, SBC);
    decodingTable[0xEA] = Instruction("NOP", Implied, dummyStage);
    decodingTable[0xEB] = Instruction("XBA", ImpliedSpecial, XBA);
    decodingTable[0xEC] = Instruction("CPX", Absolute, CPX); decodingTable[0xEC].setIsIndexRelated(true);
    decodingTable[0xED] = Instruction("SBC", Absolute, SBC);
    decodingTable[0xEE] = Instruction("INC", AbsoluteRMW, INC);
    decodingTable[0xEF] = Instruction("SBC", AbsoluteLong, SBC);
    decodingTable[0xF0] = Instruction("BEQ", RelativeBranch, BEQ);
    decodingTable[0xF1] = Instruction("SBC", DirectIndirectY, SBC);
    decodingTable[0xF2] = Instruction("SBC", DirectIndirect, SBC);
    decodingTable[0xF3] = Instruction("SBC", StackRelativeIndirectY, SBC);
    decodingTable[0xF4] = Instruction("PEA", StackPEA, dummyStage);
    decodingTable[0xF5] = Instruction("SBC", DirectX, SBC);
    decodingTable[0xF6] = Instruction("INC", DirectXRMW, INC);
    decodingTable[0xF7] = Instruction("SBC", DirectIndirectYLong, SBC);
    decodingTable[0xF8] = Instruction("SED", Implied, SED);
    decodingTable[0xF9] = Instruction("SBC", AbsoluteY, SBC);
    decodingTable[0xFA] = Instruction("PLX", StackPop, PLX); decodingTable[0xFA].setIsIndexRelated(true);
    decodingTable[0xFB] = Instruction("XCE", Implied, XCE);
    decodingTable[0xFC] = Instruction("JSR", AbsoluteXIndirectJSR, dummyStage);
    decodingTable[0xFD] = Instruction("SBC", AbsoluteX, SBC);
    decodingTable[0xFE] = Instruction("INC", AbsoluteXRMW, INC);
    decodingTable[0xFF] = Instruction("SBC", AbsoluteXLong, SBC);
}
