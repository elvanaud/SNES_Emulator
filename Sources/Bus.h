#ifndef Bus_H
#define Bus_H

#include <vector>
using std::vector;

#include "Common.h"

#include "ConsoleDebugger.h"
#include "Cartridge.h"

class W65816;

class Bus : MemoryInterface
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

    virtual void memoryMap(MemoryOperation op, uint32_t full_adr, uint8_t *data);
private:
    W65816 &cpu;
    ConsoleDebugger debugger;
    Cartridge cartridge;

    uint8_t dmr = 0;
    uint8_t ram[2][BANK_SIZE];
};

#endif //Bus_H
