#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include "MemoryInterface.h"
#include "CartridgeHeader.h"
#include "Common.h"

#include <fstream>

class Cartridge : MemoryInterface
{
public:
	Cartridge();
	virtual ~Cartridge();

	void memoryMap(MemoryOperation op, uint32_t full_adr, uint8_t *data);
	void load(std::ifstream & inputFile, CartridgeHeader header);

	CartridgeHeader header;

private:
	unsigned int NumberOfROMBanks = MAX_BANKS;
	unsigned int BankSize = 0;

	uint8_t* romBanks[MAX_BANKS];
};

#endif // CARTRIDGE_H
