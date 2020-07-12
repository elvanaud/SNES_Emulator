#include "W65816.h"

void W65816::initializeOpcodes()
{
    decodingTable[0x09] = Instruction("ORA", Immediate, ORA);
    decodingTable[0x29] = Instruction("AND", Immediate, AND);
    decodingTable[0x38] = Instruction("SEC", Implied, SEC);
    decodingTable[0x49] = Instruction("EOR", Immediate, EOR);
    decodingTable[0x69] = Instruction("ADC", Immediate, ADC);
    decodingTable[0x89] = Instruction("BIT", Immediate, BIT);
    decodingTable[0xA0] = Instruction("LDY", Immediate, LDY); decodingTable[0xA0].setIsIndexRelated(true);
    decodingTable[0xA2] = Instruction("LDX", Immediate, LDX); decodingTable[0xA2].setIsIndexRelated(true);
    decodingTable[0xA9] = Instruction("LDA", Immediate, LDA);
    decodingTable[0xC2] = Instruction("REP", ImmediateSpecial, REP);
    decodingTable[0xC9] = Instruction("CMP", Immediate, CMP);
    decodingTable[0xE0] = Instruction("CPX", Immediate, CPX); decodingTable[0xE0].setIsIndexRelated(true);
    decodingTable[0xE2] = Instruction("SEP", ImmediateSpecial, SEP);
    decodingTable[0xC0] = Instruction("CPY", Immediate, CPY); decodingTable[0xC0].setIsIndexRelated(true);
}
