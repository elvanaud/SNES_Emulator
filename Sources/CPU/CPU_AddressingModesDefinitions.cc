#include "W65816.h"

void W65816::Absolute(StageType&& inst)
{
	if(preDecodeStage)
	{
		lastPipelineStage = inst;
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		fetchInc(&pc,&adr.high);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		fetchInc(&adr,&idb.low);
	}
	if(isStageEnabled(2,SIG_MODE16_ONLY))
	{
		fetch(&adr,&idb.high);
	}
}

void W65816::AbsoluteWrite(StageType&& inst)
{
	if(preDecodeStage)
	{
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		fetchInc(&pc,&adr.high);
		inst(this);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		writeInc(&adr,&idb.low);
	}
	if(isStageEnabled(2,SIG_MODE16_ONLY))
	{
		write(&adr,&idb.high);
	}
}

void W65816::AbsoluteRMW(StageType&& inst)
{
	if(preDecodeStage)
	{
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		fetchInc(&pc,&adr.high);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		fetchInc(&adr,&idb.low);
	}
	if(isStageEnabled(2,SIG_MODE16_ONLY))
	{
		fetch(&adr,&idb.high);
	}
	if(isStageEnabled(3,SIG_ALWAYS))
	{
		dummyFetchLast();
		inst(this);
		if(enablingSignals[SIG_MODE8_ONLY])
			decReg(&adr);
	}
	if(isStageEnabled(4,SIG_ALWAYS))
	{
		if(enablingSignals[SIG_MODE16_ONLY])
			writeDec(&adr,&idb.high);
		write(&adr,&idb.low);
	}
}

void W65816::AbsoluteJMP(StageType&& inst)
{
	if(preDecodeStage)
	{
		lastPipelineStage = inst;
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		fetch(&pc,&pc.high);
		moveReg8(&adr.low,&pc.low);
	}
}

void W65816::AbsoluteJSR(StageType&& inst)
{
	if(preDecodeStage)
	{
		lastPipelineStage = inst;
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		fetch(&pc,&adr.high);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		dummyFetchLast();
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		push(&pc.high);
		moveReg8(&adr.high,&pc.high);
	}
	if(isStageEnabled(3,SIG_ALWAYS))
	{
		push(&pc.low);
		moveReg8(&adr.low,&pc.low);
	}
}

void W65816::AbsoluteLong(StageType&& inst)
{
	if(preDecodeStage)
	{
		lastPipelineStage = inst;
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		fetchInc(&pc,&adr.high);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		fetchInc(&pc,&tmpBank);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetchIncLong(&tmpBank,&adr,&idb.low);
	}
	if(isStageEnabled(3,SIG_MODE16_ONLY))
	{
		fetchLong(&tmpBank,&adr,&idb.high);
	}
}

void W65816::AbsoluteLongWrite(StageType&& inst)
{
	if(preDecodeStage)
	{
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		fetchInc(&pc,&adr.high);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		fetchInc(&pc,&tmpBank);
		inst(this);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		writeIncLong(&tmpBank,&adr,&idb.low);
	}
	if(isStageEnabled(3,SIG_MODE16_ONLY))
	{
		writeLong(&tmpBank,&adr,&idb.high);
	}
}

void W65816::AbsoluteLongJMP(StageType&& inst)
{
	if(preDecodeStage)
	{
		lastPipelineStage = inst;
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		fetchInc(&pc,&adr.high);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		fetchInc(&pc,&pbr);
		moveReg16(&adr,&pc);
	}
}

void W65816::AbsoluteLongJSL(StageType&& inst)
{
	if(preDecodeStage)
	{
		lastPipelineStage = inst;
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		fetchInc(&pc,&adr.high);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		push(&pbr);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		dummyFetchLast();
	}
	if(isStageEnabled(3,SIG_ALWAYS))
	{
		fetch(&pc,&pbr);
	}
	if(isStageEnabled(4,SIG_ALWAYS))
	{
		push(&pc.high);
	}
	if(isStageEnabled(5,SIG_ALWAYS))
	{
		push(&pc.low);
		moveReg16(&adr,&pc);
	}
}

void W65816::AbsoluteXLong(StageType&& inst)
{
	if(preDecodeStage)
	{
		lastPipelineStage = inst;
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		fetchInc(&pc,&adr.high);
		halfAdd(&adr.low,&x.low);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		fetchInc(&pc,&tmpBank);
		if(enablingSignals[SIG_X_CROSS_PAGE])
			fixCarry(&adr.high,&x.high);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetchIncLong(&tmpBank,&adr,&idb.low);
	}
	if(isStageEnabled(3,SIG_MODE16_ONLY))
	{
		fetchLong(&tmpBank,&adr,&idb.high);
	}
}

