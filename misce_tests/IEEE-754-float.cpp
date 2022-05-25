#include <stdio.h>
#include <bitset>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    std::bitset<32> flt(std::string("00111111100000000000000000000000"));
    std::cout << flt << ": " << *(float*)(&flt) << std::endl;
    uint32_t bin = 0b0'01111111'000'0000'0000'0000'0000'0000;
    uint32_t bin1 = 0b0'00000000'000'0000'0000'0000'0000'0001;
    uint32_t bin2 = 0b0'00000000'100'0000'0000'0000'0000'0000;
    std::cout << *(float*)(&bin2) << std::endl;
    uint32_t a = 0x12345678;
    uint8_t* b = (uint8_t*)&a;
    std::cout << std::hex << a << std::endl;
    // std::cout << std::hex << b[0] << b[1] << b[2] << b[3] << std::endl;
    printf("%x %x %x %x\n", b[0], b[1], b[2], b[3]);
    return 0;
}
