#include "Bus.h"
#include "W65816.h"

Bus::Bus(W65816 & c) : cpu(c)
{

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
    }
}