void W65816::AbsoluteXLongWrite(StageType&& inst)
{
	if(preDecodeStage)
	{
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		fetchInc(&pc,&adr.high);
		halfAdd(&adr.low,&x.low);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		fetchInc(&pc,&tmpBank);
		if(enablingSignals[SIG_X_CROSS_PAGE])
			fixCarry(&adr.high,&x.high);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		inst(this);
		writeIncLong(&tmpBank,&adr,&idb.low);
	}
	if(isStageEnabled(3,SIG_MODE16_ONLY))
	{
		writeLong(&tmpBank,&adr,&idb.high);
	}
}

void W65816::AbsoluteX(StageType&& inst)
{
	if(preDecodeStage)
	{
		lastPipelineStage = inst;
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		fetchInc(&pc,&adr.high);
		halfAdd(&adr.low,&x.low);
	}
	if(isStageEnabled(1,SIG_X_CROSS_PAGE))
	{
		dummyFetchLong(&dbr,&adr);
		fixCarry(&adr.high,&x.high);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetchInc(&adr,&idb.low);
	}
	if(isStageEnabled(3,SIG_MODE16_ONLY))
	{
		fetch(&adr,&idb.high);
	}
}

void W65816::AbsoluteXWrite(StageType&& inst)
{
	if(preDecodeStage)
	{
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		fetchInc(&pc,&adr.high);
		halfAdd(&adr.low,&x.low);
	}
	if(isStageEnabled(1,SIG_X_CROSS_PAGE))
	{
		dummyFetchLong(&dbr,&adr);
		fixCarry(&adr.high,&x.high);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		inst(this);
		writeInc(&adr,&idb.low);
	}
	if(isStageEnabled(3,SIG_MODE16_ONLY))
	{
		write(&adr,&idb.high);
	}
}

void W65816::AbsoluteXRMW(StageType&& inst)
{
	if(preDecodeStage)
	{
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		fetchInc(&pc,&adr.high);
		halfAdd(&adr.low,&x.low);
	}
	if(isStageEnabled(1,SIG_X_CROSS_PAGE))
	{
		dummyFetchLong(&dbr,&adr);
		fixCarry(&adr.high,&x.high);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetchInc(&adr,&idb.low);
	}
	if(isStageEnabled(3,SIG_MODE16_ONLY))
	{
		fetch(&adr,&idb.high);
	}
	if(isStageEnabled(4,SIG_ALWAYS))
	{
		dummyFetchLast();
		inst(this);
		if(enablingSignals[SIG_MODE8_ONLY])
			decReg(&adr);
	}
	if(isStageEnabled(5,SIG_MODE16_ONLY))
	{
		writeDec(&adr,&idb.high);
	}
	if(isStageEnabled(6,SIG_ALWAYS))
	{
		write(&adr,&idb.low);
	}
}

void W65816::AbsoluteY(StageType&& inst)
{
	if(preDecodeStage)
	{
		lastPipelineStage = inst;
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		fetchInc(&pc,&adr.high);
		halfAdd(&adr.low,&y.low);
	}
	if(isStageEnabled(1,SIG_Y_CROSS_PAGE))
	{
		dummyFetchLong(&dbr,&adr);
		fixCarry(&adr.high,&y.high);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetchInc(&adr,&idb.low);
	}
	if(isStageEnabled(3,SIG_MODE16_ONLY))
	{
		fetch(&adr,&idb.high);
	}
}

void W65816::AbsoluteYWrite(StageType&& inst)
{
	if(preDecodeStage)
	{
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		fetchInc(&pc,&adr.high);
		halfAdd(&adr.low,&y.low);
	}
	if(isStageEnabled(1,SIG_Y_CROSS_PAGE))
	{
		dummyFetchLong(&dbr,&adr);
		fixCarry(&adr.high,&y.high);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		inst(this);
		writeInc(&adr,&idb.low);
	}
	if(isStageEnabled(3,SIG_MODE16_ONLY))
	{
		write(&adr,&idb.high);
	}
}

void W65816::AbsoluteXIndirectJMP(StageType&& inst)
{
	if(preDecodeStage)
	{
		lastPipelineStage = inst;
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		fetchInc(&pc,&adr.high);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		dummyFetchLast();
		fullAdd(&adr,&x);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetchIncLong(&pbr,&adr,&pc.low);
	}
	if(isStageEnabled(3,SIG_ALWAYS))
	{
		fetchLong(&pbr,&adr,&pc.high);
	}
}

