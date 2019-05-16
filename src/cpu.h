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
        DEACTIVE,
        PENDING,
        ACTIVE
    };
    INTERUPT_STATE di_state;
    bool halt_state;

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
    

    void op_Load(reg8 dest);
    void op_Load(reg16 dest);
    void op_XOR(reg8 comp);
    void op_Get_dec_set(reg8 source, reg8 dest_l, reg8 dest_h);
    void op_DI();
    void op_Halt();
    
    RAM ram;
    bool running;
};