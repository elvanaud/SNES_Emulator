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
    idb8 = read(directAddress(offset));

    updateNZflags(idb8);
    inst(this);
}

void SPC700::DirectIndexedX(Inst inst)
{
    inst_cycles = 4;
    inst_length = 2;

    uint8_t offset = read(pc+1)+x;
    idb8 = read(directAddress(offset));

    updateNZflags(idb8);
    inst(this);
}

void SPC700::Absolute(Inst inst)
{
    inst_cycles = 4;
    inst_length = 3;

    uint16_t adr = make16(read(pc+2),read(pc+1));

    idb8 = read(adr);
    updateNZflags(idb8);
    inst(this);
}

void SPC700::AbsoluteIndexedX(Inst inst)
{
    inst_cycles = 5;
    inst_length = 3;

    uint16_t adr = make16(read(pc+2),read(pc+1));

    idb8 = read(adr+x);
    updateNZflags(idb8);
    inst(this);
}

void SPC700::AbsoluteIndexedY(Inst inst)
{
    inst_cycles = 5;
    inst_length = 3;

    uint16_t adr = make16(read(pc+2),read(pc+1));

    idb8 = read(adr+y);
    updateNZflags(idb8);
    inst(this);
}

void SPC700::DirectX(Inst inst)
{
    inst_cycles = 3;
    inst_length = 1;

    idb8 = read(directAddress(x));

    updateNZflags(idb8);
    inst(this);
}

void SPC700::DirectXInc(Inst inst)
{
    inst_cycles = 4;
    inst_length = 1;

    idb8 = read(directAddress(x));
    ++x;

    updateNZflags(idb8);
    inst(this);
}

void SPC700::DirectIndirectIndexed(Inst inst)
{
    inst_cycles = 6;
    inst_length = 2;

    uint8_t offset = read(pc+1);
    uint16_t adr = read(directAddress(offset));
    ++offset;
    adr |= (uint16_t(read(directAddress(offset)))<<8);
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
    uint16_t adr = read(directAddress(offset));
    ++offset;
    adr |= (uint16_t(read(directAddress(offset)))<<8);

    idb8 = read(adr);

    updateNZflags(idb8);
    inst(this);
}

void SPC700::DirectIndexedY(Inst inst)
{
    inst_cycles = 4;
    inst_length = 2;

    uint8_t offset = read(pc+1)+y;
    idb8 = read(directAddress(offset));

    updateNZflags(idb8);
    inst(this);
}

void SPC700::Direct16(Inst inst)
{
    inst_cycles = 5;
    inst_length = 2;

    uint8_t offset = read(pc+1);
    idb8 = read(directAddress(offset));
    ++offset;
    idb8_ext = read(directAddress(offset));

    updateNZflags(idb8_ext,idb8);
    inst(this);
}

void SPC700::DirectWrite(Inst inst)
{
    inst_cycles = 4;
    inst_length = 2;

    uint8_t offset = read(pc+1);
    uint16_t adr = directAddress(offset);
    read(adr); //Dummy read
    memoryDirection = WriteMemory;
    inst(this); //Puts the value to write in idb8

    write(adr,idb8);
}

void SPC700::DirectWriteImmediate(Inst inst)
{
    inst_cycles = 5;
    inst_length = 3;

    asm_inst = "MOV";

    uint8_t offset = read(pc+2);
    uint16_t adr = directAddress(offset);
    read(adr); //Dummy read

    idb8 = read(pc+1); //Read the immediate value

    //Dummy instruction
    memoryDirection = WriteMemory;
    inst(this); //Not supposed to anything(for now?)

    write(adr,idb8);
    //cout <<"[APU]adr="<<std::hex<<int(adr)<<" val="<<int(idb8)<<endl;
}

void SPC700::DirectTransfer(Inst inst)
{
    inst_cycles = 5;
    inst_length = 3;

    uint8_t sourceOffset = read(pc+1);
    uint16_t sourceAdr = directAddress(sourceOffset);
    //NO Dummy read !

    idb8 = read(sourceAdr); //Read the source data

    //Dummy instruction
    memoryDirection = WriteMemory;
    inst(this); //Not supposed to anything(for now?)

    uint8_t destOffset = read(pc+2);
    uint16_t destAdr = directAddress(destOffset);

    write(destAdr,idb8);
}

void SPC700::DirectIndexedXWrite(Inst inst)
{
    inst_cycles = 5;
    inst_length = 2;

    uint8_t offset = read(pc+1)+x;
    uint16_t adr = directAddress(offset);
    read(adr); //Dummy read

    memoryDirection = WriteMemory;
    inst(this); //Puts the value to write in idb8

    write(adr,idb8);
}

void SPC700::DirectIndexedYWrite(Inst inst)
{
    inst_cycles = 5;
    inst_length = 2;

    uint8_t offset = read(pc+1)+y;
    uint16_t adr = directAddress(offset);
    read(adr); //Dummy read

    memoryDirection = WriteMemory;
    inst(this); //Puts the value to write in idb8

    write(adr,idb8);
}