void W65816::AbsoluteXIndirectJSR(StageType&& inst)
{
	if(preDecodeStage)
	{
		lastPipelineStage = inst;
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		push(&pc.high);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		push(&pc.low);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetch(&pc,&adr.high);
	}
	if(isStageEnabled(3,SIG_ALWAYS))
	{
		dummyFetchLast();
		fullAdd(&adr,&x);
	}
	if(isStageEnabled(4,SIG_ALWAYS))
	{
		fetchIncLong(&pbr,&adr,&pc.low);
	}
	if(isStageEnabled(5,SIG_ALWAYS))
	{
		fetchLong(&pbr,&adr,&pc.high);
	}
}

void W65816::AbsoluteIndirectJML(StageType&& inst)
{
	if(preDecodeStage)
	{
		lastPipelineStage = inst;
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		fetchInc(&pc,&adr.high);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		fetchIncLong(&ZERO,&adr,&pc.low);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetchIncLong(&ZERO,&adr,&pc.high);
	}
	if(isStageEnabled(3,SIG_ALWAYS))
	{
		fetchLong(&ZERO,&adr,&pbr);
	}
}

void W65816::AbsoluteIndirectJMP(StageType&& inst)
{
	if(preDecodeStage)
	{
		lastPipelineStage = inst;
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		fetchInc(&pc,&adr.high);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		fetchIncLong(&ZERO,&adr,&pc.low);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetchLong(&ZERO,&adr,&pc.high);
	}
}

void W65816::Accumulator(StageType&& inst)
{
	if(preDecodeStage)
	{
		invalidPrefetch();
		accPrefetchInIDB();
		noAutoIncPC();
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		inst(this);
		moveReg8(&idb.low,&acc.low);
		if(enablingSignals[SIG_MODE16_ONLY])
			moveReg8(&idb.high,&acc.high);
	}
}

void W65816::Direct(StageType&& inst)
{
	if(preDecodeStage)
	{
		dhPrefetchInAdr();
		lastPipelineStage = inst;
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_DL_NOT_ZERO))
	{
		dummyFetchLast();
		halfAdd(&adr.low,&d.low);
		fixCarry(&adr.high,&ZERO);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		fetchIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(2,SIG_MODE16_ONLY))
	{
		fetchLong(&ZERO,&adr,&idb.high);
	}
}

void W65816::DirectWrite(StageType&& inst)
{
	if(preDecodeStage)
	{
		dhPrefetchInAdr();
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_DL_NOT_ZERO))
	{
		dummyFetchLast();
		halfAdd(&adr.low,&d.low);
		fixCarry(&adr.high,&ZERO);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		inst(this);
		writeIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(2,SIG_MODE16_ONLY))
	{
		writeLong(&ZERO,&adr,&idb.high);
	}
}	

void W65816::DirectRMW(StageType&& inst)
{
	if(preDecodeStage)
	{
		dhPrefetchInAdr();
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_DL_NOT_ZERO))
	{
		dummyFetchLast();
		halfAdd(&adr.low,&d.low);
		fixCarry(&adr.high,&ZERO);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		fetchIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(2,SIG_MODE16_ONLY))
	{
		fetchLong(&ZERO,&adr,&idb.high);
	}
	if(isStageEnabled(3,SIG_ALWAYS))
	{
		dummyFetchLast();
		inst(this);
		if(enablingSignals[SIG_MODE8_ONLY])
			decReg(&adr);
	}
	if(isStageEnabled(4,SIG_MODE16_ONLY))
	{
		writeDecLong(&ZERO,&adr,&idb.high);
	}
	if(isStageEnabled(5,SIG_ALWAYS))
	{
		writeLong(&ZERO,&adr,&idb.low);
	}
}	

void W65816::DirectXIndirect(StageType&& inst)
{
	if(preDecodeStage)
	{
		dhPrefetchInAdr();
		lastPipelineStage = inst;
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_DL_NOT_ZERO))
	{
		dummyFetchLast();
		halfAdd(&adr.low,&d.low);
		fixCarry(&adr.high,&ZERO);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		dummyFetchLast();
		fullAdd(&adr,&x);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetchIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(3,SIG_ALWAYS))
	{
		fetchLong(&ZERO,&adr,&adr.high);
		moveReg8(&idb.low,&adr.low);
	}
	if(isStageEnabled(4,SIG_ALWAYS))
	{
		fetchIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(5,SIG_MODE16_ONLY))
	{
		fetchLong(&ZERO,&adr,&idb.high);
	}
}

void W65816::DirectXIndirectWrite(StageType&& inst)
{
	if(preDecodeStage)
	{
		dhPrefetchInAdr();
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_DL_NOT_ZERO))
	{
		dummyFetchLast();
		halfAdd(&adr.low,&d.low);
		fixCarry(&adr.high,&ZERO);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		dummyFetchLast();
		fullAdd(&adr,&x);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetchIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(3,SIG_ALWAYS))
	{
		fetchLong(&ZERO,&adr,&adr.high);
		moveReg8(&idb.low,&adr.low);
	}
	if(isStageEnabled(4,SIG_ALWAYS))
	{
		inst(this);
		writeIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(5,SIG_MODE16_ONLY))
	{
		writeLong(&ZERO,&adr,&idb.high);
	}
}

