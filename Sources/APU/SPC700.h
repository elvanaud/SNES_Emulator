#ifndef SPC700_H
#define SPC700_H

#include "Common.h" //TODO: do smth about the useless BANK constants in this context
#include <functional>

class SNES_APU;
class SPC700;

using Inst = std::function<void(SPC700*)>&&;

class SPC700
{
public:
    SPC700();
    void attachBus(SNES_APU * bus);
    void tick();

    uint8_t read(uint16_t adr);
    void write(uint16_t adr, uint8_t data);

    void reset();
private:
    SNES_APU * apu_bus;

    uint16_t pc;
    uint8_t a;
    uint8_t x;
    uint8_t y;
    uint8_t sp;
    struct
    {
        uint8_t val;
        uint8_t C() { return (val>>0)&1; }
        uint8_t Z() { return (val>>1)&1; }
        uint8_t I() { return (val>>2)&1; }
        uint8_t H() { return (val>>3)&1; }
        uint8_t B() { return (val>>4)&1; }
        uint8_t P() { return (val>>5)&1; }
        uint8_t V() { return (val>>6)&1; }
        uint8_t N() { return (val>>7)&1; }
        void setC(bool status) { val = (val & ~(1<<0)) | (uint8_t(status)<<0); }
        void setZ(bool status) { val = (val & ~(1<<1)) | (uint8_t(status)<<1); }
        void setI(bool status) { val = (val & ~(1<<2)) | (uint8_t(status)<<2); }
        void setH(bool status) { val = (val & ~(1<<3)) | (uint8_t(status)<<3); }
        void setB(bool status) { val = (val & ~(1<<4)) | (uint8_t(status)<<4); }
        void setP(bool status) { val = (val & ~(1<<5)) | (uint8_t(status)<<5); }
        void setV(bool status) { val = (val & ~(1<<6)) | (uint8_t(status)<<6); }
        void setN(bool status) { val = (val & ~(1<<7)) | (uint8_t(status)<<7); }
    } psw; //status reg

    uint8_t  idb8;
    uint8_t  idb8_ext; //High part of idb8 when manipulating 16bit instructions

    bool branchTaken = false;

    unsigned int inst_length = 0;
    unsigned int inst_cycles = 0;

    unsigned int currentCycle = 0;

    bool halt_cpu = false;

    string asm_inst;
    string asm_adrMode;
    string asm_final;

    void updateNZflags(uint8_t a);
    void updateNZflags(uint8_t high, uint8_t a);

    void checkSignedOverflow(int a, int b, int c);
    uint8_t CarryFromBit(uint16_t a,uint16_t b,uint16_t res,uint8_t bit);
    uint8_t BorrowFromBit(uint16_t a,uint16_t b,uint16_t res,uint8_t bit);

    uint16_t directAddress(uint8_t adr);
    enum MemoryDirection{ReadMemory,WriteMemory};
    MemoryDirection memoryDirection = ReadMemory;
    void doMemOperation(MemoryDirection dir, uint8_t* left, uint8_t* right);

    void push(uint8_t data);
    uint8_t pop();

    uint16_t make16(uint8_t high, uint8_t low);

    bool cmpNoWrite = false;

    void DummyInst();

    //Addressing modes
    void Immediate(Inst);
    void Implied(Inst);
    void Direct(Inst);
    void DirectIndexedX(Inst);
    void Absolute(Inst);
    void AbsoluteIndexedX(Inst);
    void AbsoluteIndexedY(Inst);
    void DirectX(Inst);
    void DirectXInc(Inst);
    void DirectIndirectIndexed(Inst);
    void DirectIndexedIndirect(Inst);
    void DirectIndexedY(Inst);
    void Direct16(Inst);
    void DirectWrite(Inst);
    void DirectWriteImmediate(Inst);
    void DirectTransfer(Inst);
    void DirectIndexedXWrite(Inst);
    void DirectIndexedYWrite(Inst);
    void AbsoluteWrite(Inst);
    void AbsoluteIndexedXWrite(Inst);
    void AbsoluteIndexedYWrite(Inst);
    void DirectXIncWrite(Inst);
    void DirectXWrite(Inst);
    void DirectIndirectIndexedWrite(Inst);
    void DirectIndexedIndirectWrite(Inst);
    void Direct16Write(Inst);
    void Push(Inst);
    void Pop(Inst);
    void Direct_Direct(Inst);
    void Direct_Immediate(Inst);
    void DirectX_DirectY(Inst);
    void DirectRMW(Inst);
    void DirectIndexedXRMW(Inst);
    void AbsoluteRMW(Inst);
    void AccRMW(Inst);
    void XRMW(Inst);
    void YRMW(Inst);
    void Branch(Inst);
    void BranchTestMem(Inst);
    void BranchTestMemIndexedX(Inst);
    void Immediate16(Inst);
    void JumpAbsoluteIndexedX(Inst);
    void MemBit(Inst);
    void MemBitRMW(Inst);


    //Instructions
    void MOVA();
    void MOVX();
    void MOVY();
    void MOVAX();
    void MOVXA();
    void MOVAY();
    void MOVYA();
    void MOVXSP();
    void MOVSPX();
    void MOVW_YA();
    void MOV_PSW();
    void OR();
    void AND();
    void EOR();
    void CMP();
    void ADC();
    void SBC();
    void CMPX();
    void CMPY();
    void ASL();
    void ROL();
    void LSR();
    void ROR();
    void DEC();
    void INC();

    void CLR1();
    void SET1();

    void BPL();
    void BMI();
    void BVC();
    void BVS();
    void BCC();
    void BCS();
    void BNE();
    void BEQ();
    void BBS();
    void BBC();
    void CBNE();
    void DBNZ();
    void DBNZ_MEM();
    void BRA();
    void JMP();
    void CALL();
    void TCALL();
    void PCALL();
    void RET();
    void RETI();
    void BRK();
    void NOP();
    void CLRP();
    void SETP();
    void EI();
    void DI();
    void SLEEP();
    void HALT();
    void NOT1();
    void MOV1();
    void OR1();
    void OR1_NOT();
    void AND1();
    void AND1_NOT();
    void EOR1();
    void CLRC();
    void SETC();
    void NOTC();
    void CLRVH();
    void XCN();
    void TCLR1();
    void TSET1();
    void DAA();
    void DAS();
};

#endif // SPC700_H