void SPC700::AbsoluteWrite(Inst inst)
{
    inst_cycles = 5;
    inst_length = 3;

    uint16_t adr = make16(read(pc+2),read(pc+1));
    read(adr); //Dummy read

    memoryDirection = WriteMemory;
    inst(this);

    write(adr,idb8);
}

void SPC700::AbsoluteIndexedXWrite(Inst inst)
{
    inst_cycles = 6;
    inst_length = 3;

    uint16_t adr = make16(read(pc+2),read(pc+1));
    adr+=x;
    read(adr); //Dummy read

    memoryDirection = WriteMemory;
    inst(this);

    write(adr,idb8);
}

void SPC700::AbsoluteIndexedYWrite(Inst inst)
{
    inst_cycles = 6;
    inst_length = 3;

    uint16_t adr = make16(read(pc+2),read(pc+1));
    adr+=y;
    read(adr); //Dummy read

    memoryDirection = WriteMemory;
    inst(this);

    write(adr,idb8);
}

void SPC700::DirectXIncWrite(Inst inst)
{
    inst_cycles = 4;
    inst_length = 1;

    uint16_t adr = directAddress(x);
    ++x;

    memoryDirection = WriteMemory;
    inst(this);

    write(adr,idb8);
}

void SPC700::DirectXWrite(Inst inst)
{
    inst_cycles = 4;
    inst_length = 1;

    uint16_t adr = directAddress(x);
    read(adr); //Dummy read

    memoryDirection = WriteMemory;
    inst(this);

    write(adr,idb8);
}

void SPC700::DirectIndirectIndexedWrite(Inst inst)
{
    inst_cycles = 7;
    inst_length = 2;

    uint8_t offset = read(pc+1);
    uint16_t adr = read(directAddress(offset));
    ++offset;
    adr |= (uint16_t(read(directAddress(offset)))<<8);
    adr += y;
    read(adr);//Dummy read

    memoryDirection = WriteMemory;
    inst(this);

    write(adr,idb8);
}

void SPC700::DirectIndexedIndirectWrite(Inst inst)
{
    inst_cycles = 7;
    inst_length = 2;

    uint8_t offset = read(pc+1)+x;
    uint16_t adr = read(directAddress(offset));
    ++offset;
    adr |= (uint16_t(read(directAddress(offset)))<<8);

    read(adr); //Dummy read

    memoryDirection = WriteMemory;
    inst(this);

    write(adr,idb8);
}

void SPC700::Direct16Write(Inst inst)
{
    inst_cycles = 5;
    inst_length = 2;

    uint8_t offset = read(pc+1);
    read(directAddress(offset)); //Dummy read lsb

    memoryDirection = WriteMemory;
    inst(this); //Puts the lsb in idb8 and msb in idb8_ext

    write(directAddress(offset),idb8);
    ++offset;
    write(directAddress(offset),idb8_ext);
}

void SPC700::Push(Inst inst)
{
    inst_cycles = 4;
    inst_length = 1;

    memoryDirection = WriteMemory;
    inst(this); //Puts the value to push in idb8

    push(idb8);
}

void SPC700::Pop(Inst inst)
{
    inst_cycles = 4;
    inst_length = 1;

    idb8 = pop();

    inst(this); //Puts idb8 in correct reg
}

void SPC700::Direct_Direct(Inst inst)
{
    //a OP b
    inst_cycles = 6;
    inst_length = 3;

    uint8_t bOffset = read(pc+1);
    uint16_t bAdr = directAddress(bOffset);
    uint8_t aOffset = read(pc+2);
    uint16_t aAdr = directAddress(aOffset);

    idb8 = read(bAdr); //Read b operator (right)

    uint8_t save_acc = a; //Left operator a is put in the accumulator
    a = read(aAdr);

    inst(this);

    write(aAdr,a); //We save the result and restore the accumulator
    a = save_acc;
}

void SPC700::Direct_Immediate(Inst inst)
{
    //a OP b
    inst_cycles = 5;
    inst_length = 3;

    uint8_t aOffset = read(pc+2);
    uint16_t aAdr = directAddress(aOffset);

    idb8 = read(pc+1); //Read b operator (right) (immediate)

    uint8_t save_acc = a; //Left operator a is put in the accumulator
    a = read(aAdr);

    inst(this);

    if(!cmpNoWrite)
    {
        cmpNoWrite = false;
        write(aAdr,a); //We save the result and restore the accumulator
    }
    a = save_acc;
}

void SPC700::DirectX_DirectY(Inst inst)
{
    //a OP b
    inst_cycles = 5;
    inst_length = 1;

    uint16_t bAdr = directAddress(y);
    uint16_t aAdr = directAddress(x);

    idb8 = read(bAdr); //Read b operator (right)

    uint8_t save_acc = a; //Left operator a is put in the accumulator
    a = read(aAdr);

    inst(this);

    write(aAdr,a); //We save the result and restore the accumulator
    a = save_acc;
}

void SPC700::DirectRMW(Inst inst)
{
    inst_cycles = 4;
    inst_length = 2;

    uint8_t offset = read(pc+1);
    uint16_t adr = directAddress(offset);
    idb8 = read(adr);

    inst(this);

    write(adr,idb8);
}

