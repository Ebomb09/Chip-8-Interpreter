#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>

#include "chip8.h"

int chip8::init(const char* program_file) {

    // Initialize memory
    std::memset(RAM, 0, sizeof(uint8) * 4096);
    std::memset(V, 0, sizeof(uint8) * 16);
    I = 0;
    PC = 0x200;
    SP = 0;
    std::memset(STACK, 0, sizeof(uint8) * 16);
    DT = 0;
    ST = 0;
    std::memset(PIXEL, 0, sizeof(uint8) * DISPLAY_WIDTH * DISPLAY_HEIGHT);
    std::memset(KEYBOARD, false, sizeof(bool) * 16);
    LAST_KEY = 0;
    WAIT_FOR_KEY = false;
    WAIT_COMPLETE = false;

    // Load reserved data into RAM
    std::fstream reserved("std/reserved.ch8", std::fstream::in | std::fstream::binary);
    int pos = 0;

    if(!reserved.good())
        return ERROR::STD_FILE_NOT_FOUND;

    while(reserved.good()) 
        reserved.read((char*)&RAM[pos++], 1);

    reserved.close();

    // Load program into RAM
    pos = 0x200;

    std::fstream program(program_file, std::fstream::in | std::fstream::binary);

    if(!program.good())
        return ERROR::PROGRAM_FILE_NOT_FOUND;

    while(program.good()) 
        program.read((char*)&RAM[pos++], 1);

    program.close();
    return ERROR::NONE;
}

