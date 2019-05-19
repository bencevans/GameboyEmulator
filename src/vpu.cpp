#include "vpu.h"

// Used for printing hex
// #include  <iomanip>

// #include <iostream>
// #include <string.h>
#include <memory>

#define DEBUG 1

VPU::VPU(RAM *ram) {
    this->ram = ram;
    this->next_screen();
}

void VPU::next_screen() {
    this->h_timer_itx = 0;
    this->refresh_timer_itx = 0;
    // Reset current line
    this->ram->set(this->LCDC_LY_ADDR, 0x0);
}

void VPU::next_line() {
    this->h_timer_itx = 0;
    // Increment current line
    this->ram->inc(this->LCDC_LY_ADDR);
}

void VPU::tick() {
    // Update timers and skip to next screen,
    // if required.
    // Update screen timer
    this->refresh_timer_itx ++;
    if (this->refresh_timer_itx > this->SCREEN_REFRESH_INTERVAL)
        this->next_screen();
    // Update line timer
    this->h_timer_itx ++;
    if (this->h_timer_itx > this->H_LENGTH)
        this->next_line();


}

//void VPU::get_pixel_color(uint8_t x, uint8_t y) {
//    
//}