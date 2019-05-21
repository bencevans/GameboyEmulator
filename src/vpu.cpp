#include "vpu.h"

// Used for printing hex
// #include  <iomanip>

#include <iostream>
#include <unistd.h>
// #include <string.h>
#include <memory>

#define DEBUG 0

VPU::VPU(RAM *ram) {
    this->ram = ram;
    SDL_Init(SDL_INIT_VIDEO);
    this->window = SDL_CreateWindow("Gameboy Emu",
        0, 0, this->SCREEN_WIDTH, this->SCREEN_HEIGHT, 0);
    this->renderer = SDL_CreateRenderer(this->window, -1, 0);
    SDL_CreateWindowAndRenderer(this->SCREEN_HEIGHT, this->SCREEN_WIDTH, 0, &this->window, &this->renderer);


    this->h_timer_itx = 0;
    this->refresh_timer_itx = 0;
    // Reset current line
    this->ram->set(this->LCDC_LY_ADDR, 0x00);
    this->current_pixel_x = 0;

	//Create Window

    // Clear with white
    SDL_SetRenderDrawColor(this->renderer, 255, 255, 255, 0);
    SDL_RenderClear(this->renderer);
    SDL_RenderPresent(this->renderer);
    SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 0);
    // Set all pixels to black
    for (unsigned int x = 0; x < this->SCREEN_WIDTH; x++)
        for (unsigned int y = 0; y < this->SCREEN_HEIGHT; y++)
                SDL_RenderDrawPoint(this->renderer, (int)y, (int)x);
                

    SDL_SetRenderDrawColor(this->renderer, 0, 255, 0, 0);
    // Set all pixels to black
    for (unsigned int x = 60; x < 80; x++)
        for (unsigned int y = 50; y < 90; y++)
                SDL_RenderDrawPoint(this->renderer, (int)x, (int)y);
    SDL_RenderPresent(this->renderer);
}

void VPU::process_events() {
    //while (XPending(this->di))
    //while (XEventsQueued(this->di, QueuedAlready))
    //    XNextEvent(this->di, &this->ev);
    //while (XPending (this->di))
    //while (XCheckWindowEvent(this->di, this->wi, 0, &this->ev))
        //XNextEvent (this->di, &this->ev);
        //if (XFilterEvent (&this->ev, None))
        //{
        //    continue;
        //}
    //}
    
    SDL_RenderPresent(this->renderer);
    //SDL_PollEvent(&this->event);
}

void VPU::next_screen() {
    this->process_events();
    if (DEBUG)
        std::cout << "NEXT SCREEN" << std::endl;
    this->h_timer_itx = 0;
    this->refresh_timer_itx = 0;
    // Reset current line
    this->ram->set(this->LCDC_LY_ADDR, 0x00);
}

void VPU::next_line() {
    this->h_timer_itx = 0;
    this->current_pixel_x = 0;
    // Increment current line
    if (this->get_current_y() == this->SCREEN_HEIGHT)
        this->next_screen();
    else
        this->ram->inc(this->LCDC_LY_ADDR);
}

void VPU::tick() {
    // Update timers and skip to next screen,
    // if required.
    // Update screen timer
    if (! this->lcd_enabled())
        return;

    this->refresh_timer_itx ++;
    if (this->refresh_timer_itx >= this->SCREEN_REFRESH_INTERVAL)
        this->next_screen();
    // Update line timer
    this->h_timer_itx ++;

    if (this->h_timer_itx >= this->H_LENGTH) {
        if (DEBUG)
            std::cout << "NEXT LINE!" << std::endl;
        this->next_line();
    }

    if (this->get_current_x() < this->SCREEN_WIDTH)
        this->process_pixel();
}

uint8_t VPU::get_background_scroll_y() {
    return this->ram->get_val(this->LCDC_SCY);
}
uint8_t VPU::get_background_scroll_x() {
    return this->ram->get_val(this->LCDC_SCX);
}

unsigned int VPU::get_register_bit(uint16_t address, unsigned int bit_shift) {
    return ((this->ram->get_val(address) & (1U  << bit_shift)) >> bit_shift);
}

uint8_t VPU::lcd_enabled() {
    return 1;
    return this->get_register_bit(this->LCDC_CONTROL_ADDR, 0x07);
}

uint8_t VPU::get_background_map() {
    return this->get_register_bit(this->LCDC_CONTROL_ADDR, 0x03);
}
uint8_t VPU::get_background_data_type() {
    return this->get_register_bit(this->LCDC_CONTROL_ADDR, 0x04);
}

