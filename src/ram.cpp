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

uint8_t RAM::get_val(int address) {
    uint8_t val;
    memcpy(&val, &this->memory[address], 1);
    return val;
}
uint8_t RAM::get_val(uint16_t address) {
    return this->get_val((int)address);
}

uint8_t* RAM::get_ref(uint16_t address) {
    int addr = (int)address;
    uint8_t *mem_ptr = this->memory;
    //memcpy(&addr, &address, 1);
    return mem_ptr + addr;
}
void RAM::stack_push(uint16_t &sp_val, uint8_t pc_val) {
    // Decrease SP value, then store pc_val into the memory location
    // of sp
    sp_val --;
    this->set(sp_val, pc_val);
}
void RAM::stack_push(uint16_t &sp_val, uint16_t pc_val) {
    union {
        uint8_t bit8[2];
        uint16_t bit16[1];
    } data_conv;
    data_conv.bit16[0] = pc_val;

    // Write backwards due to little endian, but due to the writing of
    // the stack working backwards, it will write it backwards again,
    // so if read, it would be fowards
    // e.g.
    // |      |
    // |     H|
    // |    LH|
    this->stack_push(sp_val, data_conv.bit8[1]);
    this->stack_push(sp_val, data_conv.bit8[0]);    
}

void RAM::set(int address, uint8_t val) {
    this->memory[address] = val;
}
void RAM::set(uint16_t address, uint8_t val) {
    this->set((int)address, val);
}
void RAM::dec(int address) {
    this->memory[address] --;
}
void RAM::dec(uint16_t address) {
    this->dec((int)address);
}


RamSubset RAM::get_io_registers() {
    return RamSubset(this->memory + 0xff00, 0x80);
}

RamSubset RAM::get_high_ram() {
    return RamSubset(this->memory + 0xff80, 0x80);
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