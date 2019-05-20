#include "vpu.h"

// Used for printing hex
// #include  <iomanip>

#include <iostream>
#include <unistd.h>
// #include <string.h>
#include <memory>

#define DEBUG 1

VPU::VPU(RAM *ram) {
    this->ram = ram;
    this->di = XOpenDisplay(getenv("DISPLAY"));
    if (this->di == NULL) {
		printf("Couldn't open display.\n");
	}
    this->h_timer_itx = 0;
    this->refresh_timer_itx = 0;
    // Reset current line
    this->ram->set(this->LCDC_LY_ADDR, 0x00);
    this->current_pixel_x = 0;

	//Create Window
	int const x = 50, y = 50, border_width = 1;
	this->sc = DefaultScreen(this->di);
	this->ro = DefaultRootWindow(this->di);
	this->wi = XCreateSimpleWindow(
        this->di, this->ro,
        x, y,
        (unsigned int)this->SCREEN_WIDTH, (unsigned int)this->SCREEN_HEIGHT,
        border_width, 
        BlackPixel(this->di, this->sc),
        WhitePixel(this->di, this->sc));

    XMapWindow(this->di, this->wi); //Make window visible
    this->wait_for_window();
    XFlush(this->di);

	XStoreName(this->di, this->wi, "Gameboy EmU"); // Set window title
	//Prepare the window for drawing
	this->gc = XCreateGC(this->di, this->ro, 0, NULL);
	//Select what events the window will listen to
	XSelectInput(this->di, this->wi, KeyPressMask | ExposureMask);

    // Wait some BS time for window to display
    //sleep(10);

    // Set all pixels to black
    XSetForeground(this->di, this->gc, (unsigned long)0xFFDDCCFFDDCC);
    for (unsigned int x = 0; x < this->SCREEN_WIDTH; x++)
        for (unsigned int y = 0; y < this->SCREEN_HEIGHT; y++)
                XDrawPoint(this->di, this->wi, this->gc, (int)x, (int)y);

}

static Bool predicate(Display *display, XEvent *ev, XPointer arg)
{
    std::cout << ev->type << std::endl;
    return (ev->type == MapNotify);
}

void VPU::wait_for_window() {
    
    XEvent ev;
    XWindowAttributes xwa;

    XPeekIfEvent(this->di, &ev, predicate, NULL);

    do {
        XGetWindowAttributes(this->di, this->wi, &xwa);
        usleep(1);
        std::cout << xwa.map_state;
    } while(xwa.map_state != IsViewable);  
}

void VPU::process_events() {
    //while (XPending(this->di))
    //while (XEventsQueued(this->di, QueuedAlready))
    //    XNextEvent(this->di, &this->ev);
    while (XPending (this->di))
    //while (XCheckWindowEvent(this->di, this->wi, 0, &this->ev))
        XNextEvent (this->di, &this->ev);
        //if (XFilterEvent (&this->ev, None))
        //{
        //    continue;
        //}
    //}
}

void VPU::next_screen() {
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

unsigned char VPU::get_register_bit(uint16_t address, uint8_t bit_shift) {
    uint8_t val = this->ram->get_val(address);
    return (val & (1U  << bit_shift)) >> bit_shift;
}

uint8_t VPU::get_background_map_type() {
    return this->get_register_bit(this->LCDC_CONTROL_ADDR, 0x04);
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
        this->get_tile_address() +
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
    XFreeGC(this->di, this->gc);
	XDestroyWindow(this->di, this->wi);
	XCloseDisplay(this->di);
}

void VPU::process_pixel() {
    //
    uint32_t color;
    switch(this->get_pixel_color()) {
        case 0x00:
            color = 0x00;
            break;
        case 0x01:
            color = 0x00444444;
            break;
        case 0x02:
            color = 0x00888888;
            break;
        case 0x03:
            color = 0x00cccccc;
            break;
        default:
            color = 0x00ffffff;
    };
    XSetForeground(this->di, this->gc, color);
	XDrawPoint(this->di, this->wi, this->gc, (int)this->get_current_x(), (int)this->get_current_y());
    this->process_events();
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


uint16_t VPU::get_tile_address() {
    return (this->VRAM_TILE_DATA_TABLES[this->get_background_data_type()] + this->ram->get_val(
        this->VRAM_BG_MAPS[this->get_background_map_type()] +
        (uint16_t)(
            ((int)(this->get_background_scroll_y() + this->get_current_y()) * this->BACKGROUND_TILE_GRID_WIDTH) +
            (int)(this->get_background_scroll_x() + this->get_current_x())
        )
    ));
}