void W65816::DirectIndirect(StageType&& inst)
{
	if(preDecodeStage)
	{
		dhPrefetchInAdr();
		lastPipelineStage = inst;
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_DL_NOT_ZERO))
	{
		dummyFetchLast();
		halfAdd(&adr.low,&d.low);
		fixCarry(&adr.high,&ZERO);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		fetchIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetchLong(&ZERO,&adr,&adr.high);
		moveReg8(&idb.low,&adr.low);
	}
	if(isStageEnabled(3,SIG_ALWAYS))
	{
		fetchIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(4,SIG_MODE16_ONLY))
	{
		fetchLong(&ZERO,&adr,&idb.high);
	}
}

void W65816::DirectIndirectWrite(StageType&& inst)
{
	if(preDecodeStage)
	{
		dhPrefetchInAdr();
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_DL_NOT_ZERO))
	{
		dummyFetchLast();
		halfAdd(&adr.low,&d.low);
		fixCarry(&adr.high,&ZERO);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		fetchIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetchLong(&ZERO,&adr,&adr.high);
		moveReg8(&idb.low,&adr.low);
	}
	if(isStageEnabled(3,SIG_ALWAYS))
	{
		inst(this);
		writeIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(4,SIG_MODE16_ONLY))
	{
		writeLong(&ZERO,&adr,&idb.high);
	}
}

void W65816::DirectIndirectY(StageType&& inst)
{
	if(preDecodeStage)
	{
		dhPrefetchInAdr();
		lastPipelineStage = inst;
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_DL_NOT_ZERO))
	{
		dummyFetchLast();
		halfAdd(&adr.low,&d.low);
		fixCarry(&adr.high,&ZERO);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		fetchIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetchLong(&ZERO,&adr,&adr.high);
		moveReg8(&idb.low,&adr.low);
		halfAdd(&adr.low,&y.low);
	}
	if(isStageEnabled(3,SIG_INDIRECT_Y_CROSS_PAGE_OR_X16))
	{
		dummyFetch(&adr);
		fixCarry(&adr.high,&y.high);
	}
	if(isStageEnabled(4,SIG_ALWAYS))
	{
		fetchInc(&adr,&idb.low);
	}
	if(isStageEnabled(5,SIG_MODE16_ONLY))
	{
		fetch(&adr,&idb.high);
	}
}

void W65816::DirectIndirectYWrite(StageType&& inst)
{
	if(preDecodeStage)
	{
		dhPrefetchInAdr();
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_DL_NOT_ZERO))
	{
		dummyFetchLast();
		halfAdd(&adr.low,&d.low);
		fixCarry(&adr.high,&ZERO);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		fetchIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetchLong(&ZERO,&adr,&adr.high);
		moveReg8(&idb.low,&adr.low);
		halfAdd(&adr.low,&y.low);
	}
	if(isStageEnabled(3,SIG_INDIRECT_Y_CROSS_PAGE_OR_X16))
	{
		dummyFetch(&adr);
		fixCarry(&adr.high,&y.high);
	}
	if(isStageEnabled(4,SIG_ALWAYS))
	{
		inst(this);
		writeInc(&adr,&idb.low);
	}
	if(isStageEnabled(5,SIG_MODE16_ONLY))
	{
		write(&adr,&idb.high);
	}
}

void W65816::DirectIndirectYLong(StageType&& inst)
{
	if(preDecodeStage)
	{
		dhPrefetchInAdr();
		lastPipelineStage = inst;
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_DL_NOT_ZERO))
	{
		dummyFetchLast();
		halfAdd(&adr.low,&d.low);
		fixCarry(&adr.high,&ZERO);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		fetchIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetchIncLong(&ZERO,&adr,&idb.high);
		halfAdd(&idb.low,&y.low);
	}
	if(isStageEnabled(3,SIG_ALWAYS))
	{
		fetchLong(&ZERO,&adr,&tmpBank);
		if(enablingSignals[SIG_INDIRECT_Y_CROSS_PAGE_OR_X16])
			fixCarry(&idb.high,&y.high);
	}
	if(isStageEnabled(4,SIG_ALWAYS))
	{
		moveReg16(&idb,&adr);
		fetchIncLong(&tmpBank,&adr,&idb.low);
	}
	if(isStageEnabled(5,SIG_MODE16_ONLY))
	{
		fetchLong(&tmpBank,&adr,&idb.high);
	}
}

