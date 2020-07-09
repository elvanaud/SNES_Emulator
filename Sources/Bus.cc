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
    ram[0x104] = 0x29;
    ram[0x105] = 0x3F;
    ram[0x106] = 0x89;
    ram[0x107] = 0x00;
    ram[0x108] = 0x89;
    ram[0x109] = 0x02;
    ram[0x10A] = 0xC9;
    ram[0x10B] = 0x0A;
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
        cout << "TCycle = " << cpu.getTCycle() << endl;
        cpu.tick();

        cout << "PC = " << cpu.getPC() << "  ;  IR = " << (int)cpu.getIR() << "  ;  Acc = " << cpu.getAcc() << "  ;  Adr = " << cpu.getAdr();
        cout << "  ;  IDB = " << cpu.getIDB() <<endl;
        uint8_t p = cpu.getP();
        string status;
        if((p>>7)&1) status+="N"; else status += "-";
        if((p>>6)&1) status+="V"; else status += "-";
        if((p>>1)&1) status+="Z"; else status += "-";
        if((p>>0)&1) status+="C"; else status += "-";
        cout << "Flags = " << status << endl;

        if(cpu.VDA() && cpu.VPA()) //Sync = Opcode Fetch
        {
            std::getchar();
        }
    }
}