vec_2d VPU::get_pixel_tile_position() {
    vec_2d pos;
    pos.x = ((int)this->get_background_scroll_x() + (int)this->get_current_x()) % this->TILE_WIDTH;
    pos.y = ((int)this->get_background_scroll_y() + (int)this->get_current_y()) % this->TILE_HEIGHT;
    return pos;
}

uint8_t VPU::get_pixel_color() {
    
    vec_2d pos = this->get_pixel_tile_position();
    // Get byte index, by determining where pixel is in the byte (which contains half a line of colours)
    int byte_index = ((int)pos.x % (this->TILE_WIDTH / 2)) * 2;
    uint8_t colour_byte = this->ram->get_val(
        this->get_current_tile_data_address() +
        (uint16_t)(pos.y * this->TILE_HEIGHT) +
        (uint16_t)(pos.x >= (this->TILE_WIDTH / 2) ? 1 : 0)
    );
    if (DEBUG)
        std::cout << std::hex << "Colour Byte index: " << byte_index <<
            " tile width: " << (int)this->TILE_WIDTH <<
            " tile x pos: " << (int)pos.x <<
            " colour byte: " << (int)colour_byte <<
            " colour nibble: " << (int)((colour_byte << byte_index) & (0x04)) << std::endl;
    return (colour_byte << byte_index) & (0x04);
}

void VPU::tear_down() {
    SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->window);
    SDL_Quit();
}

void VPU::process_pixel() {
    //
    uint8_t color = this->get_pixel_color();
    switch(color) {
        case 0x00:
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            break;
        case 0x01:
            SDL_SetRenderDrawColor(renderer, 86, 86, 86, 0);
            break;
        case 0x02:
            SDL_SetRenderDrawColor(renderer, 172, 172, 172, 0);
            break;
        case 0x03:
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
            break;
        default:
            // Random colour
            SDL_SetRenderDrawColor(renderer, 255, 0, 128, 0);
    };
    SDL_RenderDrawPoint(this->renderer, (int)this->get_current_x(), (int)this->get_current_y());
    //this->process_events();
    if (DEBUG && color != 0x0)
        std::cout << std::hex << "Setting Pixel color: " << (unsigned int)this->get_current_x() << " " << (unsigned int)this->get_current_y() << " " << (int)color << std::endl;
    this->current_pixel_x ++;
}

// Return the on-screen X coornidate of the pixel being drawn 
uint8_t VPU::get_current_x() {
    return this->current_pixel_x;
}
// Return the on-screen X coornidate of the pixel being drawn 
uint8_t VPU::get_current_y() {
    return this->ram->get_val(this->LCDC_LY_ADDR);
}

uint8_t convert_int8_uint8(uint8_t in_val) {
    union {
        uint8_t uint8[2];
        int8_t int8[2];
    } data;
    data.int8[0] = (int8_t)in_val;
    return data.uint8[0];
}

int VPU::get_tile_map_index_from_current_coord() {
    return ((int)(this->get_background_scroll_y() + this->get_current_y()) * this->BACKGROUND_TILE_GRID_WIDTH) +
            (int)(this->get_background_scroll_x() + this->get_current_x());
}

uint16_t VPU::get_tile_data_address(uint8_t tile_number) {
    int offset;
    int mode = (int)this->get_background_data_type();
    if (mode == 0x3)
        offset = convert_int8_uint8(tile_number);
    else
        offset = tile_number;
    return offset + this->VRAM_TILE_DATA_TABLES[mode];
}

uint16_t VPU::get_current_tile_data_address() {
    return this->get_tile_data_address(
        this->ram->get_val(
            this->VRAM_BG_MAPS[this->get_background_map()] +
            this->get_tile_map_index_from_current_coord()
        )
    );
}


//uint16_t VPU::get_tile_address() {
//    uint16_t tile_address = get_tile_data_address()
//
//    this->ram->get(this->VRAM_BG_MAPS[this->get_background_map_type()] + this->get_tile_map_index_from_current_coord())
//
//
//
//    return (this->VRAM_TILE_DATA_TABLES[this->get_background_data_type()] + this->ram->get_val(
//        this->VRAM_BG_MAPS[this->get_background_map_type()] +
//        (uint16_t)(
//            ((int)(this->get_background_scroll_y() + this->get_current_y()) * this->BACKGROUND_TILE_GRID_WIDTH) +
//            (int)(this->get_background_scroll_x() + this->get_current_x())
//        )
//    ));
//}
