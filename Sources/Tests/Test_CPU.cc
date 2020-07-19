#include "catch.hpp"

#include <vector>
using std::vector;

#include "CPU/W65816.h"
#include "Bus.h"


TEST_CASE("CPU works", "[cpu]")
{
    //TODO: Maybe use METHOD_AS_TEST_CASE to have the common to all test cases cpu and bus instantiation...
    W65816 cpu;
    Bus bus(cpu);
    cpu.attachBus(&bus);

    REQUIRE(cpu.E()); //Check emulation mode
    REQUIRE(cpu.M());
    REQUIRE(cpu.X());

    SECTION("SEC and CLC (Implied)")
    {
        uint16_t oldPC = cpu.getPC();
        vector<uint8_t> prog = {0x38, 0x18, 0x38}; //Set C then Reset it then set it again
        bus.copyInMemory(cpu.getPC(),prog);
        cpu.tick();
        REQUIRE(cpu.VDA()); REQUIRE(cpu.VPA()); //Opcode fetch
        cpu.tick();
        REQUIRE_FALSE(cpu.VDA()); REQUIRE_FALSE(cpu.VPA()); //Decode
        oldPC += 1;
        REQUIRE(cpu.getAddressBus() == oldPC);

        oldPC = cpu.getPC();
        cpu.tick();
        REQUIRE(cpu.VDA()); REQUIRE(cpu.VPA()); //Next opcode fetch and effective instruction
        uint8_t cFlag = cpu.getP()&1;
        REQUIRE(cFlag == 1);
        cpu.tick();
        REQUIRE_FALSE(cpu.VDA()); REQUIRE_FALSE(cpu.VPA()); //Decode
        oldPC += 1;
        REQUIRE(cpu.getAddressBus() == oldPC);

        cpu.tick();
        REQUIRE(cpu.VDA()); REQUIRE(cpu.VPA()); //Opcode fetch
        cFlag = cpu.getP() & 1;
        REQUIRE(cFlag == 0);
        cpu.tick(); cpu.tick();
        cFlag = cpu.getP()&1;
        REQUIRE(cFlag == 1);
    }

    SECTION("XCE")
    {
        vector<uint8_t> prog = {0x18, 0xFB, 0xFB}; //CLC, XCE, XCE
        bus.copyInMemory(cpu.getPC(),prog);
        cpu.tick(); cpu.tick(); //CLC
        cpu.tick();
        REQUIRE(cpu.E()); //Ensures we're in emulation mode before XCE
        uint8_t oldP = cpu.getP();
        cpu.tick(); //XCE
        cpu.tick(); //Next XCE fetch
        uint8_t cFlag = cpu.getP() & 1;

        REQUIRE(cFlag == 1);
        REQUIRE_FALSE(cpu.E());
        REQUIRE(cpu.M());
        REQUIRE(cpu.X());
        oldP |= 0x31;
        REQUIRE(cpu.getP() == oldP);

        cpu.tick(); cpu.tick();
        cFlag = cpu.getP() & 1;
        REQUIRE(cFlag == 0);
        REQUIRE(cpu.E());
    }

    SECTION("REP and SEP")
    {
        uint8_t oldP = cpu.getP();
        uint16_t oldPC = cpu.getPC();
        vector<uint8_t> prog = {0xC2,0xFF, 0xE2,0xFF}; //Reset everything then set everything
        bus.copyInMemory(cpu.getPC(),prog);
        cpu.tick(); cpu.tick(); cpu.tick();

        SECTION("REP #$FF")
        {
            REQUIRE_FALSE(cpu.VDA()); REQUIRE_FALSE(cpu.VPA());
            uint16_t busAdr = cpu.getAddressBus()&0xFFFF;
            oldPC += 1;
            REQUIRE(busAdr == oldPC);
            oldP &= 0x30;
            REQUIRE(cpu.getP() == oldP);
        }


        SECTION("SEP #$FF")
        {
            oldPC = cpu.getPC();
            oldP = cpu.getP();
            cpu.tick(); cpu.tick(); cpu.tick();
            REQUIRE_FALSE(cpu.VDA()); REQUIRE_FALSE(cpu.VPA());
            uint16_t busAdr = cpu.getAddressBus()&0xFFFF;
            oldPC += 1;
            REQUIRE(busAdr == oldPC);
            oldP |= ~0x30;
            REQUIRE(cpu.getP() == oldP);
        }
    }


    SECTION("Instruction LDA - Immediate")
    {
        vector<uint8_t> prog = {0xA9,42, 0xA9,0, 0xA9,0xFF}; //LDA #42, LDA #0, LDA #-1
        bus.copyInMemory(cpu.getPC(), prog);
        cpu.tick();
        REQUIRE(cpu.VDA()); REQUIRE(cpu.VPA());
        cpu.tick();
        REQUIRE_FALSE(cpu.VDA()); REQUIRE(cpu.VPA());
        cpu.tick();
        REQUIRE(cpu.VDA()); REQUIRE(cpu.VPA());
        REQUIRE(cpu.getAcc() == 42);

        cpu.tick(); cpu.tick();
        uint8_t zFlag = (cpu.getP() >> 1)&1;
        REQUIRE(zFlag == 1);

        cpu.tick(); cpu.tick();
        uint8_t nFlag = (cpu.getP() >> 7)&1;
        REQUIRE(nFlag == 1);
    }

    SECTION("LDA 16 bit - Immediate")
    {
        vector<uint8_t> prog = {0x18, 0xFB, 0xC2,0x20, 0xA9,0x0A,0x01, 0xA9,0x00,0xFF, 0xFB, 0xA9,0, 0x38}; //CLC, XCE, REP #$20, LDA #$010A, LDA #$FF00, XCE, LDA #0, SEC
        bus.copyInMemory(cpu.getPC(), prog);
        cpu.tick();cpu.tick(); //CLC
        cpu.tick();cpu.tick(); //XCE
        cpu.tick();cpu.tick();cpu.tick(); //REP
        REQUIRE_FALSE(cpu.M());
        cpu.tick(); cpu.tick(); cpu.tick(); //LDA
        cpu.tick(); //LDA fetch
        REQUIRE(cpu.VDA()); REQUIRE(cpu.VPA());
        REQUIRE(cpu.getAcc() == 0x010A);
        cpu.tick(); cpu.tick();
        cpu.tick(); // XCE fetch
        REQUIRE(cpu.getAcc() == 0xFF00);
        uint8_t nFlag = (cpu.getP()>>7)&1;
        uint8_t zFlag = (cpu.getP()>>1)&1;
        REQUIRE(nFlag == 1); REQUIRE(zFlag == 0);
        cpu.tick(); //XCE decode
        REQUIRE_FALSE(cpu.VDA()); REQUIRE_FALSE(cpu.VPA());
        cpu.tick(); //LDA fetch
        REQUIRE(cpu.E());
        REQUIRE(cpu.M());
        cpu.tick();
        cpu.tick(); //SEC fetch
        REQUIRE(cpu.VDA()); REQUIRE(cpu.VPA());
        uint8_t acc = cpu.getAcc() & 0xFF;
        REQUIRE(acc == 0);
        nFlag = (cpu.getP()>>7)&1;
        zFlag = (cpu.getP()>>1)&1;
        REQUIRE(nFlag == 0); REQUIRE(zFlag == 1);
        cpu.tick(); cpu.tick();
        uint8_t cFlag = cpu.getP() & 1;
        REQUIRE(cFlag == 1);
    }

    SECTION("Instruction ADC 8 bit, Binary- Immediate")
    {
        vector<uint8_t> prog = {
            0x18, //CLC
            0xA9,0xFF, //LDA #$FF (-1)
            0x69,0x01, //ADC #1
            0x69,0xFE, //ADC #-2
            0x69,0xFF, //ADC #-1
            0x69,0x80 //ADC #$80
            };
        bus.copyInMemory(cpu.getPC(), prog);

        cpu.tick(); cpu.tick();  //CLC
        cpu.tick(); cpu.tick(); //LDA -1
        cpu.tick(); //Fetch adc 1
        REQUIRE(cpu.getAcc() == 0xFF);
        uint8_t cFlag = (cpu.getP()>>0)&1;
        uint8_t nFlag = (cpu.getP()>>7)&1;
        uint8_t zFlag = (cpu.getP()>>1)&1;
        uint8_t vFlag = (cpu.getP()>>6)&1;

        REQUIRE(cFlag == 0);
        REQUIRE(nFlag == 1);
        REQUIRE(zFlag == 0);
        REQUIRE(vFlag == 0);

        cpu.tick(); //decode adc 1
        cpu.tick(); //fetch adc -2

        cFlag = (cpu.getP()>>0)&1;
        nFlag = (cpu.getP()>>7)&1;
        zFlag = (cpu.getP()>>1)&1;
        vFlag = (cpu.getP()>>6)&1;

        REQUIRE(cFlag == 1);
        REQUIRE(nFlag == 0);
        REQUIRE(zFlag == 1);
        REQUIRE(vFlag == 0);

        REQUIRE(cpu.getAcc() == 0);

        cpu.tick(); //decode ADC -2
        cpu.tick(); //fetch ADC -1

        cFlag = (cpu.getP()>>0)&1;
        nFlag = (cpu.getP()>>7)&1;
        zFlag = (cpu.getP()>>1)&1;
        vFlag = (cpu.getP()>>6)&1;

        REQUIRE(cFlag == 0);
        REQUIRE(nFlag == 1);
        REQUIRE(zFlag == 0);
        REQUIRE(vFlag == 0);

        REQUIRE(cpu.getAcc() == 0xFF);

        cpu.tick(); //decode adc -1
        cpu.tick(); //fetch adc $80

        cFlag = (cpu.getP()>>0)&1;
        nFlag = (cpu.getP()>>7)&1;
        zFlag = (cpu.getP()>>1)&1;
        vFlag = (cpu.getP()>>6)&1;

        REQUIRE(cFlag == 1);
        REQUIRE(nFlag == 1);
        REQUIRE(zFlag == 0);
        REQUIRE(vFlag == 0);

        REQUIRE(cpu.getAcc() == 0xFE);

        cpu.tick(); cpu.tick(); //adc $80

        cFlag = (cpu.getP()>>0)&1;
        nFlag = (cpu.getP()>>7)&1;
        zFlag = (cpu.getP()>>1)&1;
        vFlag = (cpu.getP()>>6)&1;

        REQUIRE(cFlag == 1);
        REQUIRE(nFlag == 0);
        REQUIRE(zFlag == 0);
        REQUIRE(vFlag == 1);

        REQUIRE(cpu.getAcc() == 0x7F);
    }
}

TEST_CASE("CPU Reset", "[!mayfail]") //TODO
{
    REQUIRE(false);
}