void W65816::DirectIndirectYLongWrite(StageType&& inst)
{
	if(preDecodeStage)
	{
		dhPrefetchInAdr();
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_DL_NOT_ZERO))
	{
		dummyFetchLast();
		halfAdd(&adr.low,&d.low);
		fixCarry(&adr.high,&ZERO);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		fetchIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetchIncLong(&ZERO,&adr,&idb.high);
		halfAdd(&idb.low,&y.low);
	}
	if(isStageEnabled(3,SIG_ALWAYS))
	{
		fetchLong(&ZERO,&adr,&tmpBank);
		if(enablingSignals[SIG_INDIRECT_Y_CROSS_PAGE_OR_X16])
			fixCarry(&idb.high,&y.high);
	}
	if(isStageEnabled(4,SIG_ALWAYS))
	{
		moveReg16(&idb,&adr);
		inst(this);
		writeIncLong(&tmpBank,&adr,&idb.low);
	}
	if(isStageEnabled(5,SIG_MODE16_ONLY))
	{
		writeLong(&tmpBank,&adr,&idb.high);
	}
}

void W65816::DirectIndirectLong(StageType&& inst)
{
	if(preDecodeStage)
	{
		dhPrefetchInAdr();
		lastPipelineStage = inst;
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_DL_NOT_ZERO))
	{
		dummyFetchLast();
		halfAdd(&adr.low,&d.low);
		fixCarry(&adr.high,&ZERO);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		fetchIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetchIncLong(&ZERO,&adr,&idb.high);
	}
	if(isStageEnabled(3,SIG_ALWAYS))
	{
		fetchLong(&ZERO,&adr,&tmpBank);
		moveReg16(&idb,&adr);
	}
	if(isStageEnabled(4,SIG_ALWAYS))
	{
		fetchIncLong(&tmpBank,&adr,&idb.low);
	}
	if(isStageEnabled(5,SIG_MODE16_ONLY))
	{
		fetchLong(&tmpBank,&adr,&idb.high);
	}
}

void W65816::DirectIndirectLongWrite(StageType&& inst)
{
	if(preDecodeStage)
	{
		dhPrefetchInAdr();
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_DL_NOT_ZERO))
	{
		dummyFetchLast();
		halfAdd(&adr.low,&d.low);
		fixCarry(&adr.high,&ZERO);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		fetchIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetchIncLong(&ZERO,&adr,&idb.high);
	}
	if(isStageEnabled(3,SIG_ALWAYS))
	{
		fetchLong(&ZERO,&adr,&tmpBank);
		moveReg16(&idb,&adr);
	}
	if(isStageEnabled(4,SIG_ALWAYS))
	{
		inst(this);
		writeIncLong(&tmpBank,&adr,&idb.low);
	}
	if(isStageEnabled(5,SIG_MODE16_ONLY))
	{
		writeLong(&tmpBank,&adr,&idb.high);
	}
}

void W65816::DirectX(StageType&& inst)
{
	if(preDecodeStage)
	{
		dhPrefetchInAdr();
		lastPipelineStage = inst;
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_DL_NOT_ZERO))
	{
		dummyFetchLast();
		halfAdd(&adr.low,&d.low);
		fixCarry(&adr.high,&ZERO);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		dummyFetchLast();
		fullAdd(&adr,&x);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetchIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(3,SIG_MODE16_ONLY))
	{
		fetchLong(&ZERO,&adr,&idb.high);
	}
}

void W65816::DirectXWrite(StageType&& inst)
{
	if(preDecodeStage)
	{
		dhPrefetchInAdr();
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_DL_NOT_ZERO))
	{
		dummyFetchLast();
		halfAdd(&adr.low,&d.low);
		fixCarry(&adr.high,&ZERO);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		dummyFetchLast();
		fullAdd(&adr,&x);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		inst(this);
		writeIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(3,SIG_MODE16_ONLY))
	{
		writeLong(&ZERO,&adr,&idb.high);
	}
}

void W65816::DirectXRMW(StageType&& inst)
{
	if(preDecodeStage)
	{
		dhPrefetchInAdr();
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_DL_NOT_ZERO))
	{
		dummyFetchLast();
		halfAdd(&adr.low,&d.low);
		fixCarry(&adr.high,&ZERO);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		dummyFetchLast();
		fullAdd(&adr,&x);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetchIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(3,SIG_MODE16_ONLY))
	{
		fetchLong(&ZERO,&adr,&idb.high);
	}
	if(isStageEnabled(4,SIG_ALWAYS))
	{
		dummyFetchLast();
		inst(this);
		if(enablingSignals[SIG_MODE8_ONLY])
			decReg(&adr);
	}
	if(isStageEnabled(5,SIG_MODE16_ONLY))
	{
		writeDecLong(&ZERO,&adr,&idb.high);
	}
	if(isStageEnabled(6,SIG_ALWAYS))
	{
		writeLong(&ZERO,&adr,&idb.low);
	}
}

