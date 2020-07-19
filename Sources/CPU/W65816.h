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
    friend class CPU_Debugger;
public:
    W65816();
    void attachBus(Bus * b);
    void tick();

    //Pins
    bool VDA();
    bool VPA();
    bool E();
    bool M();
    bool X();

    void RESET();

    //Getters - Registers
    uint16_t getPC();
    uint16_t getAcc();
    uint8_t getIR();
    uint8_t getP();
    uint32_t getAddressBus();

    //Getters - Internal
    unsigned int getTCycle();
    uint16_t getAdr();
    uint16_t getIDB();

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
    enum ValidAddressState {OpcodeFetch, InternalOperation, DataFetch, OperandFetch};
    void handleValidAddressPINS(ValidAddressState state);
    bool forceInternalOperation = false;
    void preDecode();

    //Registers
    struct Register16
    {
        Register16(bool idx = false) {isIndex = idx;}
        bool isIndex = false;
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
    uint32_t addressBusBuffer;

    Register16 idb;
    Register16 acc;
    Register16 x = Register16(true);
    Register16 y = Register16(true);

    uint8_t ir; //Instruction Register

    //Status Register
    struct {
        bool mem8 = true; //Should probably not be public
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
        uint8_t B() { if(emulationMode) return (val>>4)&1; return false;}
        uint8_t E() { return emulationMode;}
        void setM(bool status) { if(!emulationMode) val = (val & ~(1<<5)) | (uint8_t(status)<<5); mem8 = status;}
        void setX(bool status) { if(!emulationMode) val = (val & ~(1<<4)) | (uint8_t(status)<<4); index8 = status;}
        void setB(bool status) { if(emulationMode) val = (val & ~(1<<4)) | (uint8_t(status)<<4); }
        void setE(bool status) { emulationMode = status; /*if(emulationMode)*/ {setM(true); setX(true);}}

        void update() {setM(M()); setX(X());}
        void setVal(uint8_t v) {val = v; update();}
        uint8_t getVal() {return val;}

        private: uint8_t val = 0x30;
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

    void dummyStage(); //Dummy operation

    void updateStatusFlags(uint32_t v, bool indexValue = false);
    void updateNZFlags(uint16_t v, bool indexValue = false);
    void checkSignedOverflow(int a, int b, int c);

    //Signals
    void incPC(unsigned int whatCycle = 1);
    void opPrefetchInIDB();

    //Predecode Signals
    void invalidPrefetch();

    //Addressing Modes
    enum AdrModeName {IMMEDIATE,IMMEDIATE_SPECIAL,IMPLIED};
    AddressingMode Immediate = AddressingMode(AdrModeName::IMMEDIATE);
    AddressingMode ImmediateSpecial = AddressingMode(AdrModeName::IMMEDIATE_SPECIAL);
    AddressingMode Implied = AddressingMode(AdrModeName::IMPLIED);

    //Instructions
    void ADC();
    void AND();
    void BIT();
    void CLC();
    void CMP();
    void CPX();
    void CPY();
    void EOR();
    void LDA();
    void LDX();
    void LDY();
    void ORA();
    void REP();
    void SEC();
    void SEP();
    void XCE();
};

#endif // W65816_H
