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
    //open file
    std::ifstream infile(bios_path);

    //get length of file
    infile.seekg(0, infile.end);
    size_t length = infile.tellg();
    infile.seekg(0, infile.beg);

    //union {
    //    uint8_t num[static_cast<int>(length)];
    //    char ch[static_cast<int>(length)];
    //} data;

    // don't overflow the buffer!
    //if (length > sizeof (this->memory))
    //{
    //    length = sizeof (this->memory);
    //}

    //union {
    //    uint8_t uint[1];
    //    char chr[1];
    //} tmp_data;

    //read file
    size_t itx = 0;
    //char tmp[1];
    while (itx < length) {
        infile >> this->memory[itx];
        //infile.read(tmp_data.chr, 1);
        //std::memcpy(this->memory[itx], tmp_data.uint, 1);
        std::cout << std::hex << static_cast<int>(this->memory[itx]) << std::endl;
        itx ++;
    }
    //infile.read(data.ch, 1);
    
    //for (long itx = 0; itx < length; itx ++;)
    //    this->memory[itx] = data.ch[itx];
    infile.close();
    
    
}
void RAM::load_rom(char *rom_path) {
    
}