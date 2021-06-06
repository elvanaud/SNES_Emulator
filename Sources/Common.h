#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <cstdint>
#include <cassert>
using std::string;
using std::cout;
using std::cin;
using std::endl;

const unsigned int BANK_SIZE = 0x1'00'00;
const unsigned int HALF_BANK_SIZE = BANK_SIZE / 2;
const unsigned int MAX_BANKS = 0x80;

#endif // COMMON_H
