#pragma once

#include <memory>
#include "ram.h"
#include <SFML/Graphics.hpp>

class VPU {
public:
    VPU(RAM *ram, sf::RenderWindow *window);
    void tick();
    
private:
    // Clock cycle definitions
    // Not sure which is most critical - timing of overall screen refresh or lenth of vblank
    const unsigned int VBLANK_LENGTH = 0x11D0;
    const unsigned int SCREEN_REFRESH_INTERVAL = 0x011250;
    unsigned int refresh_timer_itx;
    
    // Line timings
    // Mode 0 is just the remaining time after mode 2 and 3 and before H_LENGTH
    const unsigned int MODE2_LENGTH = 0x4D; // 77
    const unsigned int MODE3_LENGTH = 0xA9; // 169
    const unsigned int H_LENGTH = 0x01c8; // 456
    unsigned int h_timer_itx;
    
    // Memory locations
    const uint16_t LCDC_CONTROL_ADDR = 0xff40; // LCD Control
    const uint16_t LCDC_STATUS_ADDR = 0xff41; // LCD status
    const uint16_t LCDC_SCY = 0xff42; // Background Scroll Y
    const uint16_t LCDC_SCX = 0xff43; // Background Scroll X
    const uint16_t LCDC_LY_ADDR = 0xff44; // Current line draw iterator
    const uint16_t LCDC_LYC_ADDR = 0xff45; // Custom location for interupt if LY equals this
    const uint16_t LCDC_WY_ADDR = 0xff4a; // Window y position
    const uint16_t LCDC_WX_ADDR = 0xff4b; // Window x position
    const uint16_t VRAM_BG_MAPS[2] = {0x9800, 0x9c00};
    const uint16_t VRAM_TILE_DATA_TABLES[2] = {0x8000, 0x8800};
    
    void next_screen();
    void next_line();
    
    RAM *ram;
    sf::RenderWindow *window;
};