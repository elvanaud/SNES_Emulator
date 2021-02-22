#include "SPC700.h"

void SPC700::MOVA()
{
    asm_inst = "MOV A"; //TODO: asm change the order of operations depending on memDir
    doMemOperation(memoryDirection,&a,&idb8);
}

void SPC700::MOVX()
{
    asm_inst = "MOV X";
    doMemOperation(memoryDirection,&x,&idb8);
}

void SPC700::MOVY()
{
    asm_inst = "MOV Y";
    doMemOperation(memoryDirection,&y,&idb8);
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
    doMemOperation(memoryDirection,&y,&idb8_ext);
    doMemOperation(memoryDirection,&a,&idb8);
}

void SPC700::MOV_PSW()
{
    asm_inst = "MOV PSW"; //TODO: asm : really separate the MOV part and the reg part
    doMemOperation(memoryDirection,&psw.val,&idb8);
}

void SPC700::OR()
{
    asm_inst = "OR A"; //TODO: same thing

    a|= idb8;
    updateNZflags(a);
}

void SPC700::AND()
{
    asm_inst = "AND A";

    a&= idb8;
    updateNZflags(a);
}

void SPC700::EOR()
{
    asm_inst = "EOR A";

    a^= idb8;
    updateNZflags(a);
}

void SPC700::CMP()
{
    asm_inst = "CMP A";

    uint8_t res = a - idb8;
    updateNZflags(res);
    psw.setC(a >= idb8);
    cmpNoWrite = true;
}

void SPC700::ADC()
{
    asm_inst = "ADC A";

    /*uint8_t aright = a&0x0F;
    uint8_t bright = idb8&0x0F;
    uint8_t aleft = a&0xF0;
    uint8_t bleft = idb8&0xF0;

    uint16_t res = aright+bright+psw.C();
    psw.setH(res>>4);
    res+= aleft;
    res+=bright
    psw.setC(res>>8);

    updateNZflags(res);
    a = res&0xFF;*/

    /////
    uint8_t res = a+idb8+psw.C();
    psw.setC(CarryFromBit(a,idb8,res,7));
    psw.setH(CarryFromBit(a,idb8,res,3));
    checkSignedOverflow((a>>7)&1,(idb8>>7)&1,(res>>7)&1); //TODO:check carry flags for adc and sbc
    updateNZflags(res);
}

void SPC700::SBC()
{
    asm_inst = "SBC A";

    idb8 = ~idb8;//Complement to 1
    //ADC();
    uint8_t res = a+idb8+psw.C();
    psw.setC(BorrowFromBit(a,idb8,res,7));
    psw.setH(BorrowFromBit(a,idb8,res,3));
    checkSignedOverflow((a>>7)&1,(idb8>>7)&1,(res>>7)&1);
    updateNZflags(res);
}

void SPC700::CMPX()
{
    asm_inst = "CMP X";

    uint8_t res = x - idb8;
    updateNZflags(res);
    psw.setC(x >= idb8);
}

void SPC700::CMPY()
{
    asm_inst = "CMP Y";

    uint8_t res = y - idb8;
    updateNZflags(res);
    psw.setC(y >= idb8);
}

void SPC700::ASL()
{
    asm_inst = "ASL";

    //unary instructions will operate on idb8 (instead of the accumulator)
    psw.setC(idb8>>7);
    idb8 <<= 1;
    updateNZflags(idb8);
}

void SPC700::ROL()
{
    asm_inst = "ROL";

    uint8_t tmpC = (idb8>>7);
    idb8 <<= 1;
    idb8 |= psw.C();
    psw.setC(tmpC);
    updateNZflags(idb8);
}

void SPC700::LSR()
{
    asm_inst = "LSR";

    psw.setC(idb8&1);
    idb8 >>= 1;
    updateNZflags(idb8);
}

void SPC700::ROR()
{
    asm_inst = "ROR";

    uint8_t tmpC = (idb8&1);
    idb8 >>= 1;
    idb8 |= (psw.C()<<7);
    psw.setC(tmpC);
    updateNZflags(idb8);
}

void SPC700::DEC()
{
    asm_inst = "DEC";

    --idb8;
    updateNZflags(idb8);
}

void SPC700::INC()
{
    asm_inst = "INC";

    ++idb8;
    updateNZflags(idb8);
}

void SPC700::BPL()
{
    asm_inst = "BPL";

    branchTaken = (psw.N() == 0);
}

void SPC700::BMI()
{
    asm_inst = "BMI";

    branchTaken = (psw.N() == 1);
}

