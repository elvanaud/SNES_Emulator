#ifndef Bus_H
#define Bus_H

#include <cstdint>
#include <vector>
using std::vector;

#include "ConsoleDebugger.h"

class W65816;

const unsigned int RAM_QUANTITY = 0x1'00'00;

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
    void loadCartridge(std::string const & path);
private:
    W65816 &cpu;
    ConsoleDebugger debugger;

    enum MemType {LoROM, HiROM};
    MemType memType;

    uint8_t dmr = 0;
    uint8_t ram1[RAM_QUANTITY];
    uint8_t ram2[RAM_QUANTITY];

    uint8_t* lorom[64]; //8000
    uint8_t* hirom[64];//62 or 64 banks on waitstate //1'00'00
};

#endif //Bus_H
