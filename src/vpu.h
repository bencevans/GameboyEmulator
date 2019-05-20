#pragma once

#include <memory>
#include "ram.h"
//#include <SFML/Graphics.hpp>
#include <stdlib.h>
#include <X11/Xlib.h>


struct vec_2d {
    uint8_t x;
    uint8_t y;
};

class VPU {
public:
    VPU(RAM *ram);
    void tick();
    void tear_down();
    Display *di;
private:
    // Clock cycle definitions
    // Not sure which is most critical - timing of overall screen refresh or lenth of vblank
    const unsigned int VBLANK_LENGTH = 0x11D0;
    const unsigned int SCREEN_REFRESH_INTERVAL = 0x011250;
    unsigned int refresh_timer_itx;
    
    const unsigned int SCREEN_WIDTH = 144;
    const unsigned int SCREEN_HEIGHT = 160;
    
    // Line timings
    // Mode 0 is just the remaining time after mode 2 and 3 and before H_LENGTH
    const unsigned int MODE2_LENGTH = 0x4D; // 77
    const unsigned int MODE3_LENGTH = 0xA9; // 169
    const unsigned int H_LENGTH = 0x01c8; // 456
    unsigned int h_timer_itx;
    
    unsigned int current_pixel_x;
    
    // Memory locations
    const uint16_t LCDC_CONTROL_ADDR = 0xff40; // LCD Control
    const uint16_t LCDC_STATUS_ADDR = 0xff41; // LCD status
    const uint16_t LCDC_SCY = 0xff42; // Background Scroll Y
    const uint16_t LCDC_SCX = 0xff43; // Background Scroll X
    const uint16_t LCDC_LY_ADDR = 0xff44; // Current line draw iterator
    const uint16_t LCDC_LYC_ADDR = 0xff45; // Custom location for interupt if LY equals this
    const uint16_t LCDC_WY_ADDR = 0xff4a; // Window y position
    const uint16_t LCDC_WX_ADDR = 0xff4b; // Window x position
    
    // Each is 3FF bytes (1024 bytes), providing 32x32 tiles, meanig 1 byte per tile
    const uint16_t VRAM_BG_MAPS[2] = {0x9800, 0x9c00};
    const uint16_t VRAM_TILE_DATA_TABLES[2] = {0x8000, 0x8800};
    const uint16_t BACKGROUND_TILE_GRID_WIDTH = 32;
    const uint16_t BACKGROUND_TILE_GRID_HEIGHT = 32;
    const uint16_t TILE_WIDTH = 8;
    const uint16_t TILE_HEIGHT = 8;
    
    void next_screen();
    void next_line();
    
    RAM *ram;
//    sf::RenderWindow *window;
//    sf::Sprite sf_sprite;
//    sf::FloatRect sf_rect;
//    sf::Texture sf_texture;
//    sf::Image sf_image;
    int sc;
    Window ro;
    Window wi;
    GC gc;
    XEvent ev;
    
    
    uint8_t get_background_scroll_x();
    uint8_t get_background_scroll_y();
    uint8_t get_current_x();
    uint8_t get_current_y();
    uint8_t get_background_map_type();
    uint8_t get_background_data_type();
    uint8_t get_pixel_color();
    uint16_t get_tile_address();
    vec_2d get_pixel_tile_position();
    
    void process_pixel();
    
    // Copy from CPU :(
    unsigned char get_register_bit(uint16_t address, uint8_t bit_shift);
};