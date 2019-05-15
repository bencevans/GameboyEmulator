#include "cpu.h"

Cpu::Cpu(RAM ram) {
    this->r_a = 0;
    this->r_f = 0;
    this->r_b = 0;
    this->r_c = 0;
    this->r_d = 0;
    this->r_e = 0;
    this->r_h = 0;
    this->r_l = 0;
    
    // Stack pointer
    this->r_sp = 0;
    // Program counter
    this->r_pc = 0;
}