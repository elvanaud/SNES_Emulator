#ifndef DMAHANDLER_H
#define DMAHANDLER_H

#include "Common.h"
#include "MemoryInterface.h"

class Bus;

const unsigned int NB_DMA_CHANNELS = 8;

class DmaHandler : MemoryInterface
{
public:
	DmaHandler();
	virtual void memoryMap(MemoryOperation op, uint32_t full_adr, uint8_t* data);
	void attachBus(Bus* b);

	void tick();

private:
	Bus * bus;

	int currentCycle = 0;

	uint8_t dmaEnable = 0;
	uint8_t hdmaEnable = 0;

	int currentTransfertUnitIndex = 0;

	enum TransferDirection {CPU_TO_IO, IO_TO_CPU};
	enum HDMA_AddressingMode {DirectTable, IndirectTable};
	//enum AddressStep {Increment,Decrement,Fixed};
	enum TransferUnitMode {NoChange,FullInc,IncAfterWord,IncDuringWord};
	struct DmaParameters{
		uint8_t val = 0;
		TransferDirection direction(){return TransferDirection((val>>7)&1);}
		HDMA_AddressingMode adrMode(){return HDMA_AddressingMode((val>>6)&1);}
		uint32_t adrStep(){
			if((val>>3)&1) return 0;
			if((val>>4)&1) return -1;
			return 1;
		}
		TransferUnitMode transferUnitMode(){
			uint8_t mode = val&0x07;
			switch(mode)
			{
			case 0:
			case 2:
			case 6:
				return NoChange;
			case 1:
			case 4:
				return FullInc;
			case 3:
			case 7:
				return IncAfterWord;
			default:
				return IncDuringWord;
			}
		}
		int transferUnitSize(){
			switch(val&0x07)
			{
				case 0: return 1;
				case 1:
				case 2:
				case 6:
					return 2;
				default: return 4;
			}
		}
	};
	DmaParameters dmaParams[NB_DMA_CHANNELS];

	uint8_t ioAddress[NB_DMA_CHANNELS];

	struct CurrentAddress{
		uint8_t bank = 0;
		uint8_t low = 0;
		uint8_t high = 0;
		uint32_t fullAdr(){return (uint32_t(bank)<<16)|(uint32_t(high)<<8)|low;}
		uint16_t adr(){return (uint32_t(high)<<8)|low;}
		void setAdr(uint16_t adr){low = adr; high = (adr>>8);}
	};
	CurrentAddress currentAddress[NB_DMA_CHANNELS];

	CurrentAddress byteCounter[NB_DMA_CHANNELS]; //Also used for hdma adr
};

#endif // DMAHANDLER_H
