// Copyright (C) Dock Studios Ltd, Inc - All Rights Reserved
// Unauthorized copying of this file, via any medium is strictly prohibited
// Proprietary and confidential
// Written by Matt Comben <matthew@dockstudios.co.uk>, May 2019

#include "ram_subset.h"
#include <memory>

RamSubset::RamSubset(uint8_t *memory, uint16_t size) {
    this->memory = memory;
    this->size = size;
}

bool RamSubset::validate_address(uint16_t address) {
    return (address <= this->size);
}

uint8_t RamSubset::get_val(uint16_t a) {
    if (! this->validate_address(a)) {
        return 0;
    }
    return this->memory[a];
}

bool RamSubset::set(uint16_t a, uint8_t value) {
    if (! this->validate_address(a)) {
        return false;
    }
    this->memory[a] = value;
    return true;
}
