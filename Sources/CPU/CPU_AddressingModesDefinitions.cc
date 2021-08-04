#include "W65816.h"

using std::bind;
using namespace std::placeholders;

void W65816::initializeAddressingModes()
{
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
