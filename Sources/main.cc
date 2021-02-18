#include <iostream>

#include "CPU/W65816.h"
#include "Bus.h"

using std::cout;
using std::endl;

int main()
{
    cout << "SNES Emulator" << endl;

    W65816 cpu;
    Bus bus(cpu);
    cpu.attachBus(&bus);

    //bus.loadCartridge("6502_functional_test.bin");
    //bus.copyInMemory(0xFFFC,{0x00,04});

    bus.loadCartridge("Games/The Legend of Zelda - A Link to the Past.smc");

    bus.run();

    return 0;
}
