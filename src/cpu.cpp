#include "cpu.h"

// Used for printing hex
#include  <iomanip>

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
    
    this->cb_state = false;
    
    this->interupt_state = this->INTERUPT_STATE::DISABLED;
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

    // If interupt state is either enabled or pending disable,
    // check interupts
    if (this->interupt_state == this->INTERUPT_STATE::ENABLED ||
        this->interupt_state == this->INTERUPT_STATE::PENDING_DISABLE)
        this->check_interupts();
    
    // Check if interupt state is in a pending state
    // and move to actual state, since a clock cycle has been waited
    if (this->interupt_state == this->INTERUPT_STATE::PENDING_DISABLE)
        this->interupt_state = this->INTERUPT_STATE::DISABLED;
    if (this->interupt_state == this->INTERUPT_STATE::PENDING_ENABLE)
        this->interupt_state = this->INTERUPT_STATE::ENABLED;

    if (this->halt_state)
        return;

    // Read value from memory
    int op_val = (int)this->ram.get_val(this->r_pc.value);
    
    // Increment program counter
    // @TODO: Verify if this needs to be moved.. does it need to happen
    // after the operation is executed, i.e if we perform an operation with
    // the PC, this will matter.
    this->r_pc.value ++;

    if (this->cb_state) {
        this->execute_cb_code(op_val);
        this->cb_state = false;
    } else {
        this->execute_op_code(op_val);
    }
}

void CPU::check_interupts() {
    
}

void CPU::execute_op_code(int op_val) {
    switch(op_val) {
        case 0x0:
            // STOP
            //this->op_Noop();
            break;
        case 0x0e:
            // Load byte into C
            this->op_Load(this->r_c);
            break;
        case 0x10:
            //this->op_Stop();
            // @TODO: This is TEMPORARY
            //this->running = false;
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
            break;
        case 0x3e:
            this->op_Load(this->r_a);
            break;
        case 0x4f:
            this->op_Load(this->r_a, this->r_c);
            break;
        case 0x76:
            this->op_Halt();
            break;
        case 0xaf:
            // X-OR A with A into A
            this->op_XOR(this->r_a);
            break;
        case 0xcb:
            // Set flag for CB
            this->cb_state = true;
            break;
        case 0xe2:
            this->op_Load(this->r_a, this->r_c);
            break;
        case 0xf3:
            // Disable interupts
            this->op_DI();
            break;
        case 0xfb:
            // Enable interupts
            this->op_EI();
            break;
        default:
            std::cout << "Unknown op code: 0x";
            std::cout << std::setfill('0') << std::setw(2) << std::hex << op_val;
            std::cout << std::endl;
            break;
    }
}

void CPU::execute_cb_code(int op_val) {
    switch(op_val) {
        case 0x7c:
            this->op_Bit(this->r_h, 7);
            break;
        default:
            std::cout << "Unknown CB op code: ";
            std::cout << std::hex << op_val;
            std::cout << std::endl;
            break;
    }
}

uint16_t CPU::get_register_value16(reg8 dest_l, reg8 dest_u) {
    union {
        uint8_t bit8[2];
        uint16_t bit16[1];
    } data_conv;
    data_conv.bit8[0] = dest_l.value;
    data_conv.bit8[1] = dest_u.value;

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

void CPU::set_register_bit(reg8 source, uint8_t bit_shift, unsigned char val) {
    if (val == 1) {
        source.value |= (1UL << bit_shift);
    } else {
        // Create 0 mask
        //unsigned char mask = 0;
        // Flip all bits
        //mask ^= 1UL;
        //mask &= 0 << bit_shift;
        //source.value &= mask;
        source.value ^= (1UL << bit_shift);
    }
}
unsigned char CPU::get_register_bit(reg8 source, uint8_t bit_shift) {
    return (source.value & (1U  << bit_shift));
}

void CPU::set_zero_flag(uint8_t is_it) {
    this->r_f.value ^= (((is_it == 0) ? 1UL : 0UL) << ZERO_FLAG_BIT);
}

// Get value from memory at PC and increment PC
uint8_t CPU::get_inc_pc_val8() {
    uint8_t ori_val = this->ram.get_val(this->r_pc.value);
    uint8_t val;
    memcpy(&val, &ori_val, 1);
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


void CPU::op_Bit(reg8 comp, int bit) {
    // Set flags accordinly before operation
    //this->r_f.value |= (1UL << )
    this->set_register_bit(this->r_f, this->ZERO_FLAG_BIT,
                           this->get_register_bit(comp, bit));
}

// Op code
// @TODO: Move these
void CPU::op_Load(reg8 dest) {
    dest.value = this->get_inc_pc_val8();
}
void CPU::op_Load(reg16 dest) {
    dest.value = this->get_inc_pc_val16();
}
void CPU::op_Load(reg8 source, reg8 dest) {
    mempcpy(&dest.value, &source.value, 1);
}

void CPU::op_Halt() {
    // Set DI state to pending, e.g.
    // perform one more command before halting
    this->halt_state = true;
}

void CPU::op_EI() {
    // Set DI state to pending, e.g.
    // perform one more command before halting
    this->interupt_state = this->INTERUPT_STATE::PENDING_ENABLE;
}
void CPU::op_DI() {
    // Set DI state to pending, e.g.
    // perform one more command before halting
    this->interupt_state = this->INTERUPT_STATE::PENDING_DISABLE;
}