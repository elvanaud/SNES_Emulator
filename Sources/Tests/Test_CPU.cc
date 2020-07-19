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
        vector<uint8_t> prog = {0xA9,42}; //LDA #42
        bus.copyInMemory(cpu.getPC(), prog);
        cpu.tick();
        REQUIRE(cpu.VDA()); REQUIRE(cpu.VPA());
        cpu.tick();
        REQUIRE_FALSE(cpu.VDA()); REQUIRE(cpu.VPA());
        cpu.tick();
        REQUIRE(cpu.VDA()); REQUIRE(cpu.VPA());
        REQUIRE(cpu.getAcc() == 42);
    }

    SECTION("Instruction ADC - Immediate")
    {

    }
}

TEST_CASE("CPU Reset", "[!mayfail]") //TODO
{
    REQUIRE(false);
}
