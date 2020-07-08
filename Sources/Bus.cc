#include "Bus.h"
#include "W65816.h"

Bus::Bus(W65816 & c) : cpu(c)
{

}

void Bus::run()
{
    while(true)
    {
        cpu.tick();
    }
}
