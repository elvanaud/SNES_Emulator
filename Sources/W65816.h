#ifndef W65816_H
#define W65816_H

#include <cstdint>
#include <functional>
#include <vector>
using std::vector;


#include "Stage.h"
class Bus;


class W65816
{
public:
    using StageType = vector<std::function<void(W65816*)>>;

    W65816();
    void attachBus(Bus * b);
    void tick();

    bool VDA();
    bool VPA();
private:
    Bus * bus;

    vector<StageType> pipeline;
    unsigned int tcycle = 0;

    struct Register16
    {
        uint8_t low,high;
        uint16_t val() { return (uint16_t(high) << 8) | low; }

        void set(uint16_t v)
        {
            high = (v>>8)&0xFF;
            low = v & 0xFF;
        }

        uint16_t operator++()
        {
            uint16_t v = val()+1;
            set(v);
            return v;
        }

        uint16_t operator--()
        {
            uint16_t v = val()-1;
            set(v);
            return v;
        }
    };

    Register16 pc;
    uint8_t ir;
    Register16 adr;

    bool vda = true;
    bool vpa = true;

    void reloadPipeline();

    void decode();

    void fetchInc(Register16 *src, uint8_t * dst);
    void fetchDec(Register16 *src, uint8_t * dst);
    void fetch(Register16 *src, uint8_t * dst);
    void dummyFetch(Register16 *src);
    void moveReg(uint8_t * src, uint8_t * dst);
};

#endif // W65816_H
