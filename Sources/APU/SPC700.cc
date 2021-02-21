#include "SPC700.h"

#include "SNES_APU.h"

SPC700::SPC700()
{
    //ctor
}

void SPC700::attachBus(SNES_APU * bus)
{
    apu_bus = bus;
}

uint8_t SPC700::read(uint16_t adr)
{
    return apu_bus->read(adr);
}

void SPC700::write(uint16_t adr, uint8_t data)
{
    apu_bus->write(adr, data);
}

void SPC700::reset()
{
    pc = read(0xFFFE);
    pc |= (uint16_t(read(0xFFFF))<<8);

    a = x = y = sp = psw.val = 0;
    idb8_ext = 0;
    idb8 = 0;
    inst_cycles = inst_length = currentCycle = 0;
}

void SPC700::updateNZflags(uint8_t a)
{
    psw.setN((a>>7)&1);
    psw.setZ(a == 0);
}

void SPC700::updateNZflags(uint8_t high, uint8_t a)
{
    psw.setN((high>>7)&1);
    psw.setZ(a == 0 && high == 0);
}

void SPC700::tick()
{
    if(currentCycle != 0) //Waiting for end of current instruction
    {
        --currentCycle;
        return;
    }

    //Decoding
    inst_cycles = 0;
    inst_length = 0;

    uint8_t opcode = read(pc);

    if(false) //Manual decode
    {

    }
    else
    {
        switch(opcode)
        {
        case 0xE8: Immediate(MOVA);         break;
        case 0xCD: Immediate(MOVX);         break;
        case 0x8D: Immediate(MOVY);         break;
        case 0x7D: Implied(MOVAX);          break;
        case 0x5D: Implied(MOVXA);          break;
        case 0xDD: Implied(MOVAY);          break;
        case 0xFD: Implied(MOVYA);          break;
        case 0x9D: Implied(MOVXSP);         break;
        case 0xBD: Implied(MOVSPX);         break;
        case 0xE4: Direct(MOVA);            break;
        case 0xF4: DirectIndexedX(MOVA);    break;
        case 0xE5: Absolute(MOVA);          break;
        case 0xF5: AbsoluteIndexedX(MOVA);  break;
        case 0xF6: AbsoluteIndexedY(MOVA);  break;
        case 0xE6: DirectX(MOVA);           break;
        case 0xBF: DirectXInc(MOVA);        break;
        case 0xF7: DirectIndirectIndexed(MOVA);         break;
        case 0xE7: DirectIndexedIndirect(MOVA);         break;
        case 0xF8: Direct(MOVX);                        break;
        case 0xF9: DirectIndexedY(MOVX);                break;
        case 0xE9: Absolute(MOVX);                      break;
        case 0xEB: Direct(MOVY);                        break;
        case 0xFB: DirectIndexedX(MOVY);                break;
        case 0xEC: Absolute(MOVY);                      break;
        case 0xBA: Direct16(MOVW_YA);                   break;
        default:
            cout<<"[APU][SPC700] error: unknown opcode:"<<std::hex<<(int)opcode<<endl;
        }
    }


    pc += inst_length;
    currentCycle = inst_cycles;
}
