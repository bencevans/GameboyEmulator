#pragma once

#include <memory>
#include "ram.h"

class CPU {
public:
    CPU(RAM ram);
    void tick();
    bool is_running();
private:
    // Registers
    //  Accumulator
    uint8_t r_a;
    //
    uint8_t r_f;
    // General purpose
    uint8_t r_b;
    uint8_t r_c;
    uint8_t r_d;
    uint8_t r_e;
    uint8_t r_h;
    uint8_t r_l;
    
    // Stack pointer
    uint16_t r_sp;
    // Program counter
    uint16_t r_pc;
    
    RAM ram;
    bool running;
};