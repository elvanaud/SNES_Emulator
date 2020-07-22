#include "W65816.h"

#include <iostream>
using std::cout;
using std::endl;

void W65816::AND()
{
    uint16_t res = getReg(acc) & getReg(idb);
    updateNZFlags(res);
    setReg(acc,res);
}

void W65816::ADC()
{
    uint32_t r = 0;
    if(p.D())
    {
        auto decimalAdd8bit = [](uint8_t a, uint8_t b)
        {
            uint16_t aLeft = a&0xF0;
            uint16_t aRight = a&0x0F;
            uint16_t bLeft = b&0xF0;
            uint16_t bRight = b&0x0F;

            uint32_t res = aRight+bRight;
            if(res >= 0x0A)
                res += 0x06;
            res += aLeft+bLeft;
            if((res>>4) >= 0x0A)
                res += 0x60;

            return res;
        };

        r = decimalAdd8bit(idb.low,acc.low+p.C());
        if(!p.mem8) r = (decimalAdd8bit(idb.high,acc.high+(r>>8))<<8) | (r&0xFF);

        acc.low = r&0xFF;
        if(!p.mem8) acc.high = (r>>8)&0xFF;
    }
    else
    {
        r = getReg(acc)+getReg(idb)+p.C();

        int offset = 7;
        if(!p.mem8) offset = 15;
        int aSign = (acc.val()>>offset)&1;
        int bSign = ((idb.val()+p.C())>>offset)&1;
        int cSign = (r>>offset)&1;
        checkSignedOverflow(aSign,bSign,cSign);

        setReg(acc,r);
    }

    updateStatusFlags(r);
}

void W65816::BIT()
{
    uint16_t v = getReg(idb);
    if(decodingTable[ir].AdrMode().Name() != Immediate.Name())
    {
        p.setN((v>>7)&1);
        p.setV((v>>6)&1);
    }

    uint16_t r = getReg(acc) & v;
    p.setZ(r==0);
}

void W65816::CLC()
{
    p.setC(false);
}

void W65816::CLD()
{
    p.setD(false);
}

void W65816::CLI()
{
    p.setI(false);
}

void W65816::CLV()
{
    p.setV(false);
}

void W65816::CMP()
{
    uint16_t a = getReg(acc);
    uint16_t b = getReg(idb);
    uint16_t r = a - b;
    updateNZFlags(r);
    p.setC(a >= b);
}

void W65816::CPX()
{
    uint16_t a = getReg(x);
    uint16_t b = getReg(idb);
    uint16_t r = a - b;
    updateNZFlags(r,true);
    p.setC(a >= b);
}

void W65816::CPY()
{
    uint16_t a = getReg(y);
    uint16_t b = getReg(idb);
    uint16_t r = a - b;
    updateNZFlags(r,true);
    p.setC(a >= b);
}

void W65816::DEX()
{
    uint16_t r = getReg(x);
    --r;
    updateNZFlags(r,true);
    setReg(x,r);
}

void W65816::DEY()
{
    uint16_t r = getReg(y);
    --r;
    updateNZFlags(r,true);
    setReg(y,r);
}

void W65816::EOR()
{
    uint16_t r = getReg(acc)^getReg(idb);
    updateNZFlags(r);
    setReg(acc,r);
}

void W65816::INX()
{
    uint16_t r = getReg(x);
    ++r;
    updateNZFlags(r,true);
    setReg(x,r);
}

void W65816::INY()
{
    uint16_t r = getReg(y);
    ++r;
    updateNZFlags(r,true);
    setReg(y,r);
}

void W65816::LDA()
{
    uint16_t value = getReg(idb);
    setReg(acc,value);
    updateNZFlags(value);
}

void W65816::LDX()
{
    uint16_t value = getReg(idb);
    setReg(x,value);
    updateNZFlags(value,true);
}

void W65816::LDY()
{
    uint16_t value = getReg(idb);
    setReg(y,value);
    updateNZFlags(value,true);
}

void W65816::ORA()
{
    uint16_t res = getReg(acc) | getReg(idb);
    setReg(acc,res);
    updateNZFlags(res);
}

void W65816::REP()
{
    uint8_t mask = idb.low;
    if(p.emulationMode) mask &= 0b11'00'1111;

    p.setVal(p.getVal() & ~mask);

    if(p.index8)
    {
        x.high = 0;
        y.high = 0;
    }
}

void W65816::SBC()
{
    if(p.D())
    {
        uint16_t complement = p.mem8 ? 0x99 : 0x9999;
        setReg(idb, complement - getReg(idb));
        //ADC(); //acc+(-idb-1)+p.c
    }
    else
    {
        setReg(idb,~getReg(idb)); //1's complement (adding the carry will make it 2's complement, not adding it will make -idb-1(borrow)
    }
    ADC();
}

void W65816::SEC()
{
    p.setC(true);
}

void W65816::SED()
{
    p.setD(true);
}

void W65816::SEI()
{
    p.setI(true);
}

void W65816::SEP()
{
    uint8_t mask = idb.low;
    if(p.emulationMode) mask &= 0b11'00'1111;

    p.setVal(p.getVal() | mask);
}

void W65816::TAX()
{
    setReg(x,acc.val());
    updateNZFlags(x.val(),true);
}

void W65816::TAY()
{
    setReg(y,acc.val());
    updateNZFlags(y.val(),true);
}

void W65816::TCD()
{
    d.set(acc.val());
    updateNZFlags(d.val(),false,true); //Use force16 here
}

void W65816::TCS()
{
    s.low = acc.low;
    if(!p.emulationMode) s.high = acc.high;
}

void W65816::TDC()
{
    acc.set(d.val());
    updateNZFlags(acc.val(),false,true); //Use force16 here
}

void W65816::TSC()
{
    acc.set(s.val());
    updateNZFlags(acc.val(),false,true); //Use force16 here
}

void W65816::TSX()
{
    setReg(x,s.val());
    updateNZFlags(x.val(),true);
}

void W65816::TXA()
{
    setReg(acc,x.val());
    updateNZFlags(acc.val());
}

void W65816::TXS()
{
    s.low = x.low;
    if(!p.emulationMode) s.high = x.high;
}

void W65816::TXY()
{
    setReg(y,x.val());
    updateNZFlags(y.val(),true);
}

void W65816::TYA()
{
    setReg(acc,y.val());
    updateNZFlags(acc.val());
}

void W65816::TYX()
{
    setReg(x,y.val());
    updateNZFlags(x.val(),true);
}

void W65816::XBA()
{
    uint8_t tmp = acc.high;
    acc.high = acc.low;
    acc.low = tmp;
    updateNZFlags(uint16_t(acc.low)<<8, false,true); //Force16 and offset the low byte so it is recognized as the upper half
}

void W65816::XCE()
{
    uint8_t cFlag = p.C();
    p.setC(p.E());
    p.setE(cFlag);
}
