#ifndef W65816_H
#define W65816_H

#include "Common.h"
#include <functional>
#include <vector>
using std::vector;

class Bus;
#include "Types.h"

class W65816
{
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
	bool RDY();

	void triggerRDY(bool status);

	void triggerRESET();
	void triggerIRQ();
	void triggerNMI();

	//Getters - Registers
	uint16_t getPC();
	uint32_t getFullPC();
	uint16_t getAcc();
	uint8_t  getIR();
	uint8_t  getP();
	uint32_t getAddressBus();
	uint16_t getX();
	uint16_t getY();
	uint16_t getS();
	uint16_t getD();
	uint8_t  getDBR();

	string getPString();

	//Getters - Internal
	//unsigned int getTCycle();
	uint16_t getAdr();
	uint16_t getIDB();
	//Instruction & getInst();

	bool isBranchInstruction = false;   //used for debugging purposes (no effect in cpu emulation)
	uint32_t branchAddress = 0;         // "
	string getASM();

private:
	//Private Internal State
	Bus * bus;

	StageType lastPipelineStage;
	unsigned int tcycle = 0;

	bool vda = true;
	bool vpa = true;
	bool rdy = true;

	void IRQ();
	void NMI();
	void RESET();

	bool internalNMI = false;
	bool internalIRQ = false;
	bool internalRST = false;

	bool executeInterupt = false;

	void checkInterupts();

	uint8_t ZERO = 0;
	uint8_t SIGN_EXTENDED_OP_HALF_ADD = 0;

	//Private helper methods
	bool forceInternalOperation = false;
	bool forceTmpBank = false;
	bool thisIsABranch = false;
	bool branchTaken = false;
	bool clockStopped = false;

	
	uint32_t getParam(int index, int length = 1);

	enum ValidAddressState {OpcodeFetch, InternalOperation, DataFetch, OperandFetch};
	void handleValidAddressPINS(ValidAddressState state);

	void generateAddress(uint8_t bank, uint16_t adr);
	void generateAddress(uint16_t adr);

	void updateStatusFlags(uint32_t v, bool indexValue = false);
	void updateNZFlags(uint16_t v, bool indexValue = false, bool force16 = false);
	void checkSignedOverflow(int a, int b, int c);

	void shrinkIndexRegisters(bool doIt);

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
		void setI(bool status) { val = (val & ~(1<<2)) | (uint8_t(status)<<2); }
		void setD(bool status) { val = (val & ~(1<<3)) | (uint8_t(status)<<3); }
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

		void update() {setM(M()); setX(X()); cpu->shrinkIndexRegisters(X());}
		void setVal(uint8_t v) {val = v; update();}
		uint8_t getVal() {return val;}

		string toString() 
		{
			string status = "nvmxdizc"; //16bit mode by default
			for(int i = 0; i < 8; i++)
			{
				if(emulationMode)
				{
					if(i == 5) status[7-i] = '1';
					else if(i == 4) status[7-i] = B() ? 'B' : 'b';
				}
				if(((val>>i)&1) && ~(((i==5)||(i==4))&&emulationMode))
				{
					status[7-i] = toupper(status[7-i]);
				}
			}
			return status;
		}

