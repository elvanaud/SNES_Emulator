#ifndef SNES_APU_H
#define SNES_APU_H

#include "Common.h"
#include "MemoryInterface.h"

class Bus;

class SNES_APU : MemoryInterface
{
public:
    SNES_APU();
    void tick();
    void attachBus(Bus * bus);
    virtual void memoryMap(MemoryOperation op, uint32_t full_adr, uint8_t *data);

private:
    Bus * bus;
    bool dataToProcess = true;

    uint8_t ia0;
    uint8_t ia1 = 0;
    uint8_t ia2;
    uint8_t ia3;

    uint8_t oa0 = 0;
    uint8_t oa1 = 0;
    uint8_t oa2 = 0;
    uint8_t oa3 = 0;
};

#endif // SNES_APU_H
