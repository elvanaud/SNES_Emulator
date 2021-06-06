#ifndef MEMORYINTERFACE_H
#define MEMORYINTERFACE_H

#include "Common.h"

class MemoryInterface
{
public:
    MemoryInterface();

    enum MemoryOperation{Write, Read};
    virtual void memoryMap(MemoryOperation op, uint32_t full_adr, uint8_t *data) = 0; //TODO: might change this prototype to instead return a pointer to the decoded adr
    void doMemoryOperation(MemoryOperation op, uint8_t * dataAdr, uint8_t * data);

    //TODO: maybe implement privateRead/write here?

private:
};

#endif // MEMORYINTERFACE_H
