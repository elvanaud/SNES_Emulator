#include "W65816.h"
#include "../Bus.h"

#include <iostream>
using std::cout;
using std::endl;

void W65816::dummyStage()
{
	; //Dummy
}

void W65816::dummyFetchLast()
{
	handleValidAddressPINS(InternalOperation);
	bus->read(addressBusBuffer);
}

void W65816::dummyFetch(Register16 *src)
{
	handleValidAddressPINS(InternalOperation);
	generateAddress(src->val());
	bus->read(addressBusBuffer);
}

void W65816::dummyFetchLong(uint8_t * bank, Register16 *src)
{
	forceTmpBank = true;
	tmpBank = *bank;
	dummyFetch(src);
	forceTmpBank = false;
}

void W65816::fetch(Register16 *src, uint8_t * dst)
{
	ValidAddressState state = DataFetch;
	if(src == &pc)
	{
		state = OperandFetch;
		if(preDecodeStage && tcycle == 0) state = OpcodeFetch;
	}
	handleValidAddressPINS(state);
	generateAddress(src->val());
	bus->read(addressBusBuffer);
	*dst =  bus->DMR();
}

void W65816::fetchInc(Register16 *src, uint8_t * dst)
{
	fetch(src,dst);
	++(*src);
}

void W65816::fetchDec(Register16 *src, uint8_t * dst)
{
	fetch(src,dst);
	--(*src);
}

void W65816::fetchLong(uint8_t * bank, Register16 *src, uint8_t * dst)
{
	forceTmpBank = true;
	tmpBank = *bank;
	fetch(src,dst);
	forceTmpBank = false;
}

void W65816::fetchIncLong(uint8_t * bank, Register16 *src, uint8_t * dst)
{
	fetchLong(bank,src,dst);
	++(*src);
}

void W65816::fetchDecLong(uint8_t * bank, Register16 *src, uint8_t * dst)
{
	fetchLong(bank,src,dst);
	--(*src);
}

void W65816::moveReg8(uint8_t * src, uint8_t * dst)
{
	*dst = *src;
}

void W65816::moveReg16(Register16 * src, Register16 * dst)
{
	dst->set(src->val());
}

void W65816::write(Register16 *adr, uint8_t * data)
{
	handleValidAddressPINS(DataFetch);
	generateAddress(adr->val());
	bus->write(addressBusBuffer, *data);
	//cout << "[CPU_WRITE] adrBuffer="<<std::hex<<addressBusBuffer<<" data="<<int(*data)<<endl;
}

void W65816::writeInc(Register16 * adr, uint8_t * data)
{
	write(adr,data);
	++(*adr);
}

void W65816::writeDec(Register16 * adr, uint8_t * data)
{
	write(adr,data);
	--(*adr);
}

void W65816::writeLong(uint8_t * bank, Register16 * adr, uint8_t * data)
{
	forceTmpBank = true;
	tmpBank = *bank;
	write(adr,data);
	forceTmpBank = false;
}

void W65816::writeIncLong(uint8_t * bank, Register16 * adr, uint8_t * data)
{
	writeLong(bank,adr,data);
	++(*adr);
}

void W65816::writeDecLong(uint8_t * bank, Register16 * adr, uint8_t * data)
{
	writeLong(bank,adr,data);
	--(*adr);
}

void W65816::push(uint8_t * src)
{
	/*handleValidAddressPINS(DataFetch);
	generateAddress(0,s.val());
	bus->write(addressBusBuffer, *src);
	--s;*/
	tmpBank = 0;
	writeDecLong(&tmpBank,&s,src);
}

void W65816::pop(uint8_t * dst)
{
	/*handleValidAddressPINS(DataFetch);
	generateAddress(0,s.val());
	bus->read(addressBusBuffer);
	*dst =  bus->DMR();
	++s;*/
	tmpBank = 0;
	++s;
	fetchLong(&tmpBank,&s,dst);
}

void W65816::pushP()
{
	uint8_t v = p.getVal();
	push(&v);
}

void W65816::popP()
{
	uint8_t v;
	pop(&v);
	p.setVal(v);
}

void W65816::halfAdd(uint8_t * dst, uint8_t * op)
{
	uint16_t r = uint16_t(*dst) + *op;
	internalCarryBuffer = r>>8;
	*dst = r;

	SIGN_EXTENDED_OP_HALF_ADD = 0;
	if(((*op)>>7)&1)
		SIGN_EXTENDED_OP_HALF_ADD = 0xFF;
}

void W65816::fixCarry(uint8_t * dst, uint8_t * op)
{
	*dst = *dst + *op + internalCarryBuffer;
	internalCarryBuffer = 0;

	SIGN_EXTENDED_OP_HALF_ADD = 0;
}

void W65816::fullAdd(Register16 * dst, Register16 * op)
{
	dst->set(dst->val()+op->val());
}

void W65816::decReg(Register16 * reg)
{
	--(*reg);
}

void W65816::incReg(Register16 * reg)
{
	++(*reg);
}

void W65816::enableInterupts(bool enable)
{
	p.setI(!enable);
	if(!enable) p.setD(false);
}
