#include "DmaHandler.h"

#include "Bus.h"

DmaHandler::DmaHandler()
{
	//ctor
}

void DmaHandler::attachBus(Bus * b)
{
	bus = b;
}

void DmaHandler::memoryMap(MemoryOperation op, uint32_t full_adr, uint8_t* data)
{
	uint16_t adr = full_adr;
	uint8_t dmaChannel = (adr>>4)&0x0F;
	uint8_t reg = adr&0x0F;

	if(adr == 0x420B)
	{
		doMemoryOperation(op,&dmaEnable,data);
	}
	else if(adr == 0x420C)
	{
		doMemoryOperation(op,&hdmaEnable,data);
	}
	else if(reg == 0x00)
	{
		doMemoryOperation(op,&dmaParams[dmaChannel].val,data);
	}
	else if(reg == 0x01)
	{
		doMemoryOperation(op,&ioAddress[dmaChannel],data);
	}
	else if(reg == 0x02)
	{
		doMemoryOperation(op,&currentAddress[dmaChannel].low,data);
	}
	else if(reg == 0x03)
	{
		doMemoryOperation(op,&currentAddress[dmaChannel].high,data);
	}
	else if(reg == 0x04)
	{
		doMemoryOperation(op,&currentAddress[dmaChannel].bank,data);
	}
	else if(reg == 0x05)
	{
		doMemoryOperation(op,&byteCounter[dmaChannel].low,data);
	}
	else if(reg == 0x06)
	{
		doMemoryOperation(op,&byteCounter[dmaChannel].high,data);
	}
	else if(reg == 0x07)
	{
		doMemoryOperation(op,&byteCounter[dmaChannel].bank,data);
	}
}

void DmaHandler::tick()
{
	if((dmaEnable & hdmaEnable) != 0) //just check if we don't enable both dma and hdma on same channel (should not happen
	{
		cout << "[DMA] error: channel enabled both in dma and hdma"<<endl;
	}

	if(dmaEnable)
	{
		bus->dmaEnable(true);
		if(currentCycle == 0) //transfer one byte in 8 cycles
		{
			currentCycle = 8;
			int channelNumber = 0;
			for(; channelNumber < NB_DMA_CHANNELS; ++channelNumber)
			{
				if((dmaEnable>>channelNumber)&1) break;
			}

			uint16_t byteCount = byteCounter[channelNumber].adr();
			--byteCount;
			byteCounter[channelNumber].setAdr(byteCount);

			uint32_t cpuAdr = currentAddress[channelNumber].fullAdr();
			uint8_t ioAdr = ioAddress[channelNumber];

			switch(dmaParams[channelNumber].transferUnitMode())
			{
			case FullInc:
				ioAdr += currentTransfertUnitIndex;
				break;
			case IncAfterWord:
				if(currentTransfertUnitIndex >= 2)
					++ioAdr;
				break;
			case IncDuringWord:
				if(currentTransfertUnitIndex&1)
					++ioAdr;
				break;
			default:
				;//don't change the io address
			}

			uint32_t finalIoAdr = 0x2100|ioAdr;

			if(dmaParams[channelNumber].direction() == CPU_TO_IO)
			{
				uint8_t data = bus->privateRead(cpuAdr);
				bus->privateWrite(finalIoAdr,data);
			}
			else
			{
				uint8_t data = bus->privateRead(finalIoAdr);
				bus->privateWrite(cpuAdr,data);
			}

			++currentTransfertUnitIndex;
			currentTransfertUnitIndex%= dmaParams[channelNumber].transferUnitSize();

			uint16_t tmp = currentAddress[channelNumber].adr();
			tmp += dmaParams[channelNumber].adrStep();
			currentAddress[channelNumber].setAdr(tmp);

			if(byteCount == 0) //Transfer over
			{
				dmaEnable &= ~(1<<channelNumber);
			}
		}
		--currentCycle;
	}
	else
	{
		bus->dmaEnable(false);
		currentCycle = 0;
	}
}
