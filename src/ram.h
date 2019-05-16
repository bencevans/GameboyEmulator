#pragma once

#include <memory>
#include "ram_subset.h"

// 0x10000
#define MAX_MEM_SIZE 65535

class RAM {
public:
    RAM();
    uint8_t get();
    RamSubset get_character_ram();
    RamSubset get_bg_map_1();
    RamSubset get_bg_map_2();
    RamSubset get_internal_work_ram();
    RamSubset get_echo_ram();
    RamSubset get_oam();
    RamSubset get_high_ram();
    void load_bios(char *bios_path);
    void load_rom(char *rom_path);
    
private:
    // @TODO Reduce this to exclude mappings to other
    // periferals.
    uint8_t memory[MAX_MEM_SIZE] = {};
    uint16_t memory_max = MAX_MEM_SIZE;
};