		private: uint8_t val = 0x30;
		public: W65816* cpu;
	} p;

	void setReg(Register16 & r, uint16_t v); //TODO: How to differentiate between Index and ACC ???
	uint16_t getReg(Register16 & r);

	//Internal Operations
	void fetchInc(Register16 *src, uint8_t * dst);
	void fetchDec(Register16 *src, uint8_t * dst);
	void fetch(Register16 *src, uint8_t * dst);

	void fetchLong(uint8_t * bank, Register16 *src, uint8_t * dst);
	void fetchIncLong(uint8_t * bank, Register16 *src, uint8_t * dst);
	void fetchDecLong(uint8_t * bank, Register16 *src, uint8_t * dst);

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

	void enableInterupts(bool enable); //TODO: Signal or stage ? Note: It also disable the D flag when disabling interupts

	//Signals
	void incPC(unsigned int whatCycle = 1);
	void opPrefetchInIDB();
	void accPrefetchInIDB();
	void dhPrefetchInAdr();

	//Predecode Signals
	void invalidPrefetch();
	void branchInstruction();

	//New stuff
	void noAutoIncPC();
	bool prefetchIncPC = true;
	bool doPrefetchInIDB = false;
	bool isIndexRelated = false;
	
	void initializeAdrModeASMDecode();
	string opcodeASM;
	string asmLine;
	enum ASM_AdrModeType{   ASM_ABSOLUTE, ASM_ABSOLUTE_LONG, ASM_ABSOLUTE_X_LONG, ASM_ABSOLUTE_X, ASM_ABSOLUTE_Y, 
							ASM_ABSOLUTE_X_INDIRECT, ASM_ABSOLUTE_INDIRECT_JML,ASM_ABSOLUTE_INDIRECT_JMP,ASM_ACCUMULATOR,
							ASM_DIRECT, ASM_DIRECT_X_INDIRECT, ASM_DIRECT_INDIRECT, ASM_DIRECT_INDIRECT_Y, ASM_DIRECT_INDIRECT_Y_LONG,
							ASM_DIRECT_INDIRECT_LONG, ASM_DIRECT_X, ASM_DIRECT_Y, ASM_IMMEDIATE, ASM_IMMEDIATE_SPECIAL,
							ASM_IMPLIED, ASM_RELATIVE_BRANCH, ASM_RELATIVE_BRANCH_LONG};
	
	void processASM(ASM_AdrModeType type);
	
	
	enum EnablingCondition{
		SIG_ALWAYS,SIG_INST,SIG_DUMMY_STAGE,SIG_MEM16_ONLY,SIG_MODE16_ONLY,SIG_MODE8_ONLY,
		SIG_X_CROSS_PAGE,SIG_Y_CROSS_PAGE,SIG_DL_NOT_ZERO, SIG_INDIRECT_Y_CROSS_PAGE_OR_X16,
		SIG_PC_CROSS_PAGE_IN_EMUL, SIG_NATIVE_MODE, SIG_ACC_ZERO, SIG_ACC_NOT_ZERO, last};


	bool preDecodeStage = false;
	bool isStageEnabled(EnablingCondition st);
	bool isStageEnabled(unsigned int cycle, EnablingCondition signal);
	
	enum PipelineContent {REGULAR_INST, IRQ_INTERUPT, NMI_INTERUPT, RESET_INTERUPT};
	PipelineContent pipelineContent = REGULAR_INST;

	bool enablingSignals[EnablingCondition::last]; 
	vector<bool> enabledStages;
	int pipelineSize = 0;
	
	void decode(bool predecode = false);
	
	//Adressing Modes
	void Absolute                   (StageType&& inst);
	void AbsoluteWrite              (StageType&& inst);
	void AbsoluteRMW                (StageType&& inst);
	void AbsoluteJMP                (StageType&& inst);
	void AbsoluteJSR                (StageType&& inst);
	void AbsoluteLong               (StageType&& inst);
	void AbsoluteLongWrite          (StageType&& inst);
	void AbsoluteLongJMP            (StageType&& inst);
	void AbsoluteLongJSL            (StageType&& inst);
	void AbsoluteXLong              (StageType&& inst);
	void AbsoluteXLongWrite         (StageType&& inst);
	void AbsoluteX                  (StageType&& inst);
	void AbsoluteXWrite             (StageType&& inst);
	void AbsoluteXRMW               (StageType&& inst);
	void AbsoluteY                  (StageType&& inst);
	void AbsoluteYWrite             (StageType&& inst);
	void AbsoluteXIndirectJMP       (StageType&& inst);
	void AbsoluteXIndirectJSR       (StageType&& inst);
	void AbsoluteIndirectJML        (StageType&& inst);
	void AbsoluteIndirectJMP        (StageType&& inst);
	void Accumulator                (StageType&& inst);
	void Direct                     (StageType&& inst);
	void DirectWrite                (StageType&& inst);
	void DirectRMW                  (StageType&& inst);
	void DirectXIndirect            (StageType&& inst);
	void DirectXIndirectWrite       (StageType&& inst);
	void DirectIndirect             (StageType&& inst);
	void DirectIndirectWrite        (StageType&& inst);
	void DirectIndirectY            (StageType&& inst);
	void DirectIndirectYWrite       (StageType&& inst);
	void DirectIndirectYLong        (StageType&& inst);
	void DirectIndirectYLongWrite   (StageType&& inst);
	void DirectIndirectLong	        (StageType&& inst);
	void DirectIndirectLongWrite    (StageType&& inst);
	void DirectX                    (StageType&& inst);
	void DirectXWrite               (StageType&& inst);
	void DirectXRMW                 (StageType&& inst);
	void DirectY                    (StageType&& inst);
	void DirectYWrite               (StageType&& inst);
	void Immediate                  (StageType&& inst);
	void ImmediateSpecial           (StageType&& inst);
	void Implied                    (StageType&& inst);
	void ImpliedSpecial             (StageType&& inst);
	void RelativeBranch             (StageType&& inst);
	void RelativeBranchLong         (StageType&& inst);
	void StackPop                   (StageType&& inst);
	void StackPop8                  (StageType&& inst);
	void StackPop16                 (StageType&& inst);
	void StackPush                  (StageType&& inst);
	void StackPush8                 (StageType&& inst);
	void StackPush16                (StageType&& inst);
	void StackPEA                   (StageType&& inst);
	void StackPEI                   (StageType&& inst);
	void StackPER                   (StageType&& inst);
	void StackRTI                   (StageType&& inst);
	void StackRTS                   (StageType&& inst);
	void StackRTL                   (StageType&& inst);
	void StackRelative              (StageType&& inst);
	void StackRelativeWrite         (StageType&& inst);
	void StackRelativeIndirectY     (StageType&& inst);
	void StackRelativeIndirectYWrite(StageType&& inst);
	void StackInterupt              (StageType&& inst);
	void BlockMoveN                 (StageType&& inst);
	void BlockMoveP                 (StageType&& inst);
	
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
	void BRK();
	void BRL();
	void BVC();
	void BVS();
	void CLC();
	void CLD();
	void CLI();
	void CLV();
	void CMP();
	void COP();
	void CPX();
	void CPY();
	void DEC();
	void DEX();
	void DEY();
	void EOR();
	void INC();
	void INX();
	void INY();
	void JMP();
	void JML();
	void JSR();
	void JSL();
	void LDA();
	void LDX();
	void LDY();
	void LSR();
	void MVP();
	void MVN();
	void NOP();
	void ORA();
	void PEA();
	void PER();
	void PEI();
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
	void RTI();
	void RTL();
	void RTS();
	void SBC();
	void SEC();
	void SED();
	void SEI();
	void SEP();
	void STA();
	void STP();
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
	void WAI();
	void WDM();
	void XBA();
	void XCE();
};

#endif // W65816_H
