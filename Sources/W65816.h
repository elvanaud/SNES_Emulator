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

    //Pins
    bool VDA();
    bool VPA();

    //Getters - Registers
    uint16_t getPC();
    uint16_t getAcc();
    uint16_t getAdr();
    uint16_t getIDB();
    uint8_t getIR();

    //Getters - Internal
    unsigned int getTCycle();

private:
    //Private Internal State
    Bus * bus;

    vector<vector<StageType>> pipeline;
    vector<StageType> lastPipelineStage;
    unsigned int tcycle = 0;

    bool vda = true;
    bool vpa = true;

    Instruction decodingTable[0x100];

    //Private helper methods
    void reloadPipeline();
    void processSignals();
    bool isStageEnabled(Stage const & st);
    void initializeOpcodes();
    void initializeAddressingModes();

    //Registers
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

    Register16 pc; //Could be uint16_t directly ?
    Register16 adr;

    Register16 idb;
    Register16 acc;
    Register16 x;
    Register16 y;

    uint8_t ir;

    //Status Register
    struct {
        uint8_t val; //private and use setter ? => Update mem8,index8,etc...
        bool mem8 = true;
        bool index8 = true;
        bool emulationMode = true;
        //Common
        uint8_t C() { return (val>>0)&1; }
        uint8_t Z() { return (val>>1)&1; }
        uint8_t I() { return (val>>2)&1; }
        uint8_t D() { return (val>>3)&1; }
        uint8_t V() { return (val>>6)&1; }
        uint8_t N() { return (val>>7)&1; }
        void setC(bool status) { val = (val & ~(1<<0)) | (uint8_t(status)<<0); }
        void setZ(bool status) { val = (val & ~(1<<1)) | (uint8_t(status)<<1); }
        void setD(bool status) { val = (val & ~(1<<2)) | (uint8_t(status)<<2); }
        void setI(bool status) { val = (val & ~(1<<3)) | (uint8_t(status)<<3); }
        void setV(bool status) { val = (val & ~(1<<6)) | (uint8_t(status)<<6); }
        void setN(bool status) { val = (val & ~(1<<7)) | (uint8_t(status)<<7); }
        //Depending on Mode
        uint8_t M() { if(!emulationMode) return (val>>5)&1; return mem8;}
        uint8_t X() { if(!emulationMode) return (val>>4)&1; return index8;}
        uint8_t B() { if(emulationMode) return (val>>4)&1; return false;} //throw ?
        uint8_t E() { return emulationMode;}
        void setM(bool status) { if(!emulationMode) val = (val & ~(1<<5)) | (uint8_t(status)<<5); mem8 = status;}
        void setX(bool status) { if(!emulationMode) val = (val & ~(1<<4)) | (uint8_t(status)<<4); index8 = status;}
        void setB(bool status) { if(emulationMode) val = (val & ~(1<<4)) | (uint8_t(status)<<4); }
        void setE(bool status) { emulationMode = status;}
    } p;

    void setReg(Register16 & r, uint16_t v); //TODO: How to differentiate between Index and ACC ???
    uint16_t getReg(Register16 & r);

    //Internal Operations
    void decode();

    void fetchInc(Register16 *src, uint8_t * dst);
    void fetchDec(Register16 *src, uint8_t * dst);
    void fetch(Register16 *src, uint8_t * dst);
    void dummyFetch(Register16 *src);
    void moveReg(uint8_t * src, uint8_t * dst);

    void instStage(); //Dummy operation

    void updateStatusFlags(uint32_t v);
    void updateNZFlags(uint16_t v);
    void checkSignedOverflow(int a, int b, int c);

    //Signals
    void incPC();
    void opPrefetchInIDB();

    //Addressing Modes
    AddressingMode Immediate;

    //Instructions
    void ADC();
    void AND();
};

#endif // W65816_H
