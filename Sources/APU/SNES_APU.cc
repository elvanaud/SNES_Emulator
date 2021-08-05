#include "SNES_APU.h"
#include "Bus.h"
#include "SPC700.h"

SNES_APU::SNES_APU(SPC700 &p_spc):spc(p_spc)
{
	for(int i = 0; i < 4; ++i)
	{
		ia[i] = oa[i] = 0;
	}
}

void SNES_APU::attachBus(Bus* p_bus)
{
	bus = p_bus;
}

uint8_t SNES_APU::read(uint16_t adr)
{
	uint8_t data = 0;
	memoryMap(Read, adr, &data);
	return data;
}

void SNES_APU::write(uint16_t adr, uint8_t data)
{
	memoryMap(Write, adr, &data);
}


void SNES_APU::tick()
{
	spc_clock--;

	if(spc_clock == 0)
	{
		spc_clock = SPC_CLOCK;
		spc.tick();
	}
	

	/*oa0 = ia0;
	if(ia1 == 0 && dataToProcess) //entry command
	{
		cout << "[APU] entry command triggered"<<endl;
		dataToProcess = false;
		oa0 = 0xAA;
		oa1 = 0xBB;
		ia0 = oa0;
	}*/
}

void SNES_APU::mainBusIO(MemoryOperation op, uint32_t full_adr, uint8_t *data)
{
	int adr = full_adr-0x2140;
	adr %= 4; //Mirorring
	//From the main bus POV
	if(op==Read)
		*data = oa[adr];
	else
		ia[adr] = *data;
	//cout <<"[APU]oa read:"<<std::hex<<int(adr)<<" val="<<int(*data)<<endl;
}

void SNES_APU::memoryMap(MemoryOperation op, uint32_t full_adr, uint8_t *data)
{
	uint16_t adr = full_adr; //Theoretically useless bit it's shorter to write

	doMemoryOperation(op,&ram[adr],data);
	if(control.bootROMEnabled() && op == Read && adr >= 0xFFC0)
	{
		*data = bootROM[adr-0xFFC0];
	}
	if(adr >= 0x00F0 && adr <= 0x00FF)//IO ports
	{
		switch(adr & 0xFF)
		{
		case 0xF0: //Test reg, TODO
			break;
		case 0xF1:
			doMemoryOperation(op,&control.val,data); //TODO: write triggers timers and resets io ports
			break;
		case 0xF4: case 0xF5:
		case 0xF6: case 0xF7:
			if(op==Read)
			{
				*data = ia[adr-0x00F4];
			}
			else
			{
				oa[adr-0x00F4] = *data;
				//cout <<"[APU]oa set:"<<std::hex<<int(adr)<<" val="<<int(*data)<<endl;
			}
			break;
		}
		return;
	}
}
