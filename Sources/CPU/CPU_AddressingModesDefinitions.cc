#include "W65816.h"

using std::bind;
using namespace std::placeholders;

void W65816::initializeAddressingModes()
{
    Absolute.setStages({{Stage(Stage::SIG_ALWAYS, fetchInc,&pc,&adr.high)},
                        {Stage(Stage::SIG_ALWAYS,fetchInc,&adr,&idb.low)},
                        {Stage(Stage::SIG_MODE16_ONLY,fetch,&adr,&idb.high)},
                        {Stage(Stage::SIG_INST,dummyStage)}});
    Absolute.setSignals({bind(incPC,this,1)});


    AbsoluteWrite.setStages({   {Stage(Stage::SIG_ALWAYS,fetchInc,&pc,&adr.high),Stage(Stage::SIG_INST,dummyStage)},
                                {Stage(Stage::SIG_ALWAYS,writeInc,&adr,&idb.low)},
                                {Stage(Stage::SIG_MODE16_ONLY,write,&adr,&idb.high)},
                                {Stage(Stage::SIG_DUMMY_STAGE,dummyStage)}});
    AbsoluteWrite.setSignals({bind(incPC,this,1)});


    AbsoluteRMW.setStages({ {Stage(Stage::SIG_ALWAYS,fetchInc,&pc,&adr.high)},
                            {Stage(Stage::SIG_ALWAYS,fetchInc,&adr,&idb.low)},
                            {Stage(Stage::SIG_MODE16_ONLY,fetch,&adr,&idb.high)},
                            {Stage(Stage::SIG_ALWAYS,dummyFetchLast),Stage(Stage::SIG_INST,dummyStage),Stage(Stage::SIG_MODE8_ONLY,decReg,&adr)},
                            {Stage(Stage::SIG_MODE16_ONLY,writeDec,&adr,&idb.high)},{Stage(Stage::SIG_ALWAYS,write,&adr,&idb.low)},
                            {Stage(Stage::SIG_DUMMY_STAGE, dummyStage)}});
    AbsoluteRMW.setSignals({bind(incPC,this,1)});


    AbsoluteJMP.setStages({ {Stage(Stage::SIG_ALWAYS,fetch,&pc,&pc.high),Stage(Stage::SIG_ALWAYS,moveReg8,&adr.low,&pc.low)},
                            {Stage(Stage::SIG_INST,dummyStage)}});
    AbsoluteJMP.setSignals({bind(incPC,this,1)});


    AbsoluteJSR.setStages({ {Stage(Stage::SIG_ALWAYS,fetch,&pc,&adr.high)},
                            {Stage(Stage::SIG_ALWAYS,dummyFetchLast)},
                            {Stage(Stage::SIG_ALWAYS,push,&pc.high),Stage(Stage::SIG_ALWAYS,moveReg8,&adr.high,&pc.high)},
                            {Stage(Stage::SIG_ALWAYS,push,&pc.low),Stage(Stage::SIG_ALWAYS,moveReg8,&adr.low,&pc.low)},
                            {Stage(Stage::SIG_INST,dummyStage)}});
    AbsoluteJSR.setSignals({bind(incPC,this,1)});


    AbsoluteLong.setStages({{Stage(Stage::SIG_ALWAYS,fetchInc,&pc,&adr.high)},
                            {Stage(Stage::SIG_ALWAYS,fetchInc,&pc,&tmpBank)},
                            {Stage(Stage::SIG_ALWAYS,fetchIncLong,&tmpBank,&adr,&idb.low)},
                            {Stage(Stage::SIG_MODE16_ONLY,fetchLong,&tmpBank,&adr,&idb.high)},
                            {Stage(Stage::SIG_INST,dummyStage)}});
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
                                {Stage(Stage::SIG_INDIRECT_Y_CROSS_PAGE_OR_X16,dummyFetch,&adr),Stage(Stage::SIG_INDIRECT_Y_CROSS_PAGE_OR_X16,fixCarry,&adr.high,&y.high)},
                                {Stage(Stage::SIG_ALWAYS,fetchInc,&adr,&idb.low)},
                                {Stage(Stage::SIG_MODE16_ONLY,fetch,&adr,&idb.high)},
                                {Stage(Stage::SIG_INST,dummyStage)}});
    DirectIndirectY.setSignals({bind(incPC,this,1),bind(dhPrefetchInAdr,this)});


    DirectIndirectYWrite.setStages({{Stage(Stage::SIG_DL_NOT_ZERO,dummyFetchLast),Stage(Stage::SIG_DL_NOT_ZERO,halfAdd,&adr.low,&d.low),Stage(Stage::SIG_DL_NOT_ZERO,fixCarry,&adr.high,&ZERO)},
                                    {Stage(Stage::SIG_ALWAYS,fetchIncLong,&ZERO,&adr,&idb.low)},
                                    {Stage(Stage::SIG_ALWAYS,fetchLong,&ZERO,&adr,&adr.high),Stage(Stage::SIG_ALWAYS,moveReg8,&idb.low,&adr.low),Stage(Stage::SIG_ALWAYS,halfAdd,&adr.low,&y.low)},
                                    {Stage(Stage::SIG_INDIRECT_Y_CROSS_PAGE_OR_X16,dummyFetch,&adr),Stage(Stage::SIG_INDIRECT_Y_CROSS_PAGE_OR_X16,fixCarry,&adr.high,&y.high)},
                                    {Stage(Stage::SIG_INST,dummyStage),Stage(Stage::SIG_ALWAYS,writeInc,&adr,&idb.low)},
                                    {Stage(Stage::SIG_MODE16_ONLY,write,&adr,&idb.high)},
                                    {Stage(Stage::SIG_DUMMY_STAGE,dummyStage)}});
    DirectIndirectYWrite.setSignals({bind(incPC,this,1),bind(dhPrefetchInAdr,this)});


    DirectIndirectYLong.setStages({ {Stage(Stage::SIG_DL_NOT_ZERO,dummyFetchLast),Stage(Stage::SIG_DL_NOT_ZERO,halfAdd,&adr.low,&d.low),Stage(Stage::SIG_DL_NOT_ZERO,fixCarry,&adr.high,&ZERO)},
                                    {Stage(Stage::SIG_ALWAYS,fetchIncLong,&ZERO,&adr,&idb.low)},
                                    {Stage(Stage::SIG_ALWAYS,fetchIncLong,&ZERO,&adr,&idb.high),Stage(Stage::SIG_ALWAYS,halfAdd,&idb.low,&y.low)},
                                    {Stage(Stage::SIG_ALWAYS,fetchLong,&ZERO,&adr,&tmpBank),Stage(Stage::SIG_INDIRECT_Y_CROSS_PAGE_OR_X16,fixCarry,&idb.high,&y.high)},
                                    {Stage(Stage::SIG_ALWAYS,moveReg16,&idb,&adr),Stage(Stage::SIG_ALWAYS,fetchIncLong,&tmpBank,&adr,&idb.low)},
                                    {Stage(Stage::SIG_MODE16_ONLY,fetchLong,&tmpBank,&adr,&idb.high)},
                                    {Stage(Stage::SIG_INST,dummyStage)}});
    DirectIndirectYLong.setSignals({bind(incPC,this,1),bind(dhPrefetchInAdr,this)});


    DirectIndirectYLongWrite.setStages({{Stage(Stage::SIG_DL_NOT_ZERO,dummyFetchLast),Stage(Stage::SIG_DL_NOT_ZERO,halfAdd,&adr.low,&d.low),Stage(Stage::SIG_DL_NOT_ZERO,fixCarry,&adr.high,&ZERO)},
                                        {Stage(Stage::SIG_ALWAYS,fetchIncLong,&ZERO,&adr,&idb.low)},
                                        {Stage(Stage::SIG_ALWAYS,fetchIncLong,&ZERO,&adr,&idb.high),Stage(Stage::SIG_ALWAYS,halfAdd,&idb.low,&y.low)},
                                        {Stage(Stage::SIG_ALWAYS,fetchLong,&ZERO,&adr,&tmpBank),Stage(Stage::SIG_INDIRECT_Y_CROSS_PAGE_OR_X16,fixCarry,&idb.high,&y.high),Stage(Stage::SIG_ALWAYS,moveReg16,&idb,&adr)},
                                        {Stage(Stage::SIG_INST,dummyStage),Stage(Stage::SIG_ALWAYS,writeIncLong,&tmpBank,&adr,&idb.low)},
                                        {Stage(Stage::SIG_MODE16_ONLY,writeLong,&tmpBank,&adr,&idb.high)},
                                        {Stage(Stage::SIG_DUMMY_STAGE,dummyStage)}});
    DirectIndirectYLongWrite.setSignals({bind(incPC,this,1),bind(dhPrefetchInAdr,this)});


    DirectIndirectLong.setStages({  {Stage(Stage::SIG_DL_NOT_ZERO,dummyFetchLast),Stage(Stage::SIG_DL_NOT_ZERO,halfAdd,&adr.low,&d.low),Stage(Stage::SIG_DL_NOT_ZERO,fixCarry,&adr.high,&ZERO)},
                                    {Stage(Stage::SIG_ALWAYS,fetchIncLong,&ZERO,&adr,&idb.low)},
                                    {Stage(Stage::SIG_ALWAYS,fetchIncLong,&ZERO,&adr,&idb.high)},
                                    {Stage(Stage::SIG_ALWAYS,fetchLong,&ZERO,&adr,&tmpBank),Stage(Stage::SIG_ALWAYS,moveReg16,&idb,&adr)},
                                    {Stage(Stage::SIG_ALWAYS,fetchIncLong,&tmpBank,&adr,&idb.low)},
                                    {Stage(Stage::SIG_MODE16_ONLY,fetchLong,&tmpBank,&adr,&idb.high)},
                                    {Stage(Stage::SIG_INST,dummyStage)}});
    DirectIndirectLong.setSignals({bind(incPC,this,1),bind(dhPrefetchInAdr,this)});


    DirectIndirectLongWrite.setStages({ {Stage(Stage::SIG_DL_NOT_ZERO,dummyFetchLast),Stage(Stage::SIG_DL_NOT_ZERO,halfAdd,&adr.low,&d.low),Stage(Stage::SIG_DL_NOT_ZERO,fixCarry,&adr.high,&ZERO)},
                                        {Stage(Stage::SIG_ALWAYS,fetchIncLong,&ZERO,&adr,&idb.low)},
                                        {Stage(Stage::SIG_ALWAYS,fetchIncLong,&ZERO,&adr,&idb.high)},
                                        {Stage(Stage::SIG_ALWAYS,fetchLong,&ZERO,&adr,&tmpBank),Stage(Stage::SIG_ALWAYS,moveReg16,&idb,&adr)},
                                        {Stage(Stage::SIG_INST,dummyStage),Stage(Stage::SIG_ALWAYS,writeIncLong,&tmpBank,&adr,&idb.low)},
                                        {Stage(Stage::SIG_MODE16_ONLY,writeLong,&tmpBank,&adr,&idb.high)},
                                        {Stage(Stage::SIG_DUMMY_STAGE,dummyStage)}});
    DirectIndirectLongWrite.setSignals({bind(incPC,this,1),bind(dhPrefetchInAdr,this)});


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


    Immediate.setStages({   {Stage(Stage::SIG_MODE16_ONLY,fetchInc,&pc,&idb.high)},
                            {Stage(Stage::SIG_INST,dummyStage)}});
    Immediate.setSignals({bind(incPC,this,1),bind(opPrefetchInIDB,this)});


    ImmediateSpecial.setStages({{Stage(Stage::SIG_INST,dummyStage)},
                                {Stage(Stage::SIG_DUMMY_STAGE, dummyStage)}});
    ImmediateSpecial.setSignals({bind(incPC,this,2),bind(opPrefetchInIDB,this)});


    Implied.setStages({{Stage(Stage::SIG_INST,dummyStage)}});
    Implied.setPredecodeSignals({bind(invalidPrefetch,this)});


    ImpliedSpecial.setStages({  {Stage(Stage::SIG_INST,dummyStage)},
                                {Stage(Stage::SIG_DUMMY_STAGE, dummyStage)}});
    ImpliedSpecial.setPredecodeSignals({bind(invalidPrefetch,this)});


    RelativeBranch.setStages({  {Stage(Stage::SIG_INST,dummyStage)}, //Special Case for branch instruction: the first stage is executed within the decode operation (in T1)
                                {Stage(Stage::SIG_ALWAYS,dummyFetchLast),Stage(Stage::SIG_ALWAYS,halfAdd,&pc.low,&adr.low),Stage(Stage::SIG_NATIVE_MODE,fixCarry,&pc.high,&SIGN_EXTENDED_OP_HALF_ADD)}, //The following stages are those to be executed if the branch is taken
                                {Stage(Stage::SIG_PC_CROSS_PAGE_IN_EMUL,dummyFetchLast),Stage(Stage::SIG_PC_CROSS_PAGE_IN_EMUL,fixCarry,&pc.high,&SIGN_EXTENDED_OP_HALF_ADD)}, //TODO: remove the sig_always on this line ?? (cycle accuracy of BNE (D0)
                                {Stage(Stage::SIG_DUMMY_STAGE, dummyStage)}});
    RelativeBranch.setSignals({bind(incPC,this,1)});
    RelativeBranch.setPredecodeSignals({bind(branchInstruction,this)});


    RelativeBranchLong.setStages({  {Stage(Stage::SIG_ALWAYS,fetchInc,&pc,&adr.high)},
                                    {Stage(Stage::SIG_ALWAYS,fullAdd,&pc,&adr)},
                                    {Stage(Stage::SIG_INST, dummyStage)}});
    RelativeBranchLong.setSignals({bind(incPC,this,1)});


    StackPop.setStages({{Stage(Stage::SIG_DUMMY_STAGE,dummyStage)}, //The inc is supposed to happen here but I do it all in the "pop" operation
                        {Stage(Stage::SIG_ALWAYS,pop,&idb.low)},
                        {Stage(Stage::SIG_MODE16_ONLY,pop,&idb.high)},
                        {Stage(Stage::SIG_INST,dummyStage)}});
    StackPop.setPredecodeSignals({bind(invalidPrefetch,this)});


    StackPop8.setStages({{Stage(Stage::SIG_DUMMY_STAGE,dummyStage)},
                         {Stage(Stage::SIG_ALWAYS,pop,&idb.low)},
                         {Stage(Stage::SIG_INST,dummyStage)}});
    StackPop8.setPredecodeSignals({bind(invalidPrefetch,this)});


    StackPop16.setStages({  {Stage(Stage::SIG_DUMMY_STAGE,dummyStage)},
                            {Stage(Stage::SIG_ALWAYS,pop,&idb.low)},
                            {Stage(Stage::SIG_ALWAYS,pop,&idb.high)},
                            {Stage(Stage::SIG_INST,dummyStage)}});
    StackPop16.setPredecodeSignals({bind(invalidPrefetch,this)});


    StackPush.setStages({   {Stage(Stage::SIG_INST,dummyStage),Stage(Stage::SIG_MODE16_ONLY,push,&idb.high)},
                            {Stage(Stage::SIG_ALWAYS,push,&idb.low)},
                            {Stage(Stage::SIG_DUMMY_STAGE,dummyStage)}});
    StackPush.setPredecodeSignals({bind(invalidPrefetch,this)});


    StackPush8.setStages({  {Stage(Stage::SIG_INST,dummyStage),Stage(Stage::SIG_ALWAYS,push,&idb.low)},
                            {Stage(Stage::SIG_DUMMY_STAGE,dummyStage)}});
    StackPush8.setPredecodeSignals({bind(invalidPrefetch,this)});


    StackPush16.setStages({ {Stage(Stage::SIG_INST,dummyStage),Stage(Stage::SIG_ALWAYS,push,&idb.high)},
                            {Stage(Stage::SIG_ALWAYS,push,&idb.low)},
                            {Stage(Stage::SIG_DUMMY_STAGE,dummyStage)}});
    StackPush16.setPredecodeSignals({bind(invalidPrefetch,this)});


    StackPEA.setStages({{Stage(Stage::SIG_ALWAYS,fetchInc,&pc,&adr.high)},
                        {Stage(Stage::SIG_ALWAYS,push,&adr.high)},
                        {Stage(Stage::SIG_ALWAYS,push,&adr.low)},
                        {Stage(Stage::SIG_INST,dummyStage)}});
    StackPEA.setSignals({bind(incPC,this,1)});


    StackPEI.setStages({{Stage(Stage::SIG_DL_NOT_ZERO,dummyFetchLast),Stage(Stage::SIG_DL_NOT_ZERO,halfAdd,&adr.low,&d.low),Stage(Stage::SIG_DL_NOT_ZERO,fixCarry,&adr.high,&ZERO)},
                        {Stage(Stage::SIG_ALWAYS,fetchInc,&adr,&idb.low)},
                        {Stage(Stage::SIG_ALWAYS,fetch,&adr,&idb.high)},
                        {Stage(Stage::SIG_ALWAYS,push,&idb.high)},
                        {Stage(Stage::SIG_ALWAYS,push,&idb.low)},
                        {Stage(Stage::SIG_INST,dummyStage)}});
    StackPEI.setSignals({bind(incPC,this,1),bind(dhPrefetchInAdr,this)});


    StackPER.setStages({{Stage(Stage::SIG_ALWAYS,fetchInc,&pc,&adr.high)},
                        {Stage(Stage::SIG_ALWAYS,dummyFetchLast),Stage(Stage::SIG_ALWAYS,fullAdd,&adr,&pc)},
                        {Stage(Stage::SIG_ALWAYS,push,&adr.high)},
                        {Stage(Stage::SIG_ALWAYS,push,&adr.low)},
                        {Stage(Stage::SIG_INST,dummyStage)}});
    StackPER.setSignals({bind(incPC,this,1)});


    StackRTI.setStages({{Stage(Stage::SIG_ALWAYS,dummyFetchLast)},// Stage(Stage::SIG_ALWAYS,enableInterupts,true)}, //TODO: Re enable interupts where ? maybe no need to do it as it is saved in P
                        {Stage(Stage::SIG_ALWAYS,popP)},
                        {Stage(Stage::SIG_ALWAYS,pop,&pc.low)},
                        {Stage(Stage::SIG_ALWAYS,pop,&pc.high)},
                        {Stage(Stage::SIG_NATIVE_MODE,pop,&pbr)},
                        {Stage(Stage::SIG_INST,dummyStage)}});
    StackRTI.setPredecodeSignals({bind(invalidPrefetch,this)});


    StackRTS.setStages({{Stage(Stage::SIG_ALWAYS,dummyFetchLast)},
                        {Stage(Stage::SIG_ALWAYS,pop,&pc.low)},
                        {Stage(Stage::SIG_ALWAYS,pop,&pc.high)},
                        {Stage(Stage::SIG_ALWAYS,dummyFetchLast),Stage(Stage::SIG_ALWAYS,incReg,&pc)},
                        {Stage(Stage::SIG_INST,dummyStage)}});
    StackRTS.setPredecodeSignals({bind(invalidPrefetch,this)});


    StackRTL.setStages({{Stage(Stage::SIG_ALWAYS,dummyFetchLast)},
                        {Stage(Stage::SIG_ALWAYS,pop,&pc.low)},
                        {Stage(Stage::SIG_ALWAYS,pop,&pc.high)},
                        {Stage(Stage::SIG_ALWAYS,pop,&pbr),Stage(Stage::SIG_ALWAYS,incReg,&pc)},
                        {Stage(Stage::SIG_INST,dummyStage)}});
    StackRTL.setPredecodeSignals({bind(invalidPrefetch,this)});


    StackRelative.setStages({   {Stage(Stage::SIG_ALWAYS,dummyFetchLast),Stage(Stage::SIG_ALWAYS,moveReg8,&ZERO,&adr.high),Stage(Stage::SIG_ALWAYS,fullAdd,&adr,&s)},
                                {Stage(Stage::SIG_ALWAYS,fetchIncLong,&ZERO,&adr,&idb.low)},
                                {Stage(Stage::SIG_MODE16_ONLY,fetchLong,&ZERO,&adr,&idb.high)},
                                {Stage(Stage::SIG_INST,dummyStage)}});
    StackRelative.setSignals({bind(incPC,this,1)});


    StackRelativeWrite.setStages({  {Stage(Stage::SIG_ALWAYS,dummyFetchLast),Stage(Stage::SIG_ALWAYS,moveReg8,&ZERO,&adr.high),Stage(Stage::SIG_ALWAYS,fullAdd,&adr,&s)},
                                    {Stage(Stage::SIG_INST,dummyStage),Stage(Stage::SIG_ALWAYS,writeIncLong,&ZERO,&adr,&idb.low)},
                                    {Stage(Stage::SIG_MODE16_ONLY,writeLong,&ZERO,&adr,&idb.high)},
                                    {Stage(Stage::SIG_DUMMY_STAGE,dummyStage)}});
    StackRelativeWrite.setSignals({bind(incPC,this,1)});


    StackRelativeIndirectY.setStages({  {Stage(Stage::SIG_ALWAYS,dummyFetchLast),Stage(Stage::SIG_ALWAYS,moveReg8,&ZERO,&adr.high),Stage(Stage::SIG_ALWAYS,fullAdd,&adr,&s)},
                                        {Stage(Stage::SIG_ALWAYS,fetchIncLong,&ZERO,&adr,&idb.low)},
                                        {Stage(Stage::SIG_ALWAYS,fetchLong,&ZERO,&adr,&idb.high)},
                                        {Stage(Stage::SIG_ALWAYS,dummyFetchLast),Stage(Stage::SIG_ALWAYS,fullAdd,&idb,&y)},
                                        {Stage(Stage::SIG_ALWAYS,moveReg16,&idb,&adr),Stage(Stage::SIG_ALWAYS,fetchInc,&adr,&idb.low)},
                                        {Stage(Stage::SIG_MODE16_ONLY,fetch,&adr,&idb.high)},
                                        {Stage(Stage::SIG_INST,dummyStage)}});
    StackRelativeIndirectY.setSignals({bind(incPC,this,1)});


    StackRelativeIndirectYWrite.setStages({ {Stage(Stage::SIG_ALWAYS,dummyFetchLast),Stage(Stage::SIG_ALWAYS,moveReg8,&ZERO,&adr.high),Stage(Stage::SIG_ALWAYS,fullAdd,&adr,&s)},
                                            {Stage(Stage::SIG_ALWAYS,fetchIncLong,&ZERO,&adr,&idb.low)},
                                            {Stage(Stage::SIG_ALWAYS,fetchLong,&ZERO,&adr,&idb.high)},
                                            {Stage(Stage::SIG_ALWAYS,dummyFetchLast),Stage(Stage::SIG_ALWAYS,fullAdd,&idb,&y)},
                                            {Stage(Stage::SIG_ALWAYS,moveReg16,&idb,&adr),Stage(Stage::SIG_INST,dummyStage),Stage(Stage::SIG_ALWAYS,writeInc,&adr,&idb.low)},
                                            {Stage(Stage::SIG_MODE16_ONLY,write,&adr,&idb.high)},
                                            {Stage(Stage::SIG_DUMMY_STAGE,dummyStage)}});
    StackRelativeIndirectYWrite.setSignals({bind(incPC,this,1)});


    StackInterupt.setStages({   {Stage(Stage::SIG_NATIVE_MODE,push,&pbr)},
                                {Stage(Stage::SIG_ALWAYS,push,&pc.high)},
                                {Stage(Stage::SIG_ALWAYS,push,&pc.low)},
                                {Stage(Stage::SIG_ALWAYS,pushP),Stage(Stage::SIG_ALWAYS,enableInterupts,false),Stage(Stage::SIG_INST,dummyStage)},//TODO: moved the instruction stage here (might cause problems ??)
                                {Stage(Stage::SIG_ALWAYS,fetchIncLong,&ZERO,&adr,&pc.low),Stage(Stage::SIG_ALWAYS,moveReg8,&ZERO,&pbr)}, //TODO: Vector Pull signal here(and next line)
                                {Stage(Stage::SIG_ALWAYS,fetchLong,&ZERO,&adr,&pc.high)},
                                {Stage(Stage::SIG_DUMMY_STAGE,dummyStage)}});
    StackInterupt.setSignals({bind(incPC,this,1)});


    BlockMoveN.setStages({  {Stage(Stage::SIG_ALWAYS,fetchDec,&pc,&adr.high),Stage(Stage::SIG_ALWAYS,moveReg8,&adr.low,&dbr)},
                            {Stage(Stage::SIG_ALWAYS,fetchIncLong,&adr.high,&x,&idb.low)},
                            {Stage(Stage::SIG_ALWAYS,writeIncLong,&adr.low,&y,&idb.low)},
                            {Stage(Stage::SIG_ALWAYS,dummyFetchLast),Stage(Stage::SIG_ALWAYS,decReg,&acc),Stage(Stage::SIG_ACC_ZERO,incReg,&pc)},
                            {Stage(Stage::SIG_ACC_ZERO,incReg,&pc),Stage(Stage::SIG_ACC_NOT_ZERO,decReg,&pc)},
                            {Stage(Stage::SIG_INST,dummyStage)}});
    BlockMoveN.setSignals({bind(incPC,this,1)});


    BlockMoveP.setStages({  {Stage(Stage::SIG_ALWAYS,fetchDec,&pc,&adr.high),Stage(Stage::SIG_ALWAYS,moveReg8,&adr.low,&dbr)},
                            {Stage(Stage::SIG_ALWAYS,fetchDecLong,&adr.high,&x,&idb.low)},
                            {Stage(Stage::SIG_ALWAYS,writeDecLong,&adr.low,&y,&idb.low)},
                            {Stage(Stage::SIG_ALWAYS,dummyFetchLast),Stage(Stage::SIG_ALWAYS,decReg,&acc),Stage(Stage::SIG_ACC_ZERO,incReg,&pc)},
                            {Stage(Stage::SIG_ACC_ZERO,incReg,&pc),Stage(Stage::SIG_ACC_NOT_ZERO,decReg,&pc)},
                            {Stage(Stage::SIG_INST,dummyStage)}});
    BlockMoveP.setSignals({bind(incPC,this,1)});
}
