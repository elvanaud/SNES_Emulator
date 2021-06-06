#ifndef Bus_H
#define Bus_H

#include <vector>
using std::vector;

#include "Common.h"

#include "ConsoleDebugger.h"
#include "Cartridge.h"

class W65816;
class SNES_APU;
class SNES_PPU;
class DmaHandler;

class Bus : MemoryInterface
{
public:
    Bus(W65816 &c, SNES_APU &p_apu, SNES_PPU &p_ppu, DmaHandler &p_dma);

    void run();

    void read(uint32_t adr);
    uint8_t privateRead(uint32_t adr);
    void privateWrite(uint32_t adr, uint8_t data);
    void write(uint32_t adr, uint8_t data);
    uint8_t DMR();

    void copyInMemory(uint32_t adr, vector<uint8_t> const & buffer);
    void loadCartridge(std::string const & path);

    virtual void memoryMap(MemoryOperation op, uint32_t full_adr, uint8_t *data);

    void dmaEnable(bool enable);

    uint32_t accessedAdr = 0;
    bool isDataLoaded = false;
private:
    W65816 &cpu;
    SNES_APU &apu;
    SNES_PPU &ppu;
    DmaHandler &dmaHandler;
    ConsoleDebugger debugger;
    Cartridge cartridge;

    bool dmaEnabled = false;

    enum MemType {LoROM, HiROM};
    MemType memType;

    uint8_t dmr = 0;
    uint8_t ram[2][BANK_SIZE];

    uint8_t WMADDH = 0;
    uint8_t WMADDM = 0;
    uint8_t WMADDL = 0;
};

#endif //Bus_H
