#include <iostream>
#include <string>
#include "ram.h"
#include "./vpu.h"
#include "cpu.h"
#include <X11/Xlib.h>

#define SCREEN_WIDTH 144
#define SCREEN_HEIGHT 160
#define APP_NAME "GameBoy Emulator"

#define DEBUG 0

struct arguments_t {
    char *bios_path;
    char *rom_path;
    bool valid;
};

arguments_t get_arguments(int argc, char* args[]) {
    arguments_t arguments;
    if (argc != 3) {
        std::cout << "Usage: GameboyEmulator <BIOS path> <ROM path>" << std::endl;
        arguments.valid = false;
    } else {
        arguments.valid = true;
        arguments.bios_path = args[1];
        arguments.rom_path = args[2];
        std::cout << "Using BIOS path: " << arguments.bios_path << std::endl;
        std::cout << "Using ROM path: " << arguments.rom_path << std::endl;
    }
    return arguments;
}

int main(int argc, char* args[])
{
    //arguments_t arguments = get_arguments(argc, args);
    //if (arguments.valid == false) {
    //    return 1;
    //}
    
    // create the window
//    Display *di = XOpenDisplay(getenv("DISPLAY"));
//    if (di == NULL) {
//		printf("Couldn't open display.\n");
//		return -1;
//	}

    RAM ram_inst = RAM();
    char bios_path[] = "./copyright/DMG_ROM.bin";
    char rom_path[] = "./copyright/Tetris (JUE) (V1.1) [!].gb";
    ram_inst.load_bios(bios_path);
    ram_inst.load_rom(rom_path);

    VPU vpu_inst = VPU(&ram_inst);
    CPU cpu_inst = CPU(&ram_inst, &vpu_inst);

    XEvent ev;

    // run the program as long as the window is open
    while (cpu_inst.is_running())
    {
        cpu_inst.tick();
        vpu_inst.tick();
        
        while (XPending(vpu_inst.di))
            XNextEvent(vpu_inst.di, &ev);
    }
    vpu_inst.tear_down();
    
    std::cin.get();

    return 0;
}