void W65816::DirectY(StageType&& inst)
{
	if(preDecodeStage)
	{
		dhPrefetchInAdr();
		lastPipelineStage = inst;
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_DL_NOT_ZERO))
	{
		dummyFetchLast();
		halfAdd(&adr.low,&d.low);
		fixCarry(&adr.high,&ZERO);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		dummyFetchLast();
		fullAdd(&adr,&y);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetchIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(3,SIG_MODE16_ONLY))
	{
		fetchLong(&ZERO,&adr,&idb.high);
	}
}

void W65816::DirectYWrite(StageType&& inst)
{
	if(preDecodeStage)
	{
		dhPrefetchInAdr();
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_DL_NOT_ZERO))
	{
		dummyFetchLast();
		halfAdd(&adr.low,&d.low);
		fixCarry(&adr.high,&ZERO);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		dummyFetchLast();
		fullAdd(&adr,&y);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		inst(this);
		writeIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(3,SIG_MODE16_ONLY))
	{
		writeLong(&ZERO,&adr,&idb.high);
	}
}

void W65816::Immediate(StageType&& inst)
{
	if(preDecodeStage)
	{
		opPrefetchInIDB();
		lastPipelineStage = inst;
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_MODE16_ONLY))
	{
		fetchInc(&pc,&idb.high);
	}
}

void W65816::ImmediateSpecial(StageType&& inst)
{
	if(preDecodeStage)
	{
		opPrefetchInIDB();
		noAutoIncPC();
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		inst(this);
		++pc;//used to be incPC(2) signal
	}
}

void W65816::Implied(StageType&& inst)
{
	if(preDecodeStage)
	{
		invalidPrefetch();
		noAutoIncPC();
		lastPipelineStage = inst;
		enabledStages.resize(0);//no stages in pipeline
		inst(this);//predecode and ASM disassembly
	}
}
	
void W65816::ImpliedSpecial(StageType&& inst)
{
	if(preDecodeStage)
	{
		invalidPrefetch();
		noAutoIncPC();
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		inst(this);
	}
}	
	
void W65816::RelativeBranch(StageType&& inst)
{
	if(preDecodeStage)
	{
		inst(this);//predecode (special inst for branch) and ASM disassembly
		lastPipelineStage = StageType(dummyStage);

		if(!branchTaken)
		{
			enabledStages.resize(0);
			return;
		}
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		dummyFetchLast();
		halfAdd(&pc.low,&adr.low);
		if(enablingSignals[SIG_NATIVE_MODE])
			fixCarry(&pc.high,&SIGN_EXTENDED_OP_HALF_ADD);
	}
	if(isStageEnabled(1,SIG_PC_CROSS_PAGE_IN_EMUL))
	{
		dummyFetchLast();
		fixCarry(&pc.high,&SIGN_EXTENDED_OP_HALF_ADD);
	}
}

void W65816::RelativeBranchLong(StageType&& inst)
{
	if(preDecodeStage)
	{
		lastPipelineStage = inst;
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		fetchInc(&pc,&adr.high);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		fullAdd(&pc,&adr);
	}
}

void W65816::StackPop(StageType&& inst)
{
	if(preDecodeStage)
	{
		lastPipelineStage = inst;
		noAutoIncPC();
		invalidPrefetch();
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		dummyStage();//The inc is supposed to happen in this stage but I do it all in the "pop" operation
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		pop(&idb.low);
	}
	if(isStageEnabled(2,SIG_MODE16_ONLY))
	{
		pop(&idb.high);
	}
}

void W65816::StackPop8(StageType&& inst)
{
	if(preDecodeStage)
	{
		lastPipelineStage = inst;
		invalidPrefetch();
		noAutoIncPC();
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		dummyStage();//The inc is supposed to happen in this stage but I do it all in the "pop" operation
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		pop(&idb.low);
	}
}

void W65816::StackPop16(StageType&& inst)
{
	if(preDecodeStage)
	{
		lastPipelineStage = inst;
		noAutoIncPC();
		invalidPrefetch();
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		dummyStage();//The inc is supposed to happen in this stage but I do it all in the "pop" operation
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		pop(&idb.low);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		pop(&idb.high);
	}
}

void W65816::StackPush(StageType&& inst)
{
	if(preDecodeStage)
	{
		noAutoIncPC();
		invalidPrefetch();
		inst(this); //execute the instruction early and ASM disassembly
	}

	if(isStageEnabled(0,SIG_MODE16_ONLY))
	{
		push(&idb.high);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		push(&idb.low);
	}
}

void W65816::StackPush8(StageType&& inst)
{
	if(preDecodeStage)
	{
		noAutoIncPC();
		invalidPrefetch();
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		inst(this);
		push(&idb.low);
	}
}

