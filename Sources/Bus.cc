#include "Bus.h"
#include "W65816.h"

#include <cstdio>
#include <iostream>
using std::cout;
using std::endl;

Bus::Bus(W65816 & c) : cpu(c)
{
    ram[0x100] = 0x69;
    ram[0x101] = 19;
    ram[0x102] = 0x69;
    ram[0x103] = 55;
}

void Bus::read(uint32_t adr)
{
    if (!cpu.VDA() && !cpu.VPA()) return;

    dmr = ram[adr];
}

uint8_t Bus::DMR()
{
    return dmr;
}

void Bus::run()
{
    while(true)
    {
        cpu.tick();

        cout << "PC = " << cpu.getPC() << "  ;  IR = " << (int)cpu.getIR() << "  ;  Acc = " << cpu.getAcc() << endl;

        if(cpu.VDA() && cpu.VPA()) //Sync = Opcode Fetch
        {
            std::getchar();
        }
    }
}
