#pragma once

#include <memory>
#include "ram_subset.h"

// 0x10000
#define MAX_MEM_SIZE 65535

class RAM {
public:
    RAM();
    uint8_t get_val(uint16_t address);
    uint8_t get_val(int address);
    uint8_t* get_ref(uint16_t a);
    void set(uint16_t address, uint8_t val);
    void set(int address, uint8_t val);
    void dec(uint16_t address);
    void dec(int address);
    void stack_push(uint16_t &sp_val, uint8_t pc_val);
    void stack_push(uint16_t &sp_val, uint16_t pc_val);
    void stack_pop(uint16_t &sp_val, uint8_t &dest);
    void stack_pop(uint16_t &sp_val, uint16_t &dest);

    RamSubset get_character_ram();
    RamSubset get_bg_map_1();
    RamSubset get_bg_map_2();
    RamSubset get_internal_work_ram();
    RamSubset get_echo_ram();
    RamSubset get_oam();
    RamSubset get_io_registers();
    RamSubset get_high_ram();
    void load_bios(char *bios_path);
    void load_rom(char *rom_path);
    
private:
    // @TODO Reduce this to exclude mappings to other
    // periferals.
    uint8_t memory[MAX_MEM_SIZE] = {};
    uint16_t memory_max = MAX_MEM_SIZE;
};
