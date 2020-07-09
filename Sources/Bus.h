#ifndef Bus_H
#define Bus_H

#include <cstdint>


class W65816;

const unsigned int RAM_QUANTITY = 0x1'00'00; //TODO

class Bus
{
public:
    Bus(W65816 &c);

    void run();

    void read(uint32_t adr);
    uint8_t DMR();
private:
    W65816 &cpu;

    uint8_t dmr = 0;
    uint8_t ram[RAM_QUANTITY];
};

#endif //Bus_H