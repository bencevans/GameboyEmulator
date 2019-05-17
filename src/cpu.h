#pragma once

#include <memory>
#include "ram.h"

// Any register
struct reg { };

// 8-bit register
struct reg8 : reg {
    uint8_t value;
};
// 16-bit register
struct reg16 : reg {
    uint16_t value;
};

// Accumulator
struct accumulator : reg8 { };
// General Register
struct gen_reg : reg8 { };
// Stack pointer
struct stack_pointer : reg16 { };
// Program counter
struct program_counter : reg16 { };

class CPU {

public:
    CPU(RAM ram);
    void tick();
    bool is_running();

private:
    enum INTERUPT_STATE {
        DISABLED,
        PENDING_DISABLE,
        PENDING_ENABLE,
        ENABLED
    };
    INTERUPT_STATE interupt_state;
    bool halt_state;
    bool cb_state;
    
    // Bits for flag register
    const int CARRY_FLAG_BIT = 4;
    const int HALF_CARRY_FLAG_BIT = 5;
    const int SUBTRACT_FLAG_BIT = 6;
    const int ZERO_FLAG_BIT = 7;

    // Registers
    //  Accumulator
    accumulator r_a;
    //
    gen_reg r_f;
    // General purpose
    gen_reg r_b;
    gen_reg r_c;
    gen_reg r_d;
    gen_reg r_e;
    gen_reg r_h;
    gen_reg r_l;
    
    // Stack pointer
    stack_pointer r_sp;
    // Program counter
    program_counter r_pc;

    uint8_t get_inc_pc_val8();
    uint16_t get_inc_pc_val16();
    uint16_t get_register_value16(reg8 dest_l, reg8 dest_u);
    void set_register_bit(reg8 source, uint8_t bit_shift, unsigned char val);
    unsigned char get_register_bit(reg8 source, uint8_t bit_shift);
    
    void execute_op_code(int op_val);
    void execute_cb_code(int op_val);
    
    void check_interupts();
    
    void set_zero_flag(uint8_t is_it);

    void op_Load(reg8 dest);
    void op_Load(reg16 dest);
    void op_Load(reg8 source, reg8 dest);
    void op_XOR(reg8 comp);
    void op_Get_dec_set(reg8 source, reg8 dest_l, reg8 dest_h);
    void op_Bit(reg8 comp, int bit);
    void op_EI();
    void op_DI();
    void op_Halt();
    
    RAM ram;
    bool running;
};