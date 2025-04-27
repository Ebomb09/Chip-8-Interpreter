#ifndef CHIP_8_INTEPRETER
#define CHIP_8_INTEPRETER

#include <string>

using uint8 = unsigned char;
using uint16 = unsigned short;

struct chip8 {

    // Program memory
    uint8 RAM[4096];

    // Registers
    uint8 V[16];
    uint16 I;

    // Program counter, stack pointer, and stack
    uint16 PC;
    uint8 SP;
    uint16 STACK[16];

    // Delay timer, and sound timer
    uint8 DT;
    uint8 ST;

    // Display
    static const uint8 DISPLAY_WIDTH = 64;
    static const uint8 DISPLAY_HEIGHT = 32;
    uint8 PIXEL[DISPLAY_WIDTH][DISPLAY_HEIGHT];

    // Keyboard
    bool KEYBOARD[16];
    uint8 LAST_KEY;
    bool WAIT_FOR_KEY;
    bool WAIT_COMPLETE;

    enum ERROR {
        NONE,
        STD_FILE_NOT_FOUND,
        PROGRAM_FILE_NOT_FOUND,
        ILLEGAL_INSTRUCTION
    };

    int init(const char* program_file);
    int step();
    void timer();
};

#endif