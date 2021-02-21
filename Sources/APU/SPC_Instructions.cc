#include "SPC700.h"

void SPC700::MOVA()
{
    asm_inst = "MOV A";
    a = idb8;
}

void SPC700::MOVX()
{
    asm_inst = "MOV X";
    x = idb8;
}

void SPC700::MOVY()
{
    asm_inst = "MOV Y";
    y = idb8;
}

void SPC700::MOVAX()
{
    inst_cycles += 2;
    asm_inst = "MOV A,X";
    a = x;
    updateNZflags(a);
}

void SPC700::MOVXA()
{
    inst_cycles += 2;
    asm_inst = "MOV X,A";
    x = a;
    updateNZflags(x);
}

void SPC700::MOVAY()
{
    inst_cycles += 2;
    asm_inst = "MOV A,Y";
    a = y;
    updateNZflags(a);
}

void SPC700::MOVYA()
{
    inst_cycles += 2;
    asm_inst = "MOV Y,A";
    y = a;
    updateNZflags(y);
}

void SPC700::MOVXSP()
{
    inst_cycles += 2;
    asm_inst = "MOV X,SP";
    x = sp;
    updateNZflags(x);
}

void SPC700::MOVSPX()
{
    inst_cycles += 2;
    asm_inst = "MOV SP,X";
    sp = x;
}

void SPC700::MOVW_YA()
{
    asm_inst = "MOVW YA";
    y = idb8_ext;
    a = idb8;
}
