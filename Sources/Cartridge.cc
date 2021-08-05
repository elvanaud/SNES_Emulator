#include "Cartridge.h"

Cartridge::Cartridge()
{
	//ctor
}

Cartridge::~Cartridge()
{
	for(unsigned int i = 0; i < NumberOfROMBanks; ++i)
	{
		delete romBanks[i];
	}
}

void Cartridge::memoryMap(MemoryOperation op, uint32_t full_adr, uint8_t *data)
{
	full_adr &= ~(1<<23); //Ignoring 23rd bit (mirroring across the center ram banks)

	uint8_t bank = (full_adr>>16)&0xFF;
	uint16_t adr = full_adr & 0xFFFF;
	adr &= 0x7FFF; //Ignoring A15

	//cout << "Cartridge map: "<<(int) full_adr<<" => " << (int) romBanks[bank][adr]<<endl;
	if(bank >= NumberOfROMBanks)
	{
		switch(op)
		{
			case MemoryOperation::Write: return;
			case MemoryOperation::Read: *data = 0; return;
		}
	}

	doMemoryOperation(op, &romBanks[bank][adr], data);
}

void Cartridge::load(std::ifstream & inputFile, CartridgeHeader p_header)
{
	header = p_header;

	if(header.mapMode != CartridgeHeader::LoROM)
	{
		assert(false);
		return;
	}

	NumberOfROMBanks = header.romSize / HALF_BANK_SIZE;
	BankSize = HALF_BANK_SIZE;

	assert(NumberOfROMBanks <= MAX_BANKS);

	for(unsigned int i = 0; i < NumberOfROMBanks; ++i)
	{
		romBanks[i] = new uint8_t[BankSize];
		inputFile.read((char*)romBanks[i], BankSize);
	}
}
