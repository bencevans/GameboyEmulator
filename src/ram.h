#pragma once

#include <memory>
#include "ram_subset.h"

class RAM {
public:
    void initialise();
    RamSubset get_character_ram();
    RamSubset get_bg_map_1();
    RamSubset get_bg_map_2();
    RamSubset get_internal_work_ram();
    RamSubset get_echo_ram();
    RamSubset get_oam();
    RamSubset get_high_ram();
    
private:
    // @TODO Reduce this to exclude mappings to other
    // periferals.
    uint8_t memory[0x10000] = {};
};
