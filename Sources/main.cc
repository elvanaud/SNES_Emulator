#include <iostream>

#include "CPU/W65816.h"
#include "Bus.h"

using std::cout;
using std::endl;

int main()
{
    cout << "SNES Emulator - ArthurTendo" << endl;

    W65816 cpu;
    Bus bus(cpu);
    cpu.attachBus(&bus);

    bus.loadCartridge("Games/6502_functional_test.bin");
    bus.copyInMemory(0xFFFC,{0x00,04});

    bus.run();

    return 0;
}
