#include <iostream>
#include <fstream>
#include <cstring>
#include <signal.h>

#include "helper.h"
#include "ram.h"
#define DEBUG 0

RAM::RAM() {
    // Initialise memory to 0.
    // @TODO This is NOT what is done on the real console -
    // internal and high RAM should be left as random values.
    for (int me = 0; me < this->memory_max; me ++)
        this->memory[me] = 0;
}

uint8_t RAM::get_val(uint16_t address) {
    uint8_t val;
    memcpy(&val, &this->memory[address], 1);
    if (DEBUG)
        std::cout << std::hex << "Got from RAM (" << address << "): "  << (int)val << std::endl;
    return val;
}

uint8_t* RAM::get_ref(uint16_t address) {
    uint8_t *mem_ptr = this->memory;
    return mem_ptr + address;
}
void RAM::stack_push(uint16_t &sp_val, uint8_t pc_val) {
    // Decrease SP value, then store pc_val into the memory location
    // of sp
    sp_val --;
    if (DEBUG)
        std::cout << "pushing to stack: " << std::hex << (unsigned int)pc_val << " at " << (int)sp_val << std::endl;
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
uint8_t RAM::stack_pop8(uint16_t &sp_val) {
    // Obtain value from stack and decrease SP value,
    uint8_t dest = this->get_val(sp_val);
    if (DEBUG)
        std::cout << "got: " << std::hex << dest << " from " << (int)sp_val << std::endl;
    sp_val ++;
    return dest;
}
uint16_t RAM::stack_pop(uint16_t &sp_val) {
    union {
        uint8_t bit8[2];
        uint16_t bit16[1];
    } data_conv;
    data_conv.bit8[0] = this->stack_pop8(sp_val);
    data_conv.bit8[1] = this->stack_pop8(sp_val);
    if (DEBUG)
        std::cout << "Returning to: " << std::hex << (unsigned int)data_conv.bit16[0] << std::endl;
    return data_conv.bit16[0];
}

void RAM::set(uint16_t address, uint8_t val) {
    // If attempting to inc the LCD LY attribute, just
    // reset it
    //if (address == this->LCDC_LY_ADDR)
    //    this->v_set(address, 0x00);
    //else
        this->v_set(address, val);
}
void RAM::v_set(uint16_t address, uint8_t val) {
    //if (address < 0x4000) {
    //    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    //    std::cout << std::hex << "Forbidden RAM write: " << (int)val << " at " << (int)address << std::endl;
    //    std::cin.get();
    //    //raise(SIGSEGV);
    //}

    if (address == this->ROM_SWAP_ADDRESS && val)
        this->swap_boot_rom();

    if (DEBUG)
        std::cout << std::hex << "Set RAM value (" << address << "): "  << (int)val << std::endl;
    this->memory[address] = val;
}
uint8_t RAM::dec(uint16_t address) {
    this->memory[address] --;
    return this->memory[address];
}
uint8_t RAM::inc(uint16_t address) {
    // If attempting to inc the LCD LY attribute, just
    // reset it
    //if (address == this->LCDC_LY_ADDR)
    //{
        //this->v_set(address, 0x00);
        //return (uint8_t)0x00;
    //}
    //else
        return this->v_inc(address);
}
uint8_t RAM::v_inc(uint16_t address) {
    this->memory[address] ++;
    return this->memory[address];
}


unsigned int RAM::get_ram_bit(uint16_t address, unsigned int bit_shift) {
    return ((this->get_val(address) & (1U  << bit_shift)) >> bit_shift);
}
uint8_t RAM::set_ram_bit(uint16_t address, uint8_t bit_shift, unsigned int val) {
    uint8_t source = this->get_val(address);
    if (val == 1)
        source |= (1 << bit_shift);
    else
        source &= (0xff ^ (1 << bit_shift));
    this->v_set(address, source);
    return source;
}



RamSubset RAM::get_io_registers() {
    return RamSubset(this->memory + 0xff00, 0x80);
}

RamSubset RAM::get_high_ram() {
    return RamSubset(this->memory + 0xff80, 0x80);
}

void RAM::load_bios(char *bios_path) {
    // Open file
    std::ifstream infile(bios_path, std::ios::binary);

    // Iterate through bytes in file and store in memory
    size_t addr = 0;
    while (true) {
        uint8_t ch = infile.get();
        if (infile.eof()) {
            break;
        }
        this->memory[addr] = (uint8_t)ch;

        // DEBUG for loading BIOS
        if (DEBUG)
        {
            std::cout << std::hex << (int)addr << " " << (int)ch << " " << (int)this->memory[addr] << std::endl;
        }

        addr++;
    }
    infile.close();
    //std::cin.get();
}

void RAM::load_rom(char *rom_path) {
    // Open file
    std::ifstream infile(rom_path, std::ios::binary);

    // Iterate through bytes in file and store in memory
    size_t addr = 0;
    while (true) {
        uint8_t ch = infile.get();
        if (infile.eof()) {
            break;
        }
        // Skip first 256 bytes?
        if (addr <= 255)
            this->memory_boot_swap[addr] = (uint8_t)ch;
        else
            this->memory[addr] = (uint8_t)ch;
        addr++;
        //std::cout << std::hex << (int)(addr + 255) << " " << (int)ch << " " << (int)this->memory[addr] << std::endl;
    }
    if (DEBUG)
        infile.close();
    std::cin.get();
}
void RAM::swap_boot_rom() {
    uint8_t temp;

    for (unsigned int itx = 0; itx < this->BOOT_ROM_SIZE; itx ++) {
        temp = this->memory[itx];
        this->memory[itx] = this->memory_boot_swap[itx];
        this->memory_boot_swap[itx] = temp;
    }
}