#include "SPC700.h"

void SPC700::Immediate(Inst inst)
{
    inst_cycles = 2;
    inst_length = 2;

    idb8 = read(pc+1);
    updateNZflags(idb8);
    inst(this);

    asm_final = asm_inst;//TODO: stringstream and stuff
}

void SPC700::Implied(Inst inst)
{
    inst_length = 1;

    inst(this);
}

void SPC700::Direct(Inst inst)
{
    inst_cycles = 3;
    inst_length = 2;

    uint8_t offset = read(pc+1);
    idb8 = read((uint16_t(psw.P())<<8)|offset);

    updateNZflags(idb8);
    inst(this);
}

void SPC700::DirectIndexedX(Inst inst)
{
    inst_cycles = 4;
    inst_length = 2;

    uint8_t offset = read(pc+1)+x;
    idb8 = read((uint16_t(psw.P())<<8)|offset);

    updateNZflags(idb8);
    inst(this);
}

void SPC700::Absolute(Inst inst)
{
    inst_cycles = 4;
    inst_length = 3;

    uint16_t adr = read(pc+1);
    adr |= (uint16_t(read(pc+2))<<8);

    idb8 = read(adr);
    updateNZflags(idb8);
    inst(this);
}

void SPC700::AbsoluteIndexedX(Inst inst)
{
    inst_cycles = 5;
    inst_length = 3;

    uint16_t adr = read(pc+1);
    adr |= (uint16_t(read(pc+2))<<8);

    idb8 = read(adr+x);
    updateNZflags(idb8);
    inst(this);
}

void SPC700::AbsoluteIndexedY(Inst inst)
{
    inst_cycles = 5;
    inst_length = 3;

    uint16_t adr = read(pc+1);
    adr |= (uint16_t(read(pc+2))<<8);

    idb8 = read(adr+y);
    updateNZflags(idb8);
    inst(this);
}

void SPC700::DirectX(Inst inst)
{
    inst_cycles = 3;
    inst_length = 1;

    idb8 = read((uint16_t(psw.P())<<8)|x);

    updateNZflags(idb8);
    inst(this);
}

void SPC700::DirectXInc(Inst inst)
{
    inst_cycles = 4;
    inst_length = 1;

    idb8 = read((uint16_t(psw.P())<<8)|x);
    ++x;

    updateNZflags(idb8);
    inst(this);
}

void SPC700::DirectIndirectIndexed(Inst inst)
{
    inst_cycles = 6;
    inst_length = 2;

    uint8_t offset = read(pc+1);
    uint16_t adr = read((uint16_t(psw.P())<<8)|offset);
    ++offset;
    adr |= (uint16_t(read((uint16_t(psw.P())<<8)|offset))<<8);
    adr += y;
    idb8 = read(adr);

    updateNZflags(idb8);
    inst(this);
}

void SPC700::DirectIndexedIndirect(Inst inst)
{
    inst_cycles = 6;
    inst_length = 2;

    uint8_t offset = read(pc+1)+x;
    uint16_t adr = read((uint16_t(psw.P())<<8)|offset);
    ++offset;
    adr |= (uint16_t(read((uint16_t(psw.P())<<8)|offset))<<8);

    idb8 = read(adr);

    updateNZflags(idb8);
    inst(this);
}

void SPC700::DirectIndexedY(Inst inst)
{
    inst_cycles = 4;
    inst_length = 2;

    uint8_t offset = read(pc+1)+y;
    idb8 = read((uint16_t(psw.P())<<8)|offset);

    updateNZflags(idb8);
    inst(this);
}

void SPC700::Direct16(Inst inst)
{
    inst_cycles = 5;
    inst_length = 2;

    uint8_t offset = read(pc+1);
    idb8 = read((uint16_t(psw.P())<<8)|offset);
    ++offset;
    idb8_ext = read((uint16_t(psw.P())<<8)|offset);

    updateNZflags(idb8_ext,idb8);
    inst(this);
}
