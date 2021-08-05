#include <iostream>

#include "Common.h"

#include "CPU/W65816.h"
#include "Bus.h"
#include "APU/SNES_APU.h"
#include "APU/SPC700.h"
#include "PPU/SNES_PPU.h"
#include "PPU/DmaHandler.h"

int main()
{
	cout << "SNES Emulator" << endl;

	W65816 cpu;

	SPC700 spc;
	SNES_APU apu(spc);
	spc.attachBus(&apu);
	spc.reset();

	SNES_PPU ppu;
	DmaHandler dma;

	Bus bus(cpu, apu, ppu, dma); //TODO: c'est moche et inutile => Corrige ce code
	cpu.attachBus(&bus);
	apu.attachBus(&bus);

	//bus.loadCartridge("6502_functional_test.bin");
	//bus.copyInMemory(0xFFFC,{0x00,04});

	bus.loadCartridge("Games/The Legend of Zelda - A Link to the Past.smc");
	//bus.loadCartridge("Games/Super Mario World (Patch FR).smc");

	bus.run();

	return 0;
}
