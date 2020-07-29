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

    uint8_t ZERO = 0;

    //Private helper methods
    bool forceInternalOperation = false;
    bool forceTmpBank = false;
    bool thisIsABranch = false;
    bool branchTaken = false;

    void reloadPipeline();
    void processSignals();
    bool isStageEnabled(Stage const & st);
    void initializeOpcodes();
    void initializeAddressingModes();
    enum ValidAddressState {OpcodeFetch, InternalOperation, DataFetch, OperandFetch};
    void handleValidAddressPINS(ValidAddressState state);
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
    void pushP();
    void popP();

    void halfAdd(uint8_t * dst, uint8_t * op); uint8_t internalCarryBuffer = 0;
    void fixCarry(uint8_t * dst, uint8_t * op);
    void fullAdd(Register16 * dst, Register16 * op);

    void dummyStage(); //Dummy operation

    void decReg(Register16 * reg);
    void incReg(Register16 * reg);

    void enableInterupts(bool enable); //TODO: Signal or stage ?

    //Signals
    void incPC(unsigned int whatCycle = 1);
    void opPrefetchInIDB();
    void accPrefetchInIDB();
    void dhPrefetchInAdr();

    //Predecode Signals
    void invalidPrefetch();
    void branchInstruction();

    //Addressing Modes
    enum AdrModeName {
        ABSOLUTE, ABSOLUTE_WRITE, ABSOLUTE_RMW, ABSOLUTE_JMP, ABSOLUTE_JSR, ABSOLUTE_LONG,
            ABSOLUTE_LONG_WRITE, ABSOLUTE_LONG_JMP, ABSOLUTE_LONG_JSL, ABSOLUTE_LONG_X, ABSOLUTE_X,
            ABSOLUTE_X_WRITE, ABSOLUTE_X_LONG_WRITE, ABSOLUTE_X_RMW, ABSOLUTE_Y, ABSOLUTE_Y_WRITE,
            ABSOLUTE_X_INDIRECT_JMP, ABSOLUTE_X_INDIRECT_JSR, ABSOLUTE_INDIRECT_JML, ABSOLUTE_INDIRECT_JMP,
        ACCUMULATOR,
        DIRECT, DIRECT_WRITE, DIRECT_RMW, DIRECT_X_INDIRECT, DIRECT_X_INDIRECT_WRITE, DIRECT_INDIRECT,
            DIRECT_INDIRECT_WRITE, DIRECT_INDIRECT_Y, DIRECT_INDIRECT_Y_WRITE, DIRECT_INDIRECT_Y_LONG,
            DIRECT_INDIRECT_Y_LONG_WRITE, DIRECT_INDIRECT_LONG, DIRECT_INDIRECT_LONG_WRITE,
            DIRECT_X, DIRECT_X_WRITE, DIRECT_X_RMW, DIRECT_Y, DIRECT_Y_WRITE,
        IMMEDIATE, IMMEDIATE_SPECIAL,
        IMPLIED, IMPLIED_SPECIAL,
        RELATIVE_BRANCH, RELATIVE_BRANCH_LONG,
        STACK_POP, STACK_POP_8, STACK_POP_16, STACK_PUSH, STACK_PUSH_8, STACK_PUSH_16, STACK_PEA,
            STACK_PEI, STACK_PER, STACK_RTI, STACK_RTS, STACK_RTL, STACK_RELATIVE, STACK_RELATIVE_WRITE,
            STACK_RELATIVE_INDIRECT_Y, STACK_RELATIVE_INDIRECT_Y_WRITE,
        BLOCK_MOVE_N, BLOCK_MOVE_P
        };

    AddressingMode Absolute                 = AddressingMode(AdrModeName::ABSOLUTE);
    AddressingMode AbsoluteWrite            = AddressingMode(AdrModeName::ABSOLUTE_WRITE);
    AddressingMode AbsoluteRMW              = AddressingMode(AdrModeName::ABSOLUTE_RMW);
    AddressingMode AbsoluteJMP              = AddressingMode(AdrModeName::ABSOLUTE_JMP);
    AddressingMode AbsoluteJSR              = AddressingMode(AdrModeName::ABSOLUTE_JSR);
    AddressingMode AbsoluteLong             = AddressingMode(AdrModeName::ABSOLUTE_LONG);
    AddressingMode AbsoluteLongWrite        = AddressingMode(AdrModeName::ABSOLUTE_LONG_WRITE);
    AddressingMode AbsoluteLongJMP          = AddressingMode(AdrModeName::ABSOLUTE_LONG_JMP);
    AddressingMode AbsoluteLongJSL          = AddressingMode(AdrModeName::ABSOLUTE_LONG_JSL);
    AddressingMode AbsoluteXLong            = AddressingMode(AdrModeName::ABSOLUTE_LONG_X);
    AddressingMode AbsoluteX                = AddressingMode(AdrModeName::ABSOLUTE_X);
    AddressingMode AbsoluteXWrite           = AddressingMode(AdrModeName::ABSOLUTE_X_WRITE);
    AddressingMode AbsoluteXLongWrite       = AddressingMode(AdrModeName::ABSOLUTE_X_LONG_WRITE);
    AddressingMode AbsoluteXRMW             = AddressingMode(AdrModeName::ABSOLUTE_X_RMW);
    AddressingMode AbsoluteY                = AddressingMode(AdrModeName::ABSOLUTE_Y);
    AddressingMode AbsoluteYWrite           = AddressingMode(AdrModeName::ABSOLUTE_Y_WRITE);
    AddressingMode AbsoluteXIndirectJMP     = AddressingMode(AdrModeName::ABSOLUTE_X_INDIRECT_JMP);
    AddressingMode AbsoluteXIndirectJSR     = AddressingMode(AdrModeName::ABSOLUTE_X_INDIRECT_JSR);
    AddressingMode AbsoluteIndirectJML      = AddressingMode(AdrModeName::ABSOLUTE_INDIRECT_JML);
    AddressingMode AbsoluteIndirectJMP      = AddressingMode(AdrModeName::ABSOLUTE_INDIRECT_JMP);
    AddressingMode Accumulator              = AddressingMode(AdrModeName::ACCUMULATOR);
    AddressingMode Direct                   = AddressingMode(AdrModeName::DIRECT);
    AddressingMode DirectWrite              = AddressingMode(AdrModeName::DIRECT_WRITE);
    AddressingMode DirectRMW                = AddressingMode(AdrModeName::DIRECT_RMW);
    AddressingMode DirectXIndirect          = AddressingMode(AdrModeName::DIRECT_X_INDIRECT);
    AddressingMode DirectXIndirectWrite     = AddressingMode(AdrModeName::DIRECT_X_INDIRECT_WRITE);
    AddressingMode DirectIndirect           = AddressingMode(AdrModeName::DIRECT_INDIRECT);
    AddressingMode DirectIndirectWrite      = AddressingMode(AdrModeName::DIRECT_INDIRECT_WRITE);
    AddressingMode DirectIndirectY          = AddressingMode(AdrModeName::DIRECT_INDIRECT_Y);
    AddressingMode DirectIndirectYWrite     = AddressingMode(AdrModeName::DIRECT_INDIRECT_Y_WRITE);
    AddressingMode DirectIndirectYLong      = AddressingMode(AdrModeName::DIRECT_INDIRECT_Y);
    AddressingMode DirectIndirectYLongWrite = AddressingMode(AdrModeName::DIRECT_INDIRECT_Y_WRITE);
    AddressingMode DirectIndirectLong       = AddressingMode(AdrModeName::DIRECT_INDIRECT_Y);
    AddressingMode DirectIndirectLongWrite  = AddressingMode(AdrModeName::DIRECT_INDIRECT_Y_WRITE);
    AddressingMode DirectX                  = AddressingMode(AdrModeName::DIRECT_X);
    AddressingMode DirectXWrite             = AddressingMode(AdrModeName::DIRECT_X_WRITE);
    AddressingMode DirectXRMW               = AddressingMode(AdrModeName::DIRECT_X_RMW);
    AddressingMode DirectY                  = AddressingMode(AdrModeName::DIRECT_Y);
    AddressingMode DirectYWrite             = AddressingMode(AdrModeName::DIRECT_Y_WRITE);
    AddressingMode Immediate                = AddressingMode(AdrModeName::IMMEDIATE);
    AddressingMode ImmediateSpecial         = AddressingMode(AdrModeName::IMMEDIATE_SPECIAL);
    AddressingMode Implied                  = AddressingMode(AdrModeName::IMPLIED);
    AddressingMode ImpliedSpecial           = AddressingMode(AdrModeName::IMPLIED_SPECIAL);
    AddressingMode RelativeBranch           = AddressingMode(AdrModeName::RELATIVE_BRANCH);
    AddressingMode RelativeBranchLong       = AddressingMode(AdrModeName::RELATIVE_BRANCH_LONG);
    AddressingMode StackPop                 = AddressingMode(AdrModeName::STACK_POP);
    AddressingMode StackPop8                = AddressingMode(AdrModeName::STACK_POP_8);
    AddressingMode StackPop16               = AddressingMode(AdrModeName::STACK_POP_16);
    AddressingMode StackPush                = AddressingMode(AdrModeName::STACK_PUSH);
    AddressingMode StackPush8               = AddressingMode(AdrModeName::STACK_PUSH_8);
    AddressingMode StackPush16              = AddressingMode(AdrModeName::STACK_PUSH_16);
    AddressingMode StackPEA                 = AddressingMode(AdrModeName::STACK_PEA);
    AddressingMode StackPEI                 = AddressingMode(AdrModeName::STACK_PEI);
    AddressingMode StackPER                 = AddressingMode(AdrModeName::STACK_PER);
    AddressingMode StackRTI                 = AddressingMode(AdrModeName::STACK_RTI);
    AddressingMode StackRTS                 = AddressingMode(AdrModeName::STACK_RTS);
    AddressingMode StackRTL                 = AddressingMode(AdrModeName::STACK_RTL);
    AddressingMode StackRelative            = AddressingMode(AdrModeName::STACK_RELATIVE);
    AddressingMode StackRelativeWrite       = AddressingMode(AdrModeName::STACK_RELATIVE_WRITE);
    AddressingMode StackRelativeIndirectY   = AddressingMode(AdrModeName::STACK_RELATIVE_INDIRECT_Y);
  AddressingMode StackRelativeIndirectYWrite= AddressingMode(AdrModeName::STACK_RELATIVE_INDIRECT_Y_WRITE);
    AddressingMode BlockMoveN               = AddressingMode(AdrModeName::BLOCK_MOVE_N);
    AddressingMode BlockMoveP               = AddressingMode(AdrModeName::BLOCK_MOVE_P);


    //Instructions
    void ADC();
    void AND();
    void ASL();
    void BCC();
    void BCS();
    void BEQ();
    void BIT();
    void BMI();
    void BNE();
    void BPL();
    void BRA();
    void BVC();
    void BVS();
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
    void PHA();
    void PHB();
    void PHP();
    void PHD();
    void PHK();
    void PHX();
    void PHY();
    void PLA();
    void PLB();
    void PLD();
    void PLP();
    void PLX();
    void PLY();
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
