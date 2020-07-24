#ifndef Bus_H
#define Bus_H

#include <cstdint>
#include <vector>
using std::vector;

#include "ConsoleDebugger.h"

class W65816;

const unsigned int RAM_QUANTITY = 0x1'00'00; //TODO

class Bus
{
public:
    Bus(W65816 &c);

    void run();

    void read(uint32_t adr);
    uint8_t privateRead(uint32_t adr);
    void write(uint32_t adr, uint8_t data);
    uint8_t DMR();

    void copyInMemory(uint32_t adr, vector<uint8_t> const & buffer);
private:
    W65816 &cpu;
    ConsoleDebugger debugger;

    uint8_t dmr = 0;
    uint8_t ram[RAM_QUANTITY];
};

#endif //Bus_H
