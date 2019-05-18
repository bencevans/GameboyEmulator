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
    
    this->r_bc.lower = this->r_b;
    this->r_bc.upper = this->r_c;
    this->r_de.lower = this->r_d;
    this->r_de.upper = this->r_e;
    this->r_hl.lower = this->r_h;
    this->r_hl.upper = this->r_l;
    
    this->cb_state = false;
    
    this->interupt_state = this->INTERUPT_STATE::DISABLED;
    this->halt_state = false;
    
    // Stack pointer
    this->r_sp = stack_pointer();
    this->r_sp.value = 0xfffe;
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
    std::cout << this->r_pc.value << std::endl;
    
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
        case 0x0a:
            this->op_Load(this->r_a, 0xff00 + (this->get_register_value16(this->r_bc)));
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
        case 0x11:
            this->op_Load(this->r_de);
            break;
        case 0x1a:
            this->op_Load(this->r_a, 0xff00 + (this->get_register_value16(this->r_de)));
            break;
        case 0x21:
            this->op_Load(this->r_hl);
            break;
        case 0x31:
            // Load 2-bytes into SP
            this->op_Load(this->r_sp);
            break;
        case 0x32:
            // Get HL, dec and set
            this->op_Get_dec_set(this->r_hl, this->r_a);
            break;
        case 0x3e:
            this->op_Load(this->r_a);
            break;
        case 0x4f:
            this->op_Load(this->r_c, this->r_a);
            break;
        case 0x76:
            this->op_Halt();
            break;
        case 0x77:
            this->op_Load(0xff00 + (this->get_register_value16(this->r_hl)), this->r_a);
            break;
        case 0xaf:
            // X-OR A with A into A
            this->op_XOR(this->r_a);
            break;
        case 0xc9:
            this->op_Return();
            break;
        case 0xcb:
            // Set flag for CB
            this->cb_state = true;
            break;
        case 0xcd:
            this->op_Call();
            break;
        case 0xce:
            this->op_Adc();
            break;
        case 0xe0:
            this->op_Load(0xff00 + this->get_inc_pc_val8(), this->r_a);
            break;
        case 0xe2:
            this->op_Load(0xff00 + this->r_c.value, this->r_a);
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
        case 0xcd:
            this->op_Set(1, this->r_l);
            break;
        default:
            std::cout << "Unknown CB op code: ";
            std::cout << std::hex << op_val;
            std::cout << std::endl;
            break;
    }
}

uint16_t CPU::get_register_value16(combined_reg dest) {
    union {
        uint8_t bit8[2];
        uint16_t bit16[1];
    } data_conv;
    data_conv.bit8[0] = dest.lower.value;
    data_conv.bit8[1] = dest.upper.value;

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
    return (source.value & (1U  << bit_shift)) >> bit_shift;
}

void CPU::set_zero_flag(uint8_t is_it) {
    this->r_f.value ^= (((is_it == (uint8_t)0x0) ? 1UL : 0UL) << ZERO_FLAG_BIT);
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
void CPU::op_Get_dec_set(combined_reg dest, reg8 source) {
    uint8_t value;
    memcpy(&value, &source.value, 1);
    uint16_t register_value16 = this->get_register_value16(dest);
    this->ram.set(register_value16, value);
    this->ram.dec(register_value16);
}


void CPU::op_Bit(reg8 comp, int bit) {
    // Set flags accordinly before operation
    //this->r_f.value |= (1UL << )
    this->set_register_bit(this->r_f, this->ZERO_FLAG_BIT,
                           this->get_register_bit(comp, bit));
}

// Set single bit in a given register
void CPU::op_Set(uint8_t bit, reg8 dest) {
    this->set_register_bit(dest, bit, 1);
}

// Add 8bit PC value and carry flag to A.
void CPU::op_Adc() {
    union {
        uint8_t bit8[2];
        uint16_t bit16[1];
    } data_conv;

    // Always work with r_a
    data_conv.bit8[0] = this->get_inc_pc_val8();
    data_conv.bit16[0] += this->r_a.value;
    this->r_a.value = data_conv.bit8[0];

    // Set carry flag, based on 1st bit of second byte
    uint8_t carry_flag = (0x01 & data_conv.bit8[1]) >> 0;
    this->set_register_bit(this->r_f, this->CARRY_FLAG_BIT, carry_flag);

    // Set zero flag
    this->set_zero_flag(this->r_a.value);

    // Set subtract flag to 0
    this->set_register_bit(this->r_f, this->SUBTRACT_FLAG_BIT, 0L);

    // Determine half carry flag based on 5th bit of first byte
    uint8_t h_carry_flag = (0x10 & data_conv.bit8[0]) >> 4;
    this->set_register_bit(this->r_f, this->HALF_CARRY_FLAG_BIT, h_carry_flag);
}

// Op code
// @TODO: Move these
void CPU::op_Load(reg8 dest) {
    dest.value = this->get_inc_pc_val8();
}
void CPU::op_Load(combined_reg dest) {
    dest.lower.value = this->get_inc_pc_val8();
    dest.upper.value = this->get_inc_pc_val8();
}
void CPU::op_Load(reg16 dest) {
    dest.value = this->get_inc_pc_val16();
}
// Copy 1 byte between registers
void CPU::op_Load(reg8 dest, reg8 source) {
    mempcpy(&dest.value, &source.value, 1);
}
// Copy register value into destination address of memory
void CPU::op_Load(int dest_addr, reg8 source) {
    this->ram.set(dest_addr, source.value);
}
// Copy data from source memory address to destination
void CPU::op_Load(reg8 dest, int source_addr) {
    dest.value = this->ram.get_val(source_addr);
}

void CPU::op_Call() {
    // Get jump address
    uint16_t jmp_dest_addr = this->get_inc_pc_val16();

    // Push PC (which has already been incremented) to stack
    this->ram.stack_push(this->r_sp.value, this->r_pc.value);
    
    // Set PC to jump destination address
    this->r_pc.value = jmp_dest_addr;
}

void CPU::op_Return() {
    this->ram.stack_pop(this->r_sp.value, this->r_pc.value);
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