int chip8::step() {
    char code = (RAM[PC] & 0b11110000) >> 4;

    // 00E0 - CLS
    if(code == 0x0 && RAM[PC+1] == 0xE0) {
        std::memset(PIXEL, 0, sizeof(uint8) * DISPLAY_WIDTH * DISPLAY_HEIGHT);
        PC += 2;
        return ERROR::NONE;

    // 00EE - RET
    }else if(code == 0x0 && RAM[PC+1] == 0xEE) {
        SP --;
        PC = STACK[SP];
        return ERROR::NONE;

    // 1nnn - JP addr
    }else if(code == 0x1) {
        PC = ((RAM[PC] & 0b00001111) << 8) + RAM[PC+1];
        return ERROR::NONE;
    
    // 2nnn - CALL addr
    }else if(code == 0x2) {
        STACK[SP] = PC + 2;
        SP ++;
        PC = ((RAM[PC] & 0b00001111) << 8) + RAM[PC+1];
        return ERROR::NONE;

    // 3xkk - SE Vx, byte
    }else if(code == 0x3) {
        uint8 reg = RAM[PC] & 0b00001111;

        if(V[reg] == RAM[PC+1])
            PC += 4;
        else
            PC += 2;
        return ERROR::NONE;

    // 4xkk - SNE Vx, byte
    }else if(code == 0x4) {
        uint8 reg = RAM[PC] & 0b00001111;

        if(V[reg] != RAM[PC+1])
            PC += 4;
        else
            PC += 2;
        return ERROR::NONE;

    // 5xy0 - SE Vx, Vy
    }else if(code == 0x5) {
        uint8 reg1 = RAM[PC] & 0b00001111;
        uint8 reg2 = (RAM[PC+1] & 0b11110000) >> 4;

        if(V[reg1] == V[reg2])
            PC += 4;
        else
            PC += 2;
        return ERROR::NONE;

    // 6xkk - LD Vx, byte
    }else if(code == 0x6) {
        uint8 reg = RAM[PC] & 0b00001111;
        V[reg] = RAM[PC+1];
        PC += 2;
        return ERROR::NONE;
    
    // 7xkk - ADD Vx, byte
    }else if(code == 0x7) {
        uint8 reg = RAM[PC] & 0b00001111;
        V[reg] += RAM[PC+1];
        PC += 2;
        return ERROR::NONE;

    // Math functions
    }else if(code == 0x8) {
        uint8 subcode = RAM[PC+1] & 0b00001111;
        uint8 reg1 = RAM[PC] & 0b00001111;
        uint8 reg2 = (RAM[PC+1] & 0b11110000) >> 4;

        // 8xy0 - LD Vx, Vy
        if(subcode == 0x0) {
            V[reg1] = V[reg2];
            PC += 2;
            return ERROR::NONE;

        // 8xy1 - OR Vx, Vy
        }else if(subcode == 0x1) {
            V[reg1] |= V[reg2];
            PC += 2;
            return ERROR::NONE;

        // 8xy2 - AND Vx, Vy
        }else if(subcode == 0x2) {
            V[reg1] &= V[reg2];
            PC += 2;
            return ERROR::NONE;

        // 8xy3 - XOR Vx, Vy
        }else if(subcode == 0x3) {
            V[reg1] ^= V[reg2];
            PC += 2;
            return ERROR::NONE;

        // 8xy4 - ADD Vx, Vy
        }else if(subcode == 0x4) {

            if(V[reg1] + V[reg2] > 0xFF)    V[15] = 1;
            else                            V[15] = 0;

            V[reg1] += V[reg2];
            PC += 2;
            return ERROR::NONE;

        // 8xy5 - SUB Vx, Vy
        }else if(subcode == 0x5) {

            if(V[reg1] > V[reg2])           V[15] = 1;
            else                            V[15] = 0;

            V[reg1] -= V[reg2];
            PC += 2;
            return ERROR::NONE;

        // 8xy6 - SHR Vx {, Vy}
        }else if(subcode == 0x6) {
            V[15] = V[reg1] & 0b00000001;
            V[reg1] = V[reg1] >> 1;
            PC += 2;
            return ERROR::NONE;

        // 8xy7 - SUBN Vx, Vy
        }else if(subcode == 0x7) {

            if(V[reg2] > V[reg1])           V[15] = 1;
            else                            V[15] = 0;

            V[reg1] = V[reg2] - V[reg1];
            PC += 2;
            return ERROR::NONE;

        // 8xyE - SHL Vx {, Vy}
        }else if(subcode == 0xE) {
            V[15] = V[reg1] & 0b10000000;
            V[reg1] = V[reg1] << 1;
            PC += 2;
            return ERROR::NONE;
        }
    
    // 9xy0 - SNE Vx, Vy
    }else if(code == 0x9) {
        uint8 reg1 = RAM[PC] & 0b00001111;
        uint8 reg2 = (RAM[PC+1] & 0b11110000) >> 4;

        if(V[reg1] != V[reg2])
            PC += 4;
        else
            PC += 2;
        return ERROR::NONE;

    // Annn - LD I, addr
    }else if(code == 0xA) {
        I = ((RAM[PC] & 0b00001111) << 8) + RAM[PC+1];  
        PC += 2;
        return ERROR::NONE;

    // Bnnn - JP V0, addr
    }else if(code == 0xB) {
        PC = ((RAM[PC] & 0b00001111) << 8) + RAM[PC+1] + V[0];  
        return ERROR::NONE;

    // Cxkk - RND Vx, byte
    }else if(code == 0xC) {
        uint8 reg = RAM[PC] & 0b00001111;
        srand(time(NULL));
        V[reg] = ((uint8)rand()) & RAM[PC+1];
        PC += 2;
        return ERROR::NONE;
    
    // Dxyn - DRW Vx, Vy, nibble
    }else if(code == 0xD) {
        uint8 reg1 = RAM[PC] & 0b00001111;
        uint8 reg2 = (RAM[PC+1] & 0b11110000) >> 4;
        uint8 size = RAM[PC+1] & 0b00001111;

        // Collision register
        V[15] = 0;

        // Size is the height of the sprite top->bottom
        for(uint8 i = 0; i < size; i ++) {

            // 8bits correpsonding to each pixel left->right
            for(uint8 j = 0; j < 8; j ++) {

                // Pixel position wraps around screen
                uint8 x = V[reg1] + j;
                uint8 y = V[reg2] + i;

                while(x >= DISPLAY_WIDTH)
                    x -= DISPLAY_WIDTH;

                while(y >= DISPLAY_HEIGHT)
                    y -= DISPLAY_HEIGHT;

                uint8 draw = (RAM[I+i] >> (7 - j)) & 0b00000001;

                // Vf collision bit
                if(PIXEL[x][y] == 1 && draw == 1)
                    V[15] = 1;

                PIXEL[x][y] ^= draw;
            }
        }
        PC += 2;
        return ERROR::NONE;

    // Ex9E - SKP Vx
    }else if(code == 0xE && RAM[PC+1] == 0x9E) {
        uint8 reg = RAM[PC] & 0b00001111;

        if(KEYBOARD[V[reg]])
            PC += 4;
        else
            PC += 2;
        return ERROR::NONE;

    // ExA1 - SKNP Vx
    }else if(code == 0xE && RAM[PC+1] == 0xA1) {
        uint8 reg = RAM[PC] & 0b00001111;

        if(!KEYBOARD[V[reg]])
            PC += 4;
        else
            PC += 2;
        return ERROR::NONE;

    // Fx07 - LD Vx, DT
    }else if(code == 0xF && RAM[PC+1] == 0x07) {
        uint8 reg = RAM[PC] & 0b00001111;
        V[reg] = DT;
        PC += 2;
        return ERROR::NONE;

    // Fx0A - LD Vx, K
    }else if(code == 0xF && RAM[PC+1] == 0x0A) {
        uint8 reg = RAM[PC] & 0b00001111;
        WAIT_FOR_KEY = true;

        if(WAIT_COMPLETE) {
            WAIT_FOR_KEY = false;
            WAIT_COMPLETE = false;
            V[reg] = LAST_KEY;
            PC += 2;
        }
        return ERROR::NONE;

    // Fx15 - LD DT, Vx
    }else if(code == 0xF && RAM[PC+1] == 0x15) {
        uint8 reg = RAM[PC] & 0b00001111;
        DT = V[reg];
        PC += 2;
        return ERROR::NONE;

    // Fx18 - LD ST, Vx
    }else if(code == 0xF && RAM[PC+1] == 0x18) {
        uint8 reg = RAM[PC] & 0b00001111;
        ST = V[reg];
        PC += 2;
        return ERROR::NONE;

    // Fx1E - ADD I, Vx
    }else if(code == 0xF && RAM[PC+1] == 0x1E) {
        uint8 reg = RAM[PC] & 0b00001111;
        I += V[reg];
        PC += 2;
        return ERROR::NONE;

    // Fx29 - LD F, Vx
    }else if(code == 0xF && RAM[PC+1] == 0x29) {
        uint8 reg = RAM[PC] & 0b00001111;
        I = V[reg] * 5;
        PC += 2;
        return ERROR::NONE;

    // Fx33 - LD B, Vx
    }else if(code == 0xF && RAM[PC+1] == 0x33) {
        uint8 reg = RAM[PC] & 0b00001111;
        uint8 val = V[reg];

        // Calculate individual digits of the register
        RAM[I] = 0;
        RAM[I+1] = 0;
        RAM[I+2] = 0;

        while(val >= 100) {
            val -= 100;
            RAM[I] ++;
        }

        while(val >= 10) {
            val -= 10;
            RAM[I+1] ++;
        }

        while(val >= 1) {
            val -= 1;
            RAM[I+2] ++;
        }

        PC += 2;
        return ERROR::NONE;

    // Fx55 - LD [I], Vx
    }else if(code == 0xF && RAM[PC+1] == 0x55) {
        uint8 reg = RAM[PC] & 0b00001111;

        for(uint8 i = 0; i <= reg; i ++)
            RAM[I + i] = V[i];

        PC += 2;
        return ERROR::NONE;

    // Fx65 - LD Vx, [I]
    }else if(code == 0xF && RAM[PC+1] == 0x65) {
        uint8 reg = RAM[PC] & 0b00001111;

        for(uint8 i = 0; i <= reg; i ++)
            V[i] = RAM[I + i];

        PC += 2;
        return ERROR::NONE;
    }
    return ERROR::ILLEGAL_INSTRUCTION;
}

void chip8::timer() {
    if(DT > 0) DT --;
    if(ST > 0) ST --;
}