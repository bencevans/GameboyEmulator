#include "cpu.h"

#include <iostream>
#include <string.h>

CPU::CPU(RAM ram) {
    
    this->r_a = accumulator();
    this->r_a.value = 0;
    this->r_f = gen_reg();
    this->r_f.value = 0;
    this->r_b = gen_reg();
    this->r_b.value = 0;
    this->r_c = gen_reg();
    this->r_c.value = 0;
    this->r_d = gen_reg();
    this->r_d.value = 0;
    this->r_e = gen_reg();
    this->r_e.value = 0;
    this->r_h = gen_reg();
    this->r_h.value = 0;
    this->r_l = gen_reg();
    this->r_l.value = 0;
    
    this->di_state = this->INTERUPT_STATE::DEACTIVE;
    this->halt_state = false;
    
    // Stack pointer
    this->r_sp = stack_pointer();
    this->r_sp.value = 0;
    // Program counter
    this->r_pc = program_counter();
    this->r_pc.value = 0;
    
    this->running = true;
    
    this->ram = ram;
}

bool CPU::is_running() {
    return this->running;
}

void CPU::tick() {
    if (this->halt_state)
        return;

    // Read value from memory
    int op_val = (int)this->ram.get(this->r_pc.value);
    
    // Increment program counter
    // @TODO: Verify if this needs to be moved.. does it need to happen
    // after the operation is executed, i.e if we perform an operation with
    // the PC, this will matter.
    this->r_pc.value ++;
    
    switch(op_val) {
        case 0x0:
            // STOP
            //this->op_Noop();
            break;
        case 0x10:
            //this->op_Stop();
            // @TODO: This is TEMPORARY
            this->running = false;
            break;
        case 0x21:
            // First byte into H
            this->op_Load(this->r_h);
            // Second into L
            this->op_Load(this->r_l);
            break;
        case 0x31:
            // Load 2-bytes into SP
            this->op_Load(this->r_sp);
            break;
        case 0x32:
            // Get HL, dec and set
            this->op_Get_dec_set(this->r_a, this->r_h, this->r_l);
        case 0x76:
            this->op_Halt();
            break;
        case 0xaf:
            // X-OR A with A into A
            this->op_XOR(this->r_a);
            break;
        default:
            std::cout << "Unknown op code: ";
            std::cout << std::hex << op_val;
            std::cout << std::endl;
    }
}


uint16_t CPU::get_register_value16(reg8 dest_l, reg8 dest_u) {
    union {
        uint8_t bit8[2];
        uint16_t bit16[1];
    } data_conv;
    data_conv.bit8[0] = dest_l.value;
    data_conv.bit8[1] = dest_l.value;
    return data_conv.bit16[0];
}

// Perform XOR of registry against A and then store
// result in A
void CPU::op_XOR(reg8 comp) {
    uint8_t res = this->r_a.value ^ comp.value;
    this->r_a.value = res;
    this->r_f.value = 0;
    this->set_zero_flag(res);
}

void CPU::set_zero_flag(uint8_t is_it) {
    this->r_f.value ^= (((is_it == 0) ? 1UL : 0UL) << 7);
}

// Get value from memory at PC and increment PC
uint8_t CPU::get_inc_pc_val8() {
    uint8_t val = this->ram.get(this->r_pc.value);
    this->r_pc.value ++;
    return val;
}

// Get 2-byte value from memory address at PC,
// incrementing the PC past this value
uint16_t CPU::get_inc_pc_val16() {
    union {
        uint8_t bit8[2];
        uint16_t bit16[1];
    } data_conv;
    uint16_t tmp;
    for (int itx = 0; itx <= 1; itx ++) {
        tmp = this->get_inc_pc_val8();
        memcpy(&data_conv.bit8[itx], &tmp, 1);
    }
    //memcpy(&data_conv.bit8[0], this->get_inc_pc_val8());
    //memcpy(&data_conv.bit8[1], this->get_inc_pc_val8());
    return data_conv.bit16[0];
}

// Get value from specified register, decrement and store
// in memory (using address of two registers)
void CPU::op_Get_dec_set(reg8 source, reg8 dest_l, reg8 dest_h) {
    uint8_t value;
    memcpy(&value, &source.value, 1);
    this->ram.set(this->get_register_value16(dest_l, dest_h), value);
}


// Op code
// @TODO: Move these
void CPU::op_Load(reg8 dest) {
    dest.value = this->get_inc_pc_val8();
}
void CPU::op_Load(reg16 dest) {
    dest.value = this->get_inc_pc_val16();
}

void CPU::op_Halt() {
    // Set DI state to pending, e.g.
    // perform one more command before halting
    this->halt_state = true;
}

void CPU::op_DI() {
    // Set DI state to pending, e.g.
    // perform one more command before halting
    this->di_state = INTERUPT_STATE::PENDING;
}