void SPC700::BVC()
{
    asm_inst = "BVC";

    branchTaken = (psw.V() == 0);
}

void SPC700::BVS()
{
    asm_inst = "BVS";

    branchTaken = (psw.V() == 1);
}

void SPC700::BCC()
{
    asm_inst = "BCC";

    branchTaken = (psw.C() == 0);
}

void SPC700::BCS()
{
    asm_inst = "BCS";

    branchTaken = (psw.C() == 1);
}

void SPC700::BNE()
{
    asm_inst = "BNE";

    branchTaken = (psw.Z() == 0);
}

void SPC700::BEQ()
{
    asm_inst = "BEQ";

    branchTaken = (psw.Z() == 1);
}

void SPC700::BBS()
{
    asm_inst = "BBS";

    //Multiple opcodes target this instruction so we have to decode further here:
    uint8_t bitId = read(pc)>>4;
    bitId >>= 2; //Divide by 2

    branchTaken = (((idb8>>bitId)&1) == 1);
}

void SPC700::BBC()
{
    asm_inst = "BBC";

    //Multiple opcodes target this instruction so we have to decode further here:
    uint8_t bitId = read(pc)>>4;
    --bitId;
    bitId >>= 2; //Divide by 2

    branchTaken = (((idb8>>bitId)&1) == 0);
}

void SPC700::CBNE()
{
    asm_inst = "CBNE";

    branchTaken = (a!=idb8);
}

void SPC700::DBNZ()
{
    asm_inst = "DBNZ";
    inst_cycles += 2;

    --y;
    branchTaken = (y!=0);
}

void SPC700::DBNZ_MEM()
{
    asm_inst = "DBNZ";

    --idb8;
    write(directAddress(read(pc+1)),idb8);

    branchTaken = (idb8!=0);
}

void SPC700::BRA()
{
    asm_inst = "BRA";

    branchTaken = true;
}

void SPC700::JMP()
{
    asm_inst = "JMP";

    pc = make16(idb8_ext,idb8);
    pc-=inst_length;
}

void SPC700::CALL()
{
    asm_inst = "CALL";
    inst_cycles = 8;

    pc+=inst_length; //Compute the return address

    uint8_t lowPC = pc;
    uint8_t highPC = (pc>>8);

    push(highPC);
    push(lowPC);

    pc = make16(idb8_ext,idb8);
    pc-=inst_length;
}

void SPC700::TCALL()
{
    asm_inst = "TCALL";
    inst_cycles = 8;

    uint8_t n = (read(pc)>>4);
    pc+=inst_length; //Compute the return address

    uint8_t lowPC = pc;
    uint8_t highPC = (pc>>8);

    push(highPC);
    push(lowPC);

    uint16_t adr = 0xFFDE - 2*n;
    pc = make16(read(adr+1), read(adr));
    pc-=inst_length;
}

void SPC700::PCALL()
{
    asm_inst = "PCALL";
    inst_cycles = 6;
    inst_length = 2;

    uint8_t adr = read(pc+1); //This instruction handles its own operand (see comment in tick:decode function)
    pc+=inst_length; //Compute the return address

    uint8_t lowPC = pc;
    uint8_t highPC = (pc>>8);

    push(highPC);
    push(lowPC);

    pc = make16(0xFF,adr);
    pc-=inst_length;
}

void SPC700::RET()
{
    asm_inst = "RET";
    inst_cycles = 5;

    uint8_t lowPC = pop();
    uint8_t highPC = pop();

    pc = make16(highPC, lowPC);
    pc-=inst_length;
}

void SPC700::RETI()
{
    asm_inst = "RETI";
    inst_cycles = 6;

    psw.val = pop();

    uint8_t lowPC = pop();
    uint8_t highPC = pop();

    pc = make16(highPC, lowPC);
    pc-=inst_length;
}

void SPC700::BRK()
{
    asm_inst = "BRK";
    inst_cycles = 8;

    pc+=inst_length; //Compute the return address

    uint8_t lowPC = pc;
    uint8_t highPC = (pc>>8);

    push(highPC);
    push(lowPC);
    push(psw.val);

    psw.setB(true);
    psw.setI(false);

    pc = make16(read(0xFFDF),read(0xFFDE));
    pc-=inst_length;
}

void SPC700::NOP()
{
    asm_inst = "NOP";
    inst_cycles = 2;

    ;//That's it
}

void SPC700::CLRP()
{
    asm_inst = "CLRP";
    inst_cycles = 2;
    psw.setP(false);
}

