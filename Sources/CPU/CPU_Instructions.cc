#include "W65816.h"

#include <iostream>
using std::cout;
using std::endl;

void W65816::AND()
{
	if(preDecodeStage)
	{
		opcodeASM = "AND";
		return;
	}
	uint16_t res = getReg(acc) & getReg(idb);
	updateNZFlags(res);
	setReg(acc,res);
}

void W65816::ADC()
{
	if(preDecodeStage)
	{
		opcodeASM = "ADC";
		return;
	}
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

void W65816::ASL()
{
	if(preDecodeStage)
	{
		opcodeASM = "ASL";
		asmAccAddFinalA = true;
		return;
	}
	int offset = 7;
	if(!p.mem8) offset = 15;

	uint16_t val = getReg(idb);
	uint8_t newC = (val>>offset)&1;
	val<<=1;
	p.setC(newC);
	updateNZFlags(val);
	setReg(idb,val);
}

void W65816::BCC()
{
	if(preDecodeStage)
	{
		opcodeASM = "BCC";
		//return; //branch inst are to be executed in predecode mode
	}
	branchTaken = !p.C();
}

void W65816::BCS()
{
	if(preDecodeStage)
	{
		opcodeASM = "BCS";
		//return; //branch inst are to be executed in predecode mode
	}
	branchTaken = p.C();
}

void W65816::BEQ()
{
	if(preDecodeStage)
	{
		opcodeASM = "BEQ";
		//return; //branch inst are to be executed in predecode mode
	}
	branchTaken = p.Z();
}

void W65816::BIT()
{
	if(preDecodeStage)
	{
		opcodeASM = "BIT";
		return;
	}
	uint16_t v = getReg(idb);
	//Page 333 of the manual: When the BIT instruction is used with the immediate addressing mode, the n and v flags are unaffected.
	//if(decodingTable[ir].AdrMode().Name() != Immediate.Name()) //old mechanism
	if(ir != 0x89) //0x89 is the opcode of Bit immediate
	{
		p.setN((v>>7)&1);
		p.setV((v>>6)&1);
	}

	uint16_t r = getReg(acc) & v;
	p.setZ(r==0);
}

void W65816::BMI()
{
	if(preDecodeStage)
	{
		opcodeASM = "BMI";
		//return; //branch inst are to be executed in predecode mode
	}
	branchTaken = p.N();
}

void W65816::BNE()
{
	if(preDecodeStage)
	{
		opcodeASM = "BNE";
		//return; //branch inst are to be executed in predecode mode
	}
	branchTaken = !p.Z();
}

void W65816::BPL()
{
	if(preDecodeStage)
	{
		opcodeASM = "BPL";
		//return; //branch inst are to be executed in predecode mode
	}
	branchTaken = !p.N();
}

void W65816::BRA()
{
	if(preDecodeStage)
	{
		opcodeASM = "BRA";
		//return; //branch inst are to be executed in predecode mode
	}
	branchTaken = true;
}

void W65816::BRK()
{
	if(preDecodeStage)
	{
		opcodeASM = "BRK";
		return;
	}
	uint16_t vecAdr = 0xFFE6;
	if(p.emulationMode)
	{
		vecAdr = 0xFFFE;
		p.setB(true);
	}
	adr.set(vecAdr);

	//++pc; //TODO
	//p.setD(false);
}

void W65816::BRL()
{
	if(preDecodeStage)
	{
		opcodeASM = "BRL";
		//return; //branch inst are to be executed in predecode mode
	}
}

void W65816::BVC()
{
	if(preDecodeStage)
	{
		opcodeASM = "BVC";
		//return; //branch inst are to be executed in predecode mode
	}
	branchTaken = !p.V();
}

void W65816::BVS()
{
	if(preDecodeStage)
	{
		opcodeASM = "BVS";
		//return; //branch inst are to be executed in predecode mode
	}
	branchTaken = p.V();
}

void W65816::CLC()
{
	if(preDecodeStage)
	{
		opcodeASM = "CLC";
		return;
	}
	p.setC(false);
}

void W65816::CLD()
{
	if(preDecodeStage)
	{
		opcodeASM = "CLD";
		return;
	}
	p.setD(false);
}

void W65816::CLI()
{
	if(preDecodeStage)
	{
		opcodeASM = "CLI";
		return;
	}
	p.setI(false);
}

void W65816::CLV()
{
	if(preDecodeStage)
	{
		opcodeASM = "CLV";
		return;
	}
	p.setV(false);
}

void W65816::CMP()
{
	if(preDecodeStage)
	{
		opcodeASM = "CMP";
		return;
	}
	uint16_t a = getReg(acc);
	uint16_t b = getReg(idb);
	uint16_t r = a - b;
	updateNZFlags(r);
	p.setC(a >= b);
}

void W65816::COP()
{
	if(preDecodeStage)
	{
		opcodeASM = "COP";
		return;
	}
	uint16_t vecAdr = 0xFFE4;
	if(p.emulationMode) vecAdr = 0xFFF4;
	adr.set(vecAdr);

	//p.setD(false);
	//++pc; //TODO
}

void W65816::CPX()
{
	if(preDecodeStage)
	{
		opcodeASM = "CPX";
		isIndexRelated=true;
		return;
	}
	uint16_t a = getReg(x);
	uint16_t b = getReg(idb);
	uint16_t r = a - b;
	updateNZFlags(r,true);
	p.setC(a >= b);
}

void W65816::CPY()
{
	if(preDecodeStage)
	{
		opcodeASM = "CPY";
		isIndexRelated=true;
		return;
	}
	uint16_t a = getReg(y);
	uint16_t b = getReg(idb);
	uint16_t r = a - b;
	updateNZFlags(r,true);
	p.setC(a >= b);
}

void W65816::DEC()
{
	if(preDecodeStage)
	{
		opcodeASM = "DEC";
		return;
	}
	uint16_t val = getReg(idb);
	--val;
	updateNZFlags(val);
	setReg(idb,val);
}

void W65816::DEX()
{
	if(preDecodeStage)
	{
		opcodeASM = "DEX";
		return;
	}
	uint16_t r = getReg(x);
	--r;
	updateNZFlags(r,true);
	setReg(x,r);
}

void W65816::DEY()
{
	if(preDecodeStage)
	{
		opcodeASM = "DEY";
		return;
	}
	uint16_t r = getReg(y);
	--r;
	updateNZFlags(r,true);
	setReg(y,r);
}

void W65816::EOR()
{
	if(preDecodeStage)
	{
		opcodeASM = "EOR";
		return;
	}
	uint16_t r = getReg(acc)^getReg(idb);
	updateNZFlags(r);
	setReg(acc,r);
}

void W65816::INC()
{
	if(preDecodeStage)
	{
		opcodeASM = "INC";
		return;
	}
	uint16_t val = getReg(idb);
	++val;
	updateNZFlags(val);
	setReg(idb,val);
}

void W65816::INX()
{
	if(preDecodeStage)
	{
		opcodeASM = "INX";
		return;
	}
	uint16_t r = getReg(x);
	++r;
	updateNZFlags(r,true);
	setReg(x,r);
}

void W65816::INY()
{
	if(preDecodeStage)
	{
		opcodeASM = "INY";
		return;
	}
	uint16_t r = getReg(y);
	++r;
	updateNZFlags(r,true);
	setReg(y,r);
}

void W65816::JMP()
{
	if(preDecodeStage)
	{
		opcodeASM = "JMP";
		return;
	}
}

void W65816::JML()
{
	if(preDecodeStage)
	{
		opcodeASM = "JMP";//used to be jml but bsnes uses "jmp" (too bad)
		return;
	}
}

void W65816::JSR()
{
	if(preDecodeStage)
	{
		opcodeASM = "JSR";
		return;
	}
}

void W65816::JSL()
{
	if(preDecodeStage)
	{
		opcodeASM = "JSL";
		return;
	}
}

void W65816::LDA()
{
	if(preDecodeStage)
	{
		opcodeASM = "LDA";
		return;
	}
	uint16_t value = getReg(idb);
	setReg(acc,value);
	updateNZFlags(value);
}

void W65816::LDX()
{
	if(preDecodeStage)
	{
		opcodeASM = "LDX";
		isIndexRelated=true;
		return;
	}
	uint16_t value = getReg(idb);
	setReg(x,value);
	updateNZFlags(value,true);
}

void W65816::LDY()
{
	if(preDecodeStage)
	{
		opcodeASM = "LDY";
		isIndexRelated=true;
		return;
	}
	uint16_t value = getReg(idb);
	setReg(y,value);
	updateNZFlags(value,true);
}

void W65816::LSR()
{
	if(preDecodeStage)
	{
		opcodeASM = "LSR";
		asmAccAddFinalA = true;
		return;
	}
	uint16_t val = getReg(idb);
	uint8_t newC = val & 1;
	val>>=1;
	p.setC(newC);
	updateNZFlags(val);
	setReg(idb,val);
}

void W65816::MVP()
{
	if(preDecodeStage)
	{
		opcodeASM = "MVP";
		return;
	}
}

void W65816::MVN()
{
	if(preDecodeStage)
	{
		opcodeASM = "MVN";
		return;
	}
}

void W65816::NOP()
{
	if(preDecodeStage)
	{
		opcodeASM = "NOP";
		return;
	}
}

void W65816::ORA()
{
	if(preDecodeStage)
	{
		opcodeASM = "ORA";
		return;
	}
	uint16_t res = getReg(acc) | getReg(idb);
	setReg(acc,res);
	updateNZFlags(res);
}

void W65816::PEA()
{
	if(preDecodeStage)
	{
		opcodeASM = "PEA";
		return;
	}
}

void W65816::PER()
{
	if(preDecodeStage)
	{
		opcodeASM = "PER";
		return;
	}
}

void W65816::PEI()
{
	if(preDecodeStage)
	{
		opcodeASM = "PEI";
		return;
	}
}

void W65816::PHA()
{
	if(preDecodeStage)
	{
		opcodeASM = "PHA";
		//return; //StackPush instructions are executed in predecode mode
	}
	idb.set(acc.val());
	updateNZFlags(getReg(acc));
}

void W65816::PHB()
{
	if(preDecodeStage)
	{
		opcodeASM = "PHB";
		return;
	}
	idb.set(dbr);
}

void W65816::PHP()
{
	if(preDecodeStage)
	{
		opcodeASM = "PHP";
		return;
	}
	idb.set(p.getVal());
}

void W65816::PHD()
{
	if(preDecodeStage)
	{
		opcodeASM = "PHD";
		return;
	}
	idb.set(d.val());
}

void W65816::PHK()
{
	if(preDecodeStage)
	{
		opcodeASM = "PHK";
		return;
	}
	idb.set(pbr);
}

void W65816::PHX()
{
	if(preDecodeStage)
	{
		opcodeASM = "PHX";
		isIndexRelated=true;
		//return; //StackPush instructions are executed in predecode mode
	}
	idb.set(x.val());
}

void W65816::PHY()
{
	if(preDecodeStage)
	{
		opcodeASM = "PHY";
		isIndexRelated=true;
		//return; //StackPush instructions are executed in predecode mode
	}
	idb.set(y.val());
}

void W65816::PLA()
{
	if(preDecodeStage)
	{
		opcodeASM = "PLA";
		return;
	}
	setReg(acc,idb.val());
	updateNZFlags(getReg(acc));
}

void W65816::PLB()
{
	if(preDecodeStage)
	{
		opcodeASM = "PLB";
		return;
	}
	dbr = idb.low;
	//updateNZFlags(dbr); need to force 8 bit (I'll do that manually here =>)
	p.setZ(dbr == 0);
	p.setN(dbr>>7);
}

void W65816::PLD()
{
	if(preDecodeStage)
	{
		opcodeASM = "PLD";
		return;
	}
	d.set(idb.val());
	updateNZFlags(d.val(), false, true); //use force16 here
}

void W65816::PLP()
{
	if(preDecodeStage)
	{
		opcodeASM = "PLP";
		return;
	}
	p.setVal(idb.low);
}

void W65816::PLX()
{
	if(preDecodeStage)
	{
		opcodeASM = "PLX";
		isIndexRelated=true;
		return;
	}
	setReg(x,idb.val());
	updateNZFlags(x.val(),true);
}

void W65816::PLY()
{
	if(preDecodeStage)
	{
		opcodeASM = "PLY";
		isIndexRelated=true;
		return;
	}
	setReg(y,idb.val());
	updateNZFlags(y.val(),true);
}

void W65816::REP()
{
	if(preDecodeStage)
	{
		opcodeASM = "REP";
		return;
	}
	uint8_t mask = idb.low;
	if(p.emulationMode) mask &= 0b11'00'1111;

	p.setVal(p.getVal() & ~mask);

	if(p.index8)
	{
		x.high = 0;
		y.high = 0;
	}
}

void W65816::ROL()
{
	if(preDecodeStage)
	{
		opcodeASM = "ROL";
		asmAccAddFinalA = true;
		return;
	}
	int offset = 7;
	if(!p.mem8) offset = 15;

	uint16_t val = getReg(idb);
	uint8_t newC = (val>>offset)&1;
	val<<=1;
	val |= p.C();
	p.setC(newC);
	updateNZFlags(val);
	setReg(idb,val);
}

void W65816::ROR()
{
	if(preDecodeStage)
	{
		opcodeASM = "ROR";
		return;
	}
	int offset = 7;
	if(!p.mem8) offset = 15;

	uint16_t val = getReg(idb);
	uint8_t newC = val&1; //(val>>offset)&1;
	val>>=1;
	val |= uint16_t(p.C())<<offset;
	p.setC(newC);
	updateNZFlags(val);
	setReg(idb,val);
}

void W65816::RTI()
{
	if(preDecodeStage)
	{
		opcodeASM = "RTI";
		return;
	}
}

void W65816::RTL()
{
	if(preDecodeStage)
	{
		opcodeASM = "RTL";
		return;
	}
}

void W65816::RTS()
{
	if(preDecodeStage)
	{
		opcodeASM = "RTS";
		return;
	}
}

void W65816::SBC()
{
	if(preDecodeStage)
	{
		opcodeASM = "SBC";
		return;
	}
	if(p.D())
	{
		uint16_t complement = p.mem8 ? 0x99 : 0x9999;
		setReg(idb, complement - getReg(idb)); //acc+(-idb-1)+p.c
	}
	else
	{
		setReg(idb,~getReg(idb)); //1's complement (adding the carry will make it 2's complement, not adding it will make -idb-1(borrow)
	}
	ADC();
}

void W65816::SEC()
{
	if(preDecodeStage)
	{
		opcodeASM = "SEC";
		return;
	}
	p.setC(true);
}

void W65816::SED()
{
	if(preDecodeStage)
	{
		opcodeASM = "SED";
		return;
	}
	p.setD(true);
}

void W65816::SEI()
{
	if(preDecodeStage)
	{
		opcodeASM = "SEI";
		return;
	}
	p.setI(true);
}

void W65816::SEP()
{
	if(preDecodeStage)
	{
		opcodeASM = "SEP";
		return;
	}
	uint8_t mask = idb.low;
	if(p.emulationMode) mask &= 0b11'00'1111;

	p.setVal(p.getVal() | mask);
}

void W65816::STA()
{
	if(preDecodeStage)
	{
		opcodeASM = "STA";
		return;
	}
	setReg(idb, acc.val());
	//cout<<"[STA]idb="<<std::hex<<int(idb.val())<<" adr="<<int(adr.val())<<endl;
}

void W65816::STP()
{
	if(preDecodeStage)
	{
		opcodeASM = "STP";
		return;
	}
	clockStopped = true;
}

void W65816::STX()
{
	if(preDecodeStage)
	{
		opcodeASM = "STX";
		isIndexRelated=true;
		return;
	}
	setReg(idb, x.val());
}

void W65816::STY()
{
	if(preDecodeStage)
	{
		opcodeASM = "STY";
		isIndexRelated=true;
		return;
	}
	setReg(idb, y.val());
}

void W65816::STZ()
{
	if(preDecodeStage)
	{
		opcodeASM = "STZ";
		return;
	}
	setReg(idb, 0);
}

void W65816::TAX()
{
	if(preDecodeStage)
	{
		opcodeASM = "TAX";
		return;
	}
	setReg(x,acc.val());
	updateNZFlags(x.val(),true);
}

void W65816::TAY()
{
	if(preDecodeStage)
	{
		opcodeASM = "TAY";
		return;
	}
	setReg(y,acc.val());
	updateNZFlags(y.val(),true);
}

void W65816::TCD()
{
	if(preDecodeStage)
	{
		opcodeASM = "TCD";
		return;
	}
	d.set(acc.val());
	updateNZFlags(d.val(),false,true); //Use force16 here
}

void W65816::TCS()
{
	if(preDecodeStage)
	{
		opcodeASM = "TCS";
		return;
	}
	s.low = acc.low;
	if(!p.emulationMode) s.high = acc.high;
}

void W65816::TDC()
{
	if(preDecodeStage)
	{
		opcodeASM = "TDC";
		return;
	}
	acc.set(d.val());
	updateNZFlags(acc.val(),false,true); //Use force16 here
}

void W65816::TRB()
{
	if(preDecodeStage)
	{
		opcodeASM = "TRB";
		return;
	}
	uint16_t val = getReg(idb);
	setReg(idb, ~getReg(acc) & val);
	p.setZ((getReg(acc) & val) == 0);
}

void W65816::TSB()
{
	if(preDecodeStage)
	{
		opcodeASM = "TSB";
		return;
	}
	uint16_t val = getReg(idb);
	setReg(idb, getReg(acc) | val);
	p.setZ((getReg(acc) & val) == 0);
}

void W65816::TSC()
{
	if(preDecodeStage)
	{
		opcodeASM = "TSC";
		return;
	}
	acc.set(s.val());
	updateNZFlags(acc.val(),false,true); //Use force16 here
}

void W65816::TSX()
{
	if(preDecodeStage)
	{
		opcodeASM = "TSX";
		return;
	}
	setReg(x,s.val());
	updateNZFlags(x.val(),true);
}

void W65816::TXA()
{
	if(preDecodeStage)
	{
		opcodeASM = "TXA";
		return;
	}
	setReg(acc,x.val());
	updateNZFlags(acc.val());
}

void W65816::TXS()
{
	if(preDecodeStage)
	{
		opcodeASM = "TXS";
		return;
	}
	s.low = x.low;
	if(!p.emulationMode) s.high = x.high;
}

void W65816::TXY()
{
	if(preDecodeStage)
	{
		opcodeASM = "TXY";
		return;
	}
	setReg(y,x.val());
	updateNZFlags(y.val(),true);
}

void W65816::TYA()
{
	if(preDecodeStage)
	{
		opcodeASM = "TYA";
		return;
	}
	setReg(acc,y.val());
	updateNZFlags(acc.val());
}

void W65816::TYX()
{
	if(preDecodeStage)
	{
		opcodeASM = "TYX";
		return;
	}
	setReg(x,y.val());
	updateNZFlags(x.val(),true);
}

void W65816::WAI()
{
	if(preDecodeStage)
	{
		opcodeASM = "WAI";
		return;
	}
	rdy = false;
}

void W65816::WDM()
{
	if(preDecodeStage)
	{
		opcodeASM = "WDM";
		return;
	}
}

void W65816::XBA()
{
	if(preDecodeStage)
	{
		opcodeASM = "XBA";
		return;
	}
	uint8_t tmp = acc.high;
	acc.high = acc.low;
	acc.low = tmp;
	updateNZFlags(uint16_t(acc.low)<<8, false,true); //Force16 and offset the low byte so it is recognized as the upper half
}

void W65816::XCE()
{
	if(preDecodeStage)
	{
		opcodeASM = "XCE";
		return;
	}
	uint8_t cFlag = p.C();
	p.setC(p.E());
	p.setE(cFlag);
}
