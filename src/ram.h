#pragma once

#include <memory>
#include "ram_subset.h"

class RAM {
public:
    void initialise();
    
private:
    // @TODO Reduce this to exclude mappings to other
    // periferals.
    uint8_t memory[0x10000] = {};
};
