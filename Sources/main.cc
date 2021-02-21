#include <iostream>

#include "Common.h"

#include "CPU/W65816.h"
#include "Bus.h"
#include "APU/SNES_APU.h"
#include "APU/SPC700.h"

int main()
{
    cout << "SNES Emulator" << endl;

    W65816 cpu;

    SPC700 spc;
    SNES_APU apu(spc);
    spc.attachBus(&apu);
    spc.reset();

    Bus bus(cpu, apu);
    cpu.attachBus(&bus);
    apu.attachBus(&bus);

    //bus.loadCartridge("6502_functional_test.bin");
    //bus.copyInMemory(0xFFFC,{0x00,04});

    bus.loadCartridge("Games/The Legend of Zelda - A Link to the Past.smc");

    bus.run();

    return 0;
}