void SPC700::SETP()
{
    asm_inst = "SETP";
    inst_cycles = 2;
    psw.setP(true);
}

void SPC700::EI()
{
    asm_inst = "EI";
    inst_cycles = 3;
    psw.setI(true);
}

void SPC700::DI()
{
    asm_inst = "DI";
    inst_cycles = 3;
    psw.setI(false);
}

void SPC700::HALT()
{
    asm_inst = "HALT";
    inst_cycles = 2;

    halt_cpu = true;
}

void SPC700::SLEEP()
{
    asm_inst = "SLEEP";
    inst_cycles = 2;

    halt_cpu = true;
}

void SPC700::CLR1()
{
    asm_inst = "CLR1";

    uint8_t n = (read(pc)>>4);
    --n;
    n /= 2;

    idb8 &= ~(1<<n);
}

void SPC700::SET1()
{
    asm_inst = "SET1";

    uint8_t n = (read(pc)>>4);
    n /= 2;

    idb8 |= (1<<n);
}

void SPC700::NOT1()
{
    asm_inst = "NOT1";

    idb8 = ((~idb8)&1);
}

void SPC700::MOV1()
{
    asm_inst = "MOV1";


    if(memoryDirection == ReadMemory)
    {
        psw.setC(idb8);
    }
    else
    {
        inst_cycles+=1;
        idb8 = psw.C();
    }
}

void SPC700::OR1()
{
    asm_inst = "OR1 C";
    inst_cycles+=1;

    psw.setC(psw.C()|idb8);
}

void SPC700::OR1_NOT()
{
    asm_inst = "OR1 C,not";
    inst_cycles+=1;

    psw.setC(psw.C()|((~idb8)&1));
}

void SPC700::AND1()
{
    asm_inst = "AND1 C";

    psw.setC(psw.C()&idb8);
}

void SPC700::AND1_NOT()
{
    asm_inst = "AND1 C,not";

    psw.setC(psw.C()&((~idb8)&1));
}

void SPC700::EOR1()
{
    asm_inst = "EOR1 C";

    psw.setC(psw.C()^idb8);
}

void SPC700::CLRC()
{
    asm_inst = "CLRC";
    inst_cycles = 2;

    psw.setC(false);
}

void SPC700::SETC()
{
    asm_inst = "SETC";
    inst_cycles = 2;

    psw.setC(true);
}

void SPC700::NOTC()
{
    asm_inst = "NOTC";
    inst_cycles = 3;

    psw.setC((~psw.C())&1);
}

void SPC700::CLRVH()
{
    asm_inst = "CLR V,H";
    inst_cycles = 2;

    psw.setV(false);
    psw.setH(false);
}

void SPC700::XCN()
{
    asm_inst = "XCN";
    inst_cycles = 5;

    uint8_t accLeft = (a>>4);
    a <<= 4;
    a |= accLeft;

    updateNZflags(a);
}

void SPC700::TCLR1()
{
    asm_inst = "TCLR1";
    inst_cycles = 6;

    uint8_t tmpRes = a-idb8;
    updateNZflags(tmpRes);

    idb8 &= (~a);
}

void SPC700::TSET1()
{
    asm_inst = "TSET1";
    inst_cycles = 6;

    uint8_t tmpRes = a-idb8;
    updateNZflags(tmpRes);

    idb8 |= a;
}

void SPC700::DAA()
{
    asm_inst = "DAA";
    inst_cycles = 3;

    uint8_t offset = 0;
    uint8_t right = a&0x0F;
    uint8_t left = (a>>4);

    if(psw.H()==1 || (right>=0xA && right<=0xF))
    {
        offset += 0x06;
    }
    if(psw.C()==1 || (left>=0xA && left <= 0xF) || ((right>=0xA && right<=0xF)&&left==9))
    {
        offset += 0x60;
        psw.setC(true);
    }

    a+=offset;
    updateNZflags(a);
}

void SPC700::DAS() //TODO: test those two
{
    asm_inst = "DAS";
    inst_cycles = 3;

    uint8_t offset = 0;
    //uint8_t right = a&0x0F;
    //uint8_t left = (a>>4);

    if(psw.H()==1)
    {
        offset += 0x0A;
    }
    uint8_t left_offset[] = {0x00,0xF0,0xA0,0x90};
    offset += left_offset[(psw.C()<<1)|psw.H()];
    if(psw.C()==1)
    {
        psw.setC(true); //useless ? Why did I write this ??
    }

    a+=offset;
    updateNZflags(a);
}