void W65816::StackPush16(StageType&& inst)
{
	if(preDecodeStage)
	{
		noAutoIncPC();
		invalidPrefetch();
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		inst(this); 
		push(&idb.high);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		push(&idb.low);
	}
}

void W65816::StackPEA(StageType&& inst)
{
	if(preDecodeStage)
	{
		lastPipelineStage = inst;
		inst(this);//predecode and ASM disassembly
	}

	if(isStageEnabled(0,SIG_ALWAYS))
	{
		fetchInc(&pc,&adr.high);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		push(&adr.high);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		push(&adr.low);
	}
}

void W65816::StackPEI(StageType&& inst)
{
	if(preDecodeStage)
	{
		lastPipelineStage = inst;
		dhPrefetchInAdr();
		inst(this);//predecode and ASM disassembly
	}
	
	if(isStageEnabled(0,SIG_DL_NOT_ZERO))
	{
		dummyFetchLast();
		halfAdd(&adr.low,&d.low);
		fixCarry(&adr.high,&ZERO);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		fetchInc(&adr,&idb.low);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetch(&adr,&idb.high);
	}
	if(isStageEnabled(3,SIG_ALWAYS))
	{
		push(&idb.high);
	}
	if(isStageEnabled(4,SIG_ALWAYS))
	{
		push(&idb.low);
	}
}

void W65816::StackPER(StageType&& inst)
{
	if(preDecodeStage)
	{
		lastPipelineStage = inst;
		inst(this);//predecode and ASM disassembly
	}
	
	if(isStageEnabled(0,SIG_ALWAYS))
	{
		fetchInc(&pc,&adr.high);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		dummyFetchLast();
		fullAdd(&adr,&pc);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		push(&adr.high);
	}
	if(isStageEnabled(3,SIG_ALWAYS))
	{
		push(&adr.low);
	}
}

void W65816::StackRTI(StageType&& inst)
{
	if(preDecodeStage)
	{
		lastPipelineStage = inst;
		invalidPrefetch();
		inst(this);//predecode and ASM disassembly
	}
	
	if(isStageEnabled(0,SIG_ALWAYS))
	{
		dummyFetchLast();
		// Stage(Stage::SIG_ALWAYS,enableInterupts,true)}, //TODO: Re enable interupts where ? maybe no need to do it as it is saved in P
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		popP();
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		pop(&pc.low);
	}
	if(isStageEnabled(3,SIG_ALWAYS))
	{
		pop(&pc.high);
	}
	if(isStageEnabled(4,SIG_NATIVE_MODE))
	{
		pop(&pbr);
	}
}

void W65816::StackRTS(StageType&& inst)
{
	if(preDecodeStage)
	{
		lastPipelineStage = inst;
		invalidPrefetch();
		inst(this);//predecode and ASM disassembly
	}
	
	if(isStageEnabled(0,SIG_ALWAYS))
	{
		dummyFetchLast();
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		pop(&pc.low);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		pop(&pc.high);
	}
	if(isStageEnabled(3,SIG_ALWAYS))
	{
		dummyFetchLast();
		incReg(&pc);
	}
}

void W65816::StackRTL(StageType&& inst)
{
	if(preDecodeStage)
	{
		lastPipelineStage = inst;
		invalidPrefetch();
		inst(this);//predecode and ASM disassembly
	}
	
	if(isStageEnabled(0,SIG_ALWAYS))
	{
		dummyFetchLast();
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		pop(&pc.low);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		pop(&pc.high);
	}
	if(isStageEnabled(3,SIG_ALWAYS))
	{
		pop(&pbr);
		incReg(&pc);
	}
}	

void W65816::StackRelative(StageType&& inst)
{
	if(preDecodeStage)
	{
		lastPipelineStage = inst;
		inst(this);//predecode and ASM disassembly
	}
	
	if(isStageEnabled(0,SIG_ALWAYS))
	{
		dummyFetchLast();
		moveReg8(&ZERO,&adr.high);
		fullAdd(&adr,&s);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		fetchIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(2,SIG_MODE16_ONLY))
	{
		fetchLong(&ZERO,&adr,&idb.high);
	}
}	

void W65816::StackRelativeWrite(StageType&& inst)
{
	if(preDecodeStage)
	{
		inst(this);//predecode and ASM disassembly
	}
	
	if(isStageEnabled(0,SIG_ALWAYS))
	{
		dummyFetchLast();
		moveReg8(&ZERO,&adr.high);
		fullAdd(&adr,&s);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		inst(this);
		writeIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(2,SIG_MODE16_ONLY))
	{
		writeLong(&ZERO,&adr,&idb.high);
	}
}	

void W65816::StackRelativeIndirectY(StageType&& inst)
{
	if(preDecodeStage)
	{
		lastPipelineStage = inst;
		inst(this);//predecode and ASM disassembly
	}
	
	if(isStageEnabled(0,SIG_ALWAYS))
	{
		dummyFetchLast();
		moveReg8(&ZERO,&adr.high);
		fullAdd(&adr,&s);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		fetchIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetchLong(&ZERO,&adr,&idb.high);
	}
	if(isStageEnabled(3,SIG_ALWAYS))
	{
		dummyFetchLast();
		fullAdd(&idb,&y);
	}
	if(isStageEnabled(4,SIG_ALWAYS))
	{
		moveReg16(&idb,&adr);
		fetchInc(&adr,&idb.low);
	}
	if(isStageEnabled(5,SIG_MODE16_ONLY))
	{
		fetch(&adr,&idb.high);
	}
}	

void W65816::StackRelativeIndirectYWrite(StageType&& inst)
{
	if(preDecodeStage)
	{
		inst(this);//predecode and ASM disassembly
	}
	
	if(isStageEnabled(0,SIG_ALWAYS))
	{
		dummyFetchLast();
		moveReg8(&ZERO,&adr.high);
		fullAdd(&adr,&s);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		fetchIncLong(&ZERO,&adr,&idb.low);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		fetchLong(&ZERO,&adr,&idb.high);
	}
	if(isStageEnabled(3,SIG_ALWAYS))
	{
		dummyFetchLast();
		fullAdd(&idb,&y);
	}
	if(isStageEnabled(4,SIG_ALWAYS))
	{
		moveReg16(&idb,&adr);
		inst(this);
		writeInc(&adr,&idb.low);
	}
	if(isStageEnabled(5,SIG_MODE16_ONLY))
	{
		write(&adr,&idb.high);
	}
}	

void W65816::StackInterupt(StageType&& inst)
{
	if(preDecodeStage)
	{
		lastPipelineStage = StageType(dummyStage);
		noAutoIncPC();
		//no predecode inst here
		//todo: maybe erase asm here
	}

	if(isStageEnabled(0,SIG_NATIVE_MODE))
	{
		push(&pbr);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		push(&pc.high);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		push(&pc.low);
	}
	if(isStageEnabled(3,SIG_ALWAYS))
	{
		pushP();
		enableInterupts(false);
	}
	if(isStageEnabled(4,SIG_ALWAYS))
	{
		inst(this);
		fetchIncLong(&ZERO,&adr,&pc.low);
		moveReg8(&ZERO,&pbr); 
		//TODO: Vector Pull signal here(and next line)
	}
	if(isStageEnabled(5,SIG_ALWAYS))
	{
		fetchLong(&ZERO,&adr,&pc.high);
	}
}

void W65816::BlockMoveN(StageType&& inst)
{
	if(preDecodeStage)
	{
		lastPipelineStage = inst;
		inst(this);//predecode and ASM disassembly
	}
	
	if(isStageEnabled(0,SIG_ALWAYS))
	{
		fetchDec(&pc,&adr.high);
		moveReg8(&adr.low,&dbr);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		fetchIncLong(&adr.high,&x,&idb.low);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		writeIncLong(&adr.low,&y,&idb.low);
	}
	if(isStageEnabled(3,SIG_ALWAYS))
	{
		dummyFetchLast();
		decReg(&acc);
		if(enablingSignals[SIG_ACC_ZERO])
			incReg(&pc);
	}
	if(isStageEnabled(4,SIG_ALWAYS))
	{
		if(enablingSignals[SIG_ACC_ZERO]) incReg(&pc);
		if(enablingSignals[SIG_ACC_NOT_ZERO]) decReg(&pc);
	}
}	

void W65816::BlockMoveP(StageType&& inst)
{
	if(preDecodeStage)
	{
		lastPipelineStage = inst;
		inst(this);//predecode and ASM disassembly
	}
	
	if(isStageEnabled(0,SIG_ALWAYS))
	{
		fetchDec(&pc,&adr.high);
		moveReg8(&adr.low,&dbr);
	}
	if(isStageEnabled(1,SIG_ALWAYS))
	{
		fetchDecLong(&adr.high,&x,&idb.low);
	}
	if(isStageEnabled(2,SIG_ALWAYS))
	{
		writeDecLong(&adr.low,&y,&idb.low);
	}
	if(isStageEnabled(3,SIG_ALWAYS))
	{
		dummyFetchLast();
		decReg(&acc);
		if(enablingSignals[SIG_ACC_ZERO])
			incReg(&pc);
	}
	if(isStageEnabled(4,SIG_ALWAYS))
	{
		if(enablingSignals[SIG_ACC_ZERO]) incReg(&pc);
		if(enablingSignals[SIG_ACC_NOT_ZERO]) decReg(&pc);
	}
}	
