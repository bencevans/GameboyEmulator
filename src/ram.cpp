#include <iostream>
#include <fstream>
#include <cstring>

#include "ram.h"

RAM::RAM() {
    // Initialise memory to 0.
    // @TODO This is NOT what is done on the real console -
    // internal and high RAM should be left as random values.
    for (int me = 0; me < this->memory_max; me ++)
        this->memory[me] = 0;
}

void RAM::load_bios(char *bios_path) {
    // Open file
    std::ifstream infile(bios_path);

    // Get length of file
    infile.seekg(0, infile.end);
    size_t length = infile.tellg();
    infile.seekg(0, infile.beg);

    // Iterate through bytes in file and store in memory
    size_t itx = 0;
    while (itx < length) {
        infile >> this->memory[itx];
        itx ++;
    }
    infile.close();
}
void RAM::load_rom(char *rom_path) {
    
}