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
    bool forceTmpBank = false;
    void preDecode();

    void generateAddress(uint8_t bank, uint16_t adr);
    void generateAddress(uint16_t adr);

    void updateStatusFlags(uint32_t v, bool indexValue = false);
    void updateNZFlags(uint16_t v, bool indexValue = false, bool force16 = false);
    void checkSignedOverflow(int a, int b, int c);

    //Registers
    struct Register16
    {
        Register16(bool idx = false) {isIndex = idx;}
        bool isIndex = false;
        uint8_t low = 0,high = 0;
        uint16_t val() { return (uint16_t(high) << 8) | low; } //TODO: high=0 when index8

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
    Register16 d;
    Register16 s;

    uint8_t dbr = 0;
    uint8_t pbr = 0;
    uint8_t tmpBank;

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
        void setD(bool status) { val = (val & ~(1<<3)) | (uint8_t(status)<<3); }
        void setI(bool status) { val = (val & ~(1<<2)) | (uint8_t(status)<<2); }
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

    void fetchLong(uint8_t * bank, Register16 *src, uint8_t * dst);
    void fetchIncLong(uint8_t * bank, Register16 *src, uint8_t * dst);

    void dummyFetch(Register16 *src);
    void dummyFetchLong(uint8_t *bank,Register16 *src);
    void dummyFetchLast();

    void moveReg8(uint8_t * src, uint8_t * dst);
    void moveReg16(Register16 * src, Register16 * dst);

    void write(Register16 * adr, uint8_t * data);
    void writeInc(Register16 * adr, uint8_t * data);
    void writeDec(Register16 * adr, uint8_t * data);

    void writeLong(uint8_t * bank, Register16 * adr, uint8_t * data);
    void writeIncLong(uint8_t * bank, Register16 * adr, uint8_t * data);
    void writeDecLong(uint8_t * bank, Register16 * adr, uint8_t * data);

    void push(uint8_t * src);
    void pop(uint8_t * dst);

    void halfAdd(uint8_t * dst, uint8_t * op); uint8_t internalCarryBuffer = 0;
    void fixCarry(uint8_t * dst, uint8_t * op);
    void fullAdd(Register16 * dst, Register16 * op);

    void dummyStage(); //Dummy operation

    void decReg(Register16 * reg);

    //Signals
    void incPC(unsigned int whatCycle = 1);
    void opPrefetchInIDB();
    void accPrefetchInIDB();

    //Predecode Signals
    void invalidPrefetch();

    //Addressing Modes
    enum AdrModeName {
        IMMEDIATE, IMMEDIATE_SPECIAL,
        IMPLIED, IMPLIED_SPECIAL,
        ABSOLUTE, ABSOLUTE_WRITE, ABSOLUTE_RMW, ABSOLUTE_JMP, ABSOLUTE_JSR, ABSOLUTE_LONG,
            ABSOLUTE_LONG_WRITE, ABSOLUTE_LONG_JMP, ABSOLUTE_LONG_JSL, ABSOLUTE_LONG_X, ABSOLUTE_X,
            ABSOLUTE_X_WRITE, ABSOLUTE_X_LONG_WRITE, ABSOLUTE_X_RMW, ABSOLUTE_Y, ABSOLUTE_Y_WRITE,
            ABSOLUTE_X_INDIRECT_JMP,ABSOLUTE_X_INDIRECT_JSR,
        ACCUMULATOR};

    AddressingMode Immediate            = AddressingMode(AdrModeName::IMMEDIATE);
    AddressingMode ImmediateSpecial     = AddressingMode(AdrModeName::IMMEDIATE_SPECIAL);
    AddressingMode Implied              = AddressingMode(AdrModeName::IMPLIED);
    AddressingMode ImpliedSpecial       = AddressingMode(AdrModeName::IMPLIED_SPECIAL);
    AddressingMode Absolute             = AddressingMode(AdrModeName::ABSOLUTE);
    AddressingMode AbsoluteWrite        = AddressingMode(AdrModeName::ABSOLUTE_WRITE);
    AddressingMode AbsoluteRMW          = AddressingMode(AdrModeName::ABSOLUTE_RMW);
    AddressingMode AbsoluteJMP          = AddressingMode(AdrModeName::ABSOLUTE_JMP);
    AddressingMode AbsoluteJSR          = AddressingMode(AdrModeName::ABSOLUTE_JSR);
    AddressingMode AbsoluteLong         = AddressingMode(AdrModeName::ABSOLUTE_LONG);
    AddressingMode AbsoluteLongWrite    = AddressingMode(AdrModeName::ABSOLUTE_LONG_WRITE);
    AddressingMode AbsoluteLongJMP      = AddressingMode(AdrModeName::ABSOLUTE_LONG_JMP);
    AddressingMode AbsoluteLongJSL      = AddressingMode(AdrModeName::ABSOLUTE_LONG_JSL);
    AddressingMode AbsoluteXLong        = AddressingMode(AdrModeName::ABSOLUTE_LONG_X);
    AddressingMode AbsoluteX            = AddressingMode(AdrModeName::ABSOLUTE_X);
    AddressingMode AbsoluteXWrite       = AddressingMode(AdrModeName::ABSOLUTE_X_WRITE);
    AddressingMode AbsoluteXLongWrite   = AddressingMode(AdrModeName::ABSOLUTE_X_LONG_WRITE);
    AddressingMode AbsoluteXRMW         = AddressingMode(AdrModeName::ABSOLUTE_X_RMW);
    AddressingMode AbsoluteY            = AddressingMode(AdrModeName::ABSOLUTE_Y);
    AddressingMode AbsoluteYWrite       = AddressingMode(AdrModeName::ABSOLUTE_Y_WRITE);
    AddressingMode AbsoluteXIndirectJMP = AddressingMode(AdrModeName::ABSOLUTE_X_INDIRECT_JMP);
    AddressingMode AbsoluteXIndirectJSR = AddressingMode(AdrModeName::ABSOLUTE_X_INDIRECT_JSR);
    AddressingMode Accumulator          = AddressingMode(AdrModeName::ACCUMULATOR);


    //Instructions
    void ADC();
    void AND();
    void ASL();
    void BIT();
    void CLC();
    void CLD();
    void CLI();
    void CLV();
    void CMP();
    void CPX();
    void CPY();
    void DEC();
    void DEX();
    void DEY();
    void EOR();
    void INC();
    void INX();
    void INY();
    void LDA();
    void LDX();
    void LDY();
    void LSR();
    void ORA();
    void REP();
    void ROL();
    void ROR();
    void SBC();
    void SEC();
    void SED();
    void SEI();
    void SEP();
    void STA();
    void STX();
    void STY();
    void STZ();
    void TAX();
    void TAY();
    void TCD();
    void TCS();
    void TDC();
    void TRB();
    void TSB();
    void TSC();
    void TSX();
    void TXA();
    void TXS();
    void TXY();
    void TYA();
    void TYX();
    void XBA();
    void XCE();
};

#endif // W65816_H
