#ifndef W65816_H
#define W65816_H

#include <cstdint>
#include <functional>
#include <vector>
using std::vector;


#include "Stage.h"
class Bus;
#include "Instruction.h"
#include "Types.h"
#include "AddressingMode.h"


class W65816
{
public:
    W65816();
    void attachBus(Bus * b);
    void tick();

    bool VDA();
    bool VPA();

    uint16_t getPC();
    uint16_t getAcc();
    uint16_t getAdr();
    uint16_t getIDB();
    uint8_t getIR();

    unsigned int getTCycle();

private:
    Bus * bus;

    vector<vector<StageType>> pipeline;
    unsigned int tcycle = 0;
    bool mem8 = true;
    bool index8 = true;

    Instruction decodingTable[0x100];

    struct Register16
    {
        uint8_t low = 0,high = 0;
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
    Register16 idb;
    Register16 acc;

    struct {
        uint8_t val;
        uint8_t C() { return (val>>0)&1; }
        uint8_t Z() { return (val>>1)&1; }
        uint8_t N() { return (val>>7)&1; }
        void setC(bool status) { val = (val & ~(1<<0)) | (uint8_t(status)<<0); }
        void setZ(bool status) { val = (val & ~(1<<1)) | (uint8_t(status)<<1); }
        void setN(bool status) { val = (val & ~(1<<7)) | (uint8_t(status)<<7); }
    } p;

    bool vda = true;
    bool vpa = true;

    void reloadPipeline();
    void processSignals();
    bool isStageEnabled(Stage &st);
    void initializeOpcodes();
    void initializeAddressingModes();

    void decode();

    void fetchInc(Register16 *src, uint8_t * dst);
    void fetchDec(Register16 *src, uint8_t * dst);
    void fetch(Register16 *src, uint8_t * dst);
    void dummyFetch(Register16 *src);
    void moveReg(uint8_t * src, uint8_t * dst);

    void instStage(); //Dummy

    void incPC();
    void opPrefetchInIDB();

    AddressingMode Immediate;

    void ADC();
};

#endif // W65816_H