void SPC700::DirectIndexedXRMW(Inst inst)
{
    inst_cycles = 5;
    inst_length = 2;

    uint8_t offset = read(pc+1)+x;
    uint16_t adr = directAddress(offset);
    idb8 = read(adr);

    inst(this);

    write(adr,idb8);
}

void SPC700::AbsoluteRMW(Inst inst)
{
    inst_cycles = 5;
    inst_length = 3;

    uint16_t adr = make16(read(pc+2),read(pc+1));
    idb8 = read(adr);

    inst(this);

    write(adr,idb8);
}

void SPC700::AccRMW(Inst inst)
{
    inst_cycles = 2;
    inst_length = 1;

    idb8 = a;
    inst(this);
    a = idb8;
}

void SPC700::XRMW(Inst inst)
{
    inst_cycles = 2;
    inst_length = 1;

    idb8 = x;
    inst(this);
    x = idb8;
}

void SPC700::YRMW(Inst inst)
{
    inst_cycles = 2;
    inst_length = 1;

    idb8 = y;
    inst(this);
    y = idb8;
}

void SPC700::Branch(Inst inst) //TODO: refactor: maybe i should have use immediate instead and have the individual inst set branchTaken or call relativeBranch(true) (helper function) instead
{
    inst_cycles = 2;//Turns 4 if branch is taken
    inst_length = 2;

    inst(this);
    if(branchTaken)
    {
        branchTaken = false;
        inst_cycles+=2;

        uint8_t offset = read(pc+1);
        //cout<<"branch: offset="<<std::hex<<int(offset)<<endl;
        uint8_t pcRight = pc&0xFF;
        pcRight += offset;
        pc = (pc&0xFF00)|pcRight;
    }
}

void SPC700::BranchTestMem(Inst inst)
{
    inst_cycles = 5;//Turns 7 if branch is taken
    inst_length = 3;

    idb8 = read(directAddress(read(pc+1)));
    inst(this);
    if(branchTaken)
    {
        branchTaken = false;
        inst_cycles+= 2;

        uint8_t offset = read(pc+2);
        uint8_t pcRight = pc&0xFF;
        pcRight += offset;
        pc = (pc&0xFF00)|pcRight;
    }
}

void SPC700::BranchTestMemIndexedX(Inst inst)
{
    inst_cycles = 6;//Turns 8 if branch is taken
    inst_length = 3;

    idb8 = read(directAddress(read(pc+1)+x));
    inst(this);
    if(branchTaken)
    {
        branchTaken = false;
        inst_cycles+= 2;

        uint8_t offset = read(pc+2);
        uint8_t pcRight = pc&0xFF;
        pcRight += offset;
        pc = (pc&0xFF00)|pcRight;
    }
}

void SPC700::Immediate16(Inst inst)
{
    inst_cycles = 3;
    inst_length = 3;

    //asm_inst = "JMP"; //Exceptionally this adr mode is used (almost?) exclusively for a single jmp inst

    idb8 = read(pc+1);
    idb8_ext = read(pc+2);

    inst(this);
    //adr |= (uint16_t(read(pc+2))<<8);

    //pc = adr;
}

void SPC700::JumpAbsoluteIndexedX(Inst inst)
{
    inst_cycles = 6;
    inst_length = 3;

    asm_inst = "JMP";

    uint16_t adr = make16(read(pc+2),read(pc+1));
    adr+=x;

    pc = make16(read(adr+1),read(adr));
    pc-=inst_length;//Correct pc autoincrement
}

void SPC700::MemBit(Inst inst)
{
    inst_cycles = 4; //4 or 5 cycles depending on inst
    inst_length = 3;

    uint8_t lowAdr = read(pc+1);
    uint8_t bit = read(pc+2);
    uint8_t highAdr = bit & 0x0F;
    bit >>= 4;

    uint16_t adr = make16(highAdr,lowAdr);
    uint8_t data = read(adr);

    idb8 = ((data>>bit)&1);

    inst(this);
}

void SPC700::MemBitRMW(Inst inst)
{
    inst_cycles = 5; //5 or 6 cycles depending on inst
    inst_length = 3;

    uint8_t lowAdr = read(pc+1);
    uint8_t bit = read(pc+2);
    uint8_t highAdr = bit & 0x0F;
    bit >>= 4;

    uint16_t adr = make16(highAdr,lowAdr);
    uint8_t data = read(adr);

    idb8 = ((data>>bit)&1);

    memoryDirection = WriteMemory;
    inst(this);

    idb8 = ((idb8&1)<<bit);
    data &= ~(1<<bit); //Reset data bit
    data |= idb8;

    write(adr,data);
}

void SPC700::DirectRMW16(Inst inst)
{
    inst_cycles = 6;
    inst_length = 2;

    uint8_t offset = read(pc+1);
    uint16_t adrLow = directAddress(offset);
    ++offset;
    uint16_t adrHigh = directAddress(offset);

    idb8 = read(adrLow);
    idb8_ext = read(adrHigh);

    memoryDirection = WriteMemory;
    inst(this);

    write(adrLow,idb8);
    write(adrHigh,idb8_ext);
}
