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
                            {Stage(Stage::SIG_MODE16_ONLY,fetch,&adr,&idb.high)},{Stage(Stage::SIG_ALWAYS,dummyFetchLast),Stage(Stage::SIG_INST,dummyStage),Stage(Stage::SIG_MODE8_ONLY,decReg,&adr)},
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


    AbsoluteXLong.setStages({   {Stage(Stage::SIG_ALWAYS,fetchInc,&pc,&adr.high),Stage(Stage::SIG_ALWAYS,halfAdd,&adr.low,&x.low)},
                                {Stage(Stage::SIG_ALWAYS,fetchInc,&pc,&tmpBank),Stage(Stage::SIG_X_CROSS_PAGE,fixCarry,&adr.high,&x.high)},
                                {Stage(Stage::SIG_ALWAYS,fetchIncLong,&tmpBank,&adr,&idb.low)},
                                {Stage(Stage::SIG_MODE16_ONLY,fetchLong,&tmpBank,&adr,&idb.high)},
                                {Stage(Stage::SIG_INST,dummyStage)}});
    AbsoluteXLong.setSignals({bind(incPC,this,1)});


    AbsoluteXLongWrite.setStages({  {Stage(Stage::SIG_ALWAYS,fetchInc,&pc,&adr.high),Stage(Stage::SIG_ALWAYS,halfAdd,&adr.low,&x.low)},
                                    {Stage(Stage::SIG_ALWAYS,fetchInc,&pc,&tmpBank),Stage(Stage::SIG_X_CROSS_PAGE,fixCarry,&adr.high,&x.high)},
                                    {Stage(Stage::SIG_INST,dummyStage),Stage(Stage::SIG_ALWAYS,writeIncLong,&tmpBank,&adr,&idb.low)},
                                    {Stage(Stage::SIG_MODE16_ONLY,writeLong,&tmpBank,&adr,&idb.high)},
                                    {Stage(Stage::SIG_DUMMY_STAGE,dummyStage)}});
    AbsoluteXLongWrite.setSignals({bind(incPC,this,1)});


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


    AbsoluteXRMW.setStages({    {Stage(Stage::SIG_ALWAYS,fetchInc,&pc,&adr.high),Stage(Stage::SIG_ALWAYS,halfAdd,&adr.low,&x.low)},
                                {Stage(Stage::SIG_X_CROSS_PAGE,dummyFetchLong,&dbr,&adr),Stage(Stage::SIG_X_CROSS_PAGE,fixCarry,&adr.high,&x.high)},
                                {Stage(Stage::SIG_ALWAYS,fetchInc,&adr,&idb.low)},
                                {Stage(Stage::SIG_MODE16_ONLY,fetch,&adr,&idb.high)},
                                {Stage(Stage::SIG_ALWAYS,dummyFetchLast),Stage(Stage::SIG_INST,dummyStage),Stage(Stage::SIG_MODE8_ONLY,decReg,&adr)},
                                {Stage(Stage::SIG_MODE16_ONLY,writeDec,&adr,&idb.high)},
                                {Stage(Stage::SIG_ALWAYS,write,&adr,&idb.low)},
                                {Stage(Stage::SIG_DUMMY_STAGE,dummyStage)}});
    AbsoluteXRMW.setSignals({bind(incPC,this,1)});


    AbsoluteY.setStages({   {Stage(Stage::SIG_ALWAYS,fetchInc,&pc,&adr.high),Stage(Stage::SIG_ALWAYS,halfAdd,&adr.low,&y.low)},
                            {Stage(Stage::SIG_Y_CROSS_PAGE,dummyFetchLong,&dbr,&adr),Stage(Stage::SIG_Y_CROSS_PAGE,fixCarry,&adr.high,&y.high)},
                            {Stage(Stage::SIG_ALWAYS,fetchInc,&adr,&idb.low)},
                            {Stage(Stage::SIG_MODE16_ONLY,fetch,&adr,&idb.high)},
                            {Stage(Stage::SIG_INST,dummyStage)}});
    AbsoluteY.setSignals({bind(incPC,this,1)});


    AbsoluteYWrite.setStages({  {Stage(Stage::SIG_ALWAYS,fetchInc,&pc,&adr.high),Stage(Stage::SIG_ALWAYS,halfAdd,&adr.low,&y.low)},
                                {Stage(Stage::SIG_Y_CROSS_PAGE,dummyFetchLong,&dbr,&adr),Stage(Stage::SIG_Y_CROSS_PAGE,fixCarry,&adr.high,&y.high)},
                                {Stage(Stage::SIG_INST,dummyStage),Stage(Stage::SIG_ALWAYS,writeInc,&adr,&idb.low)},
                                {Stage(Stage::SIG_MODE16_ONLY,write,&adr,&idb.high)},
                                {Stage(Stage::SIG_DUMMY_STAGE,dummyStage)}});
    AbsoluteYWrite.setSignals({bind(incPC,this,1)});


    AbsoluteXIndirectJMP.setStages({    {Stage(Stage::SIG_ALWAYS,fetchInc,&pc,&adr.high)},
                                        {Stage(Stage::SIG_ALWAYS,dummyFetchLast),Stage(Stage::SIG_ALWAYS,fullAdd,&adr,&x)},
                                        {Stage(Stage::SIG_ALWAYS,fetchIncLong,&pbr,&adr,&pc.low)},
                                        {Stage(Stage::SIG_ALWAYS,fetchLong,&pbr,&adr,&pc.high)},
                                        {Stage(Stage::SIG_INST,dummyStage)}});
    AbsoluteXIndirectJMP.setSignals({bind(incPC,this,1)});


    AbsoluteXIndirectJSR.setStages({    {Stage(Stage::SIG_ALWAYS,push,&pc.high)},
                                        {Stage(Stage::SIG_ALWAYS,push,&pc.low)},
                                        {Stage(Stage::SIG_ALWAYS,fetch,&pc,&adr.high)},
                                        {Stage(Stage::SIG_ALWAYS,dummyFetchLast),Stage(Stage::SIG_ALWAYS,fullAdd,&adr,&x)},
                                        {Stage(Stage::SIG_ALWAYS,fetchIncLong,&pbr,&adr,&pc.low)},
                                        {Stage(Stage::SIG_ALWAYS,fetchLong,&pbr,&adr,&pc.high)},
                                        {Stage(Stage::SIG_INST,dummyStage)}});
    AbsoluteXIndirectJSR.setSignals({bind(incPC,this,1)});


    AbsoluteIndirectJML.setStages({ {Stage(Stage::SIG_ALWAYS,fetchInc,&pc,&adr.high)},
                                    {Stage(Stage::SIG_ALWAYS,fetchIncLong,&ZERO,&adr,&pc.low)},
                                    {Stage(Stage::SIG_ALWAYS,fetchIncLong,&ZERO,&adr,&pc.high)},
                                    {Stage(Stage::SIG_ALWAYS,fetchLong,&ZERO,&adr,&pbr)},
                                    {Stage(Stage::SIG_INST,dummyStage)}});
    AbsoluteIndirectJML.setSignals({bind(incPC,this,1)});


    AbsoluteIndirectJMP.setStages({ {Stage(Stage::SIG_ALWAYS,fetchInc,&pc,&adr.high)},
                                    {Stage(Stage::SIG_ALWAYS,fetchIncLong,&ZERO,&adr,&pc.low)},
                                    {Stage(Stage::SIG_ALWAYS,fetchLong,&ZERO,&adr,&pc.high)},
                                    {Stage(Stage::SIG_INST,dummyStage)}});
    AbsoluteIndirectJMP.setSignals({bind(incPC,this,1)});


    Accumulator.setStages({{Stage(Stage::SIG_INST,dummyStage),Stage(Stage::SIG_ALWAYS,moveReg8,&idb.low,&acc.low),Stage(Stage::SIG_MODE16_ONLY,moveReg8,&idb.high,&acc.high)}});
    Accumulator.setPredecodeSignals({bind(invalidPrefetch,this),bind(accPrefetchInIDB,this)});


    Direct.setStages({  {Stage(Stage::SIG_DL_NOT_ZERO,dummyFetchLast),Stage(Stage::SIG_DL_NOT_ZERO,halfAdd,&adr.low,&d.low),Stage(Stage::SIG_DL_NOT_ZERO,fixCarry,&adr.high,&ZERO)},
                        {Stage(Stage::SIG_ALWAYS,fetchIncLong,&ZERO,&adr,&idb.low)},
                        {Stage(Stage::SIG_MODE16_ONLY,fetchLong,&ZERO,&adr,&idb.high)},
                        {Stage(Stage::SIG_INST,dummyStage)}});
    Direct.setSignals({bind(incPC,this,1),bind(dhPrefetchInAdr,this)});


    DirectWrite.setStages({ {Stage(Stage::SIG_DL_NOT_ZERO,dummyFetchLast),Stage(Stage::SIG_DL_NOT_ZERO,halfAdd,&adr.low,&d.low),Stage(Stage::SIG_DL_NOT_ZERO,fixCarry,&adr.high,&ZERO)},
                            {Stage(Stage::SIG_INST,dummyStage),Stage(Stage::SIG_ALWAYS,writeIncLong,&ZERO,&adr,&idb.low)},
                            {Stage(Stage::SIG_MODE16_ONLY,writeLong,&ZERO,&adr,&idb.high)},
                            {Stage(Stage::SIG_DUMMY_STAGE,dummyStage)}});
    DirectWrite.setSignals({bind(incPC,this,1),bind(dhPrefetchInAdr,this)});


    DirectRMW.setStages({   {Stage(Stage::SIG_DL_NOT_ZERO,dummyFetchLast),Stage(Stage::SIG_DL_NOT_ZERO,halfAdd,&adr.low,&d.low),Stage(Stage::SIG_DL_NOT_ZERO,fixCarry,&adr.high,&ZERO)},
                            {Stage(Stage::SIG_ALWAYS,fetchIncLong,&ZERO,&adr,&idb.low)},
                            {Stage(Stage::SIG_MODE16_ONLY,fetchLong,&ZERO,&adr,&idb.high)},
                            {Stage(Stage::SIG_ALWAYS,dummyFetchLast),Stage(Stage::SIG_INST,dummyStage),Stage(Stage::SIG_MODE8_ONLY,decReg,&adr)},
                            {Stage(Stage::SIG_MODE16_ONLY,writeDecLong,&ZERO,&adr,&idb.high)},
                            {Stage(Stage::SIG_ALWAYS,writeLong,&ZERO,&adr,&idb.low)},
                            {Stage(Stage::SIG_DUMMY_STAGE,dummyStage)}});
    DirectRMW.setSignals({bind(incPC,this,1),bind(dhPrefetchInAdr,this)});


    DirectXIndirect.setStages({ {Stage(Stage::SIG_DL_NOT_ZERO,dummyFetchLast),Stage(Stage::SIG_DL_NOT_ZERO,halfAdd,&adr.low,&d.low),Stage(Stage::SIG_DL_NOT_ZERO,fixCarry,&adr.high,&ZERO)},
                                {Stage(Stage::SIG_ALWAYS,dummyFetchLast),Stage(Stage::SIG_ALWAYS,fullAdd,&adr,&x)},
                                {Stage(Stage::SIG_ALWAYS,fetchIncLong,&ZERO,&adr,&idb.low)},
                                {Stage(Stage::SIG_ALWAYS,fetchLong,&ZERO,&adr,&adr.high),Stage(Stage::SIG_ALWAYS,moveReg8,&idb.low,&adr.low)},
                                {Stage(Stage::SIG_ALWAYS,fetchIncLong,&ZERO,&adr,&idb.low)},
                                {Stage(Stage::SIG_MODE16_ONLY,fetchLong,&ZERO,&adr,&idb.high)},
                                {Stage(Stage::SIG_INST,dummyStage)}});
    DirectXIndirect.setSignals({bind(incPC,this,1),bind(dhPrefetchInAdr,this)});

    DirectXIndirectWrite.setStages({{Stage(Stage::SIG_DL_NOT_ZERO,dummyFetchLast),Stage(Stage::SIG_DL_NOT_ZERO,halfAdd,&adr.low,&d.low),Stage(Stage::SIG_DL_NOT_ZERO,fixCarry,&adr.high,&ZERO)},
                                    {Stage(Stage::SIG_ALWAYS,dummyFetchLast),Stage(Stage::SIG_ALWAYS,fullAdd,&adr,&x)},
                                    {Stage(Stage::SIG_ALWAYS,fetchIncLong,&ZERO,&adr,&idb.low)},
                                    {Stage(Stage::SIG_ALWAYS,fetchLong,&ZERO,&adr,&adr.high),Stage(Stage::SIG_ALWAYS,moveReg8,&idb.low,&adr.low)},
                                    {Stage(Stage::SIG_INST,dummyStage),Stage(Stage::SIG_ALWAYS,writeIncLong,&ZERO,&adr,&idb.low)},
                                    {Stage(Stage::SIG_MODE16_ONLY,writeLong,&ZERO,&adr,&idb.high)},
                                    {Stage(Stage::SIG_DUMMY_STAGE,dummyStage)}});
    DirectXIndirectWrite.setSignals({bind(incPC,this,1),bind(dhPrefetchInAdr,this)});


    DirectIndirect.setStages({  {Stage(Stage::SIG_DL_NOT_ZERO,dummyFetchLast),Stage(Stage::SIG_DL_NOT_ZERO,halfAdd,&adr.low,&d.low),Stage(Stage::SIG_DL_NOT_ZERO,fixCarry,&adr.high,&ZERO)},
                                {Stage(Stage::SIG_ALWAYS,fetchIncLong,&ZERO,&adr,&idb.low)},
                                {Stage(Stage::SIG_ALWAYS,fetchLong,&ZERO,&adr,&adr.high),Stage(Stage::SIG_ALWAYS,moveReg8,&idb.low,&adr.low)},
                                {Stage(Stage::SIG_ALWAYS,fetchIncLong,&ZERO,&adr,&idb.low)},
                                {Stage(Stage::SIG_MODE16_ONLY,fetchLong,&ZERO,&adr,&idb.high)},
                                {Stage(Stage::SIG_INST,dummyStage)}});
    DirectIndirect.setSignals({bind(incPC,this,1),bind(dhPrefetchInAdr,this)});

    DirectIndirectWrite.setStages({ {Stage(Stage::SIG_DL_NOT_ZERO,dummyFetchLast),Stage(Stage::SIG_DL_NOT_ZERO,halfAdd,&adr.low,&d.low),Stage(Stage::SIG_DL_NOT_ZERO,fixCarry,&adr.high,&ZERO)},
                                    {Stage(Stage::SIG_ALWAYS,fetchIncLong,&ZERO,&adr,&idb.low)},
                                    {Stage(Stage::SIG_ALWAYS,fetchLong,&ZERO,&adr,&adr.high),Stage(Stage::SIG_ALWAYS,moveReg8,&idb.low,&adr.low)},
                                    {Stage(Stage::SIG_INST,dummyStage),Stage(Stage::SIG_ALWAYS,writeIncLong,&ZERO,&adr,&idb.low)},
                                    {Stage(Stage::SIG_MODE16_ONLY,writeLong,&ZERO,&adr,&idb.high)},
                                    {Stage(Stage::SIG_DUMMY_STAGE,dummyStage)}});
    DirectIndirectWrite.setSignals({bind(incPC,this,1),bind(dhPrefetchInAdr,this)});


    DirectIndirectY.setStages({ {Stage(Stage::SIG_DL_NOT_ZERO,dummyFetchLast),Stage(Stage::SIG_DL_NOT_ZERO,halfAdd,&adr.low,&d.low),Stage(Stage::SIG_DL_NOT_ZERO,fixCarry,&adr.high,&ZERO)},
                                {Stage(Stage::SIG_ALWAYS,fetchIncLong,&ZERO,&adr,&idb.low)},
                                {Stage(Stage::SIG_ALWAYS,fetchLong,&ZERO,&adr,&adr.high),Stage(Stage::SIG_ALWAYS,moveReg8,&idb.low,&adr.low),Stage(Stage::SIG_ALWAYS,halfAdd,&adr.low,&y.low)},
                                {Stage(Stage::SIG_INDIRECT_Y_CROSS_PAGE_OR_X16,dummyFetch,&adr),(Stage::SIG_INDIRECT_Y_CROSS_PAGE_OR_X16,fixCarry,&adr.high,&y.high)},
                                {Stage(Stage::SIG_ALWAYS,fetchInc,&adr,&idb.low)},
                                {Stage(Stage::SIG_MODE16_ONLY,fetch,&adr,&idb.high)},
                                {Stage(Stage::SIG_INST,dummyStage)}});
    DirectIndirectY.setSignals({bind(incPC,this,1),bind(dhPrefetchInAdr,this)});


    DirectIndirectYWrite.setStages({{Stage(Stage::SIG_DL_NOT_ZERO,dummyFetchLast),Stage(Stage::SIG_DL_NOT_ZERO,halfAdd,&adr.low,&d.low),Stage(Stage::SIG_DL_NOT_ZERO,fixCarry,&adr.high,&ZERO)},
                                    {Stage(Stage::SIG_ALWAYS,fetchIncLong,&ZERO,&adr,&idb.low)},
                                    {Stage(Stage::SIG_ALWAYS,fetchLong,&ZERO,&adr,&adr.high),Stage(Stage::SIG_ALWAYS,moveReg8,&idb.low,&adr.low),Stage(Stage::SIG_ALWAYS,halfAdd,&adr.low,&y.low)},
                                    {Stage(Stage::SIG_INDIRECT_Y_CROSS_PAGE_OR_X16,dummyFetch,&adr),(Stage::SIG_INDIRECT_Y_CROSS_PAGE_OR_X16,fixCarry,&adr.high,&y.high)},
                                    {Stage(Stage::SIG_INST,dummyStage),Stage(Stage::SIG_ALWAYS,writeInc,&adr,&idb.low)},
                                    {Stage(Stage::SIG_MODE16_ONLY,write,&adr,&idb.high)},
                                    {Stage(Stage::SIG_DUMMY_STAGE,dummyStage)}});
    DirectIndirectYWrite.setSignals({bind(incPC,this,1),bind(dhPrefetchInAdr,this)});


    DirectX.setStages({ {Stage(Stage::SIG_DL_NOT_ZERO,dummyFetchLast),Stage(Stage::SIG_DL_NOT_ZERO,halfAdd,&adr.low,&d.low),Stage(Stage::SIG_DL_NOT_ZERO,fixCarry,&adr.high,&ZERO)},
                        {Stage(Stage::SIG_ALWAYS,dummyFetchLast),Stage(Stage::SIG_ALWAYS,fullAdd,&adr,&x)},
                        {Stage(Stage::SIG_ALWAYS,fetchIncLong,&ZERO,&adr,&idb.low)},
                        {Stage(Stage::SIG_MODE16_ONLY,fetchLong,&ZERO,&adr,&idb.high)},
                        {Stage(Stage::SIG_INST,dummyStage)}});
    DirectX.setSignals({bind(incPC,this,1),bind(dhPrefetchInAdr,this)});


    DirectXWrite.setStages({{Stage(Stage::SIG_DL_NOT_ZERO,dummyFetchLast),Stage(Stage::SIG_DL_NOT_ZERO,halfAdd,&adr.low,&d.low),Stage(Stage::SIG_DL_NOT_ZERO,fixCarry,&adr.high,&ZERO)},
                            {Stage(Stage::SIG_ALWAYS,dummyFetchLast),Stage(Stage::SIG_ALWAYS,fullAdd,&adr,&x)},
                            {Stage(Stage::SIG_INST,dummyStage),Stage(Stage::SIG_ALWAYS,writeIncLong,&ZERO,&adr,&idb.low)},
                            {Stage(Stage::SIG_MODE16_ONLY,writeLong,&ZERO,&adr,&idb.high)},
                            {Stage(Stage::SIG_DUMMY_STAGE,dummyStage)}});
    DirectXWrite.setSignals({bind(incPC,this,1),bind(dhPrefetchInAdr,this)});


    DirectXRMW.setStages({  {Stage(Stage::SIG_DL_NOT_ZERO,dummyFetchLast),Stage(Stage::SIG_DL_NOT_ZERO,halfAdd,&adr.low,&d.low),Stage(Stage::SIG_DL_NOT_ZERO,fixCarry,&adr.high,&ZERO)},
                            {Stage(Stage::SIG_ALWAYS,dummyFetchLast),Stage(Stage::SIG_ALWAYS,fullAdd,&adr,&x)},
                            {Stage(Stage::SIG_ALWAYS,fetchIncLong,&ZERO,&adr,&idb.low)},
                            {Stage(Stage::SIG_MODE16_ONLY,fetchLong,&ZERO,&adr,&idb.high)},
                            {Stage(Stage::SIG_ALWAYS,dummyFetchLast),Stage(Stage::SIG_INST,dummyStage),Stage(Stage::SIG_MODE8_ONLY,decReg,&adr)},
                            {Stage(Stage::SIG_MODE16_ONLY,writeDecLong,&ZERO,&adr,&idb.high)},
                            {Stage(Stage::SIG_ALWAYS,writeLong,&ZERO,&adr,&idb.low)},
                            {Stage(Stage::SIG_DUMMY_STAGE,dummyStage)}});
    DirectXRMW.setSignals({bind(incPC,this,1),bind(dhPrefetchInAdr,this)});


    DirectY.setStages({ {Stage(Stage::SIG_DL_NOT_ZERO,dummyFetchLast),Stage(Stage::SIG_DL_NOT_ZERO,halfAdd,&adr.low,&d.low),Stage(Stage::SIG_DL_NOT_ZERO,fixCarry,&adr.high,&ZERO)},
                        {Stage(Stage::SIG_ALWAYS,dummyFetchLast),Stage(Stage::SIG_ALWAYS,fullAdd,&adr,&y)},
                        {Stage(Stage::SIG_ALWAYS,fetchIncLong,&ZERO,&adr,&idb.low)},
                        {Stage(Stage::SIG_MODE16_ONLY,fetchLong,&ZERO,&adr,&idb.high)},
                        {Stage(Stage::SIG_INST,dummyStage)}});
    DirectY.setSignals({bind(incPC,this,1),bind(dhPrefetchInAdr,this)});


    DirectYWrite.setStages({{Stage(Stage::SIG_DL_NOT_ZERO,dummyFetchLast),Stage(Stage::SIG_DL_NOT_ZERO,halfAdd,&adr.low,&d.low),Stage(Stage::SIG_DL_NOT_ZERO,fixCarry,&adr.high,&ZERO)},
                            {Stage(Stage::SIG_ALWAYS,dummyFetchLast),Stage(Stage::SIG_ALWAYS,fullAdd,&adr,&y)},
                            {Stage(Stage::SIG_INST,dummyStage),Stage(Stage::SIG_ALWAYS,writeIncLong,&ZERO,&adr,&idb.low)},
                            {Stage(Stage::SIG_MODE16_ONLY,writeLong,&ZERO,&adr,&idb.high)},
                            {Stage(Stage::SIG_DUMMY_STAGE,dummyStage)}});
    DirectYWrite.setSignals({bind(incPC,this,1),bind(dhPrefetchInAdr,this)});
}
