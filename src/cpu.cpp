#include "cpu.h"

#include <iostream>

CPU::CPU(RAM ram) {
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
    
    this->running = true;
    
    this->ram = ram;
}

bool CPU::is_running() {
    return this->running;
}

void CPU::tick() {
    // Read value from memory
    int op_val = (int)this->ram.get(this->r_pc);
    
    switch(op_val) {
        case 0x0:
            // STOP
            this->running = false;
        default:
            std::cout << "Unknown op code: ";
            std::cout << std::hex << op_val;
            std::cout << std::endl;
    }
    
    // Increment program counter
    this->r_pc ++;
    
    //std::cout << "tick";
}