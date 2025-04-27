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
    uint8 PIXEL[64][32];

    // Keyboard
    bool KEYBOARD[16];
    uint8 LAST_KEY;
    bool WAIT_FOR_KEY;
    bool WAIT_COMPLETE;

    bool log = true;

    void init(const char* program_file);
    void step();
    void timer();
};

#endif