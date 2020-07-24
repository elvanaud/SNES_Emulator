#include "W65816.h"

using std::bind;
using namespace std::placeholders;

void W65816::initializeAddressingModes()
{
    Immediate.setStages({{Stage(Stage::SIG_MODE16_ONLY,fetchInc,&pc,&idb.high)},{Stage(Stage::SIG_INST,dummyStage)}});
    Immediate.setSignals({bind(incPC,this,1),bind(opPrefetchInIDB,this)});

    ImmediateSpecial.setStages({{Stage(Stage::SIG_INST,dummyStage)},{Stage(Stage::SIG_DUMMY_STAGE, dummyStage)}});
    ImmediateSpecial.setSignals({bind(incPC,this,2),bind(opPrefetchInIDB,this)});

    Implied.setStages({{Stage(Stage::SIG_INST,dummyStage)}});
    Implied.setPredecodeSignals({bind(invalidPrefetch,this)});

    ImpliedSpecial.setStages({{Stage(Stage::SIG_INST,dummyStage)},{Stage(Stage::SIG_DUMMY_STAGE, dummyStage)}});
    ImpliedSpecial.setPredecodeSignals({bind(invalidPrefetch,this)});


    Absolute.setStages({{Stage(Stage::SIG_ALWAYS, fetchInc,&pc,&adr.high)},{Stage(Stage::SIG_ALWAYS,fetchInc,&adr,&idb.low)},
                       {Stage(Stage::SIG_MODE16_ONLY,fetch,&adr,&idb.high)},{Stage(Stage::SIG_INST,dummyStage)}});
    Absolute.setSignals({bind(incPC,this,1)});


    AbsoluteWrite.setStages({{Stage(Stage::SIG_ALWAYS,fetchInc,&pc,&adr.high),Stage(Stage::SIG_INST,dummyStage)},{Stage(Stage::SIG_ALWAYS,writeInc,&adr,&idb.low)},
                            {Stage(Stage::SIG_MODE16_ONLY,write,&adr,&idb.high)},{Stage(Stage::SIG_DUMMY_STAGE,dummyStage)}});
    AbsoluteWrite.setSignals({bind(incPC,this,1)});


    AbsoluteRMW.setStages({ {Stage(Stage::SIG_ALWAYS,fetchInc,&pc,&adr.high)},{Stage(Stage::SIG_ALWAYS,fetchInc,&adr,&idb.low)},
                            {Stage(Stage::SIG_MODE16_ONLY,fetch,&adr,&idb.high)},{Stage(Stage::SIG_INST,dummyStage),Stage(Stage::SIG_MODE8_ONLY,decReg,&adr)},
                            {Stage(Stage::SIG_MODE16_ONLY,writeDec,&adr,&idb.high)},{Stage(Stage::SIG_ALWAYS,write,&adr,&idb.low)},{Stage(Stage::SIG_DUMMY_STAGE, dummyStage)}});
    AbsoluteRMW.setSignals({bind(incPC,this,1)});


    AbsoluteJMP.setStages({{Stage(Stage::SIG_ALWAYS,fetch,&pc,&pc.high),Stage(Stage::SIG_ALWAYS,moveReg8,&adr.low,&pc.low)},{Stage(Stage::SIG_INST,dummyStage)}});
    AbsoluteJMP.setSignals({bind(incPC,this,1)});


    AbsoluteJSR.setStages({ {Stage(Stage::SIG_ALWAYS,fetch,&pc,&adr.high)},
                            {Stage(Stage::SIG_ALWAYS,dummyFetchLast)},
                            {Stage(Stage::SIG_ALWAYS,push,&pc.high),Stage(Stage::SIG_ALWAYS,moveReg8,&adr.high,&pc.high)},
                            {Stage(Stage::SIG_ALWAYS,push,&pc.low),Stage(Stage::SIG_ALWAYS,moveReg8,&adr.low,&pc.low)},
                            {Stage(Stage::SIG_INST,dummyStage)}});
    AbsoluteJSR.setSignals({bind(incPC,this,1)});


    AbsoluteLong.setStages({{Stage(Stage::SIG_ALWAYS,fetchInc,&pc,&adr.high)},{Stage(Stage::SIG_ALWAYS,fetchInc,&pc,&tmpBank)},{Stage(Stage::SIG_ALWAYS,fetchIncLong,&tmpBank,&adr,&idb.low)},
                            {Stage(Stage::SIG_MODE16_ONLY,fetchLong,&tmpBank,&adr,&idb.high)},{Stage(Stage::SIG_INST,dummyStage)}});
    AbsoluteLong.setSignals({bind(incPC,this,1)});


    AbsoluteLongWrite.setStages({   {Stage(Stage::SIG_ALWAYS,fetchInc,&pc,&adr.high)},
                                    {Stage(Stage::SIG_ALWAYS,fetchInc,&pc,&tmpBank),Stage(Stage::SIG_INST,dummyStage)},
                                    {Stage(Stage::SIG_ALWAYS,writeIncLong,&tmpBank,&adr,&idb.low)},
                                    {Stage(Stage::SIG_MODE16_ONLY,writeLong,&tmpBank,&adr,&idb.high)},
                                    {Stage(Stage::SIG_DUMMY_STAGE,dummyStage)}});
    AbsoluteLongWrite.setSignals({bind(incPC,this,1)});


    AbsoluteLongJMP.setStages({     {Stage(Stage::SIG_ALWAYS,fetchInc,&pc,&adr.high)},
                                    {Stage(Stage::SIG_ALWAYS,fetchInc,&pc,&pbr),Stage(Stage::SIG_ALWAYS,moveReg16,&adr,&pc)},
                                    {Stage(Stage::SIG_INST,dummyStage)}});
    AbsoluteLongJMP.setSignals({bind(incPC,this,1)});


    AbsoluteLongJSL.setStages({ {Stage(Stage::SIG_ALWAYS,fetchInc,&pc,&adr.high)},
                                {Stage(Stage::SIG_ALWAYS,push,&pbr)},
                                {Stage(Stage::SIG_ALWAYS,dummyFetchLast)},
                                {Stage(Stage::SIG_ALWAYS,fetch,&pc,&pbr)},
                                {Stage(Stage::SIG_ALWAYS,push,&pc.high)},
                                {Stage(Stage::SIG_ALWAYS,push,&pc.low),Stage(Stage::SIG_ALWAYS,moveReg16,&adr,&pc)},
                                {Stage(Stage::SIG_INST,dummyStage)}});
    AbsoluteLongJSL.setSignals({bind(incPC,this,1)});

    AbsoluteX.setStages({   {Stage(Stage::SIG_ALWAYS,fetchInc,&pc,&adr.high),Stage(Stage::SIG_ALWAYS,halfAdd,&adr.low,&x.low)},
                            {Stage(Stage::SIG_X_CROSS_PAGE,dummyFetchLong,&dbr,&adr),Stage(Stage::SIG_X_CROSS_PAGE,fixCarry,&adr.high,&x.high)},
                            {Stage(Stage::SIG_ALWAYS,fetchInc,&adr,&idb.low)},
                            {Stage(Stage::SIG_MODE16_ONLY,fetch,&adr,&idb.high)},
                            {Stage(Stage::SIG_INST,dummyStage)}});
    AbsoluteX.setSignals({bind(incPC,this,1)});


    AbsoluteXWrite.setStages({  {Stage(Stage::SIG_ALWAYS,fetchInc,&pc,&adr.high),Stage(Stage::SIG_ALWAYS,halfAdd,&adr.low,&x.low)},
                                {Stage(Stage::SIG_X_CROSS_PAGE,dummyFetchLong,&dbr,&adr),Stage(Stage::SIG_X_CROSS_PAGE,fixCarry,&adr.high,&x.high)},
                                {Stage(Stage::SIG_INST,dummyStage),Stage(Stage::SIG_ALWAYS,writeInc,&adr,&idb.low)},
                                {Stage(Stage::SIG_MODE16_ONLY,write,&adr,&idb.high)},
                                {Stage(Stage::SIG_DUMMY_STAGE,dummyStage)}});
    AbsoluteXWrite.setSignals({bind(incPC,this,1)});
}
