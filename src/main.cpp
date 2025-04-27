#include <iostream>
#include <iomanip>
#include <cstring>
#include "SDL3/SDl.h"
#include "chip8.h"

int main(int argc, char* argv[]) {

    if(argc == 1) {
        std::cout << "Usage: " << argv[0] << " file\n";
        return 1;

    }else if(argc <= 0) {
        std::cerr << "Error: No command arguments passed to program!";
        return 1;
    }

    if(!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "Error: " << SDL_GetError();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Chip-8 Interpreter", 640, 320, NULL);

    if(!window) {
        std::cerr << SDL_GetError();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

    if(!renderer) {
        std::cerr << SDL_GetError();
        return 1;
    }

    chip8 interp;
    interp.init(argv[1]);

    bool exit = false;

    const int CLOCK_RATE = 1000; // Hz
    const int DELAY_RATE = 60;  // Hz

    Uint64 clock = SDL_GetTicks();
    Uint64 delay = SDL_GetTicks();

    while(!exit) {

        // Clock
        if(SDL_GetTicks() - clock >= 1000 / CLOCK_RATE) {
            /*std::cout << std::hex << std::setfill('0') << std::setw(2);
            std::cout << (int)interp.RAM[interp.PC];
            std::cout << (int)interp.RAM[interp.PC+1];
            std::cout << "\n";*/
            interp.step();

            clock = SDL_GetTicks();
        }

        // Delay
        if(SDL_GetTicks() - delay >= 1000 / DELAY_RATE) {
            interp.timer();

            // Render pixels
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
    
            for(int i = 0; i < 64; i ++) {
                for(int j = 0; j < 32; j ++) {
    
                    if(interp.PIXEL[i][j] == 1) {
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                        SDL_FRect dest {
                            i * 10,
                            j * 10,
                            10,
                            10
                        };
                        SDL_RenderFillRect(renderer, &dest);
                    }
                }
            }

            SDL_RenderPresent(renderer);

            delay = SDL_GetTicks();
        }

        // Gather keyboard inputs
        SDL_Event e;
        while(SDL_PollEvent(&e)) {

            if(e.type == SDL_EVENT_QUIT) {
                exit = true;

            }else if(e.type == SDL_EVENT_KEY_DOWN) {

                // Scuffed input detection
                switch(e.key.scancode) {
                    case SDL_SCANCODE_1: interp.KEYBOARD[1] = true;     interp.LAST_KEY = 1;    if(interp.WAIT_FOR_KEY) { interp.WAIT_COMPLETE = true; } break;
                    case SDL_SCANCODE_2: interp.KEYBOARD[2] = true;     interp.LAST_KEY = 2;    if(interp.WAIT_FOR_KEY) { interp.WAIT_COMPLETE = true; } break;
                    case SDL_SCANCODE_3: interp.KEYBOARD[3] = true;     interp.LAST_KEY = 3;    if(interp.WAIT_FOR_KEY) { interp.WAIT_COMPLETE = true; } break;
                    case SDL_SCANCODE_4: interp.KEYBOARD[12] = true;    interp.LAST_KEY = 12;   if(interp.WAIT_FOR_KEY) { interp.WAIT_COMPLETE = true; } break;
                    case SDL_SCANCODE_Q: interp.KEYBOARD[4] = true;     interp.LAST_KEY = 4;    if(interp.WAIT_FOR_KEY) { interp.WAIT_COMPLETE = true; } break;
                    case SDL_SCANCODE_W: interp.KEYBOARD[5] = true;     interp.LAST_KEY = 5;    if(interp.WAIT_FOR_KEY) { interp.WAIT_COMPLETE = true; } break;
                    case SDL_SCANCODE_E: interp.KEYBOARD[6] = true;     interp.LAST_KEY = 6;    if(interp.WAIT_FOR_KEY) { interp.WAIT_COMPLETE = true; } break;
                    case SDL_SCANCODE_R: interp.KEYBOARD[13] = true;    interp.LAST_KEY = 13;   if(interp.WAIT_FOR_KEY) { interp.WAIT_COMPLETE = true; } break;
                    case SDL_SCANCODE_A: interp.KEYBOARD[7] = true;     interp.LAST_KEY = 7;    if(interp.WAIT_FOR_KEY) { interp.WAIT_COMPLETE = true; } break;
                    case SDL_SCANCODE_S: interp.KEYBOARD[8] = true;     interp.LAST_KEY = 8;    if(interp.WAIT_FOR_KEY) { interp.WAIT_COMPLETE = true; } break;
                    case SDL_SCANCODE_D: interp.KEYBOARD[9] = true;     interp.LAST_KEY = 9;    if(interp.WAIT_FOR_KEY) { interp.WAIT_COMPLETE = true; } break;
                    case SDL_SCANCODE_F: interp.KEYBOARD[14] = true;    interp.LAST_KEY = 14;   if(interp.WAIT_FOR_KEY) { interp.WAIT_COMPLETE = true; } break;
                    case SDL_SCANCODE_Z: interp.KEYBOARD[10] = true;    interp.LAST_KEY = 10;   if(interp.WAIT_FOR_KEY) { interp.WAIT_COMPLETE = true; } break;
                    case SDL_SCANCODE_X: interp.KEYBOARD[0] = true;     interp.LAST_KEY = 0;    if(interp.WAIT_FOR_KEY) { interp.WAIT_COMPLETE = true; } break;
                    case SDL_SCANCODE_C: interp.KEYBOARD[11] = true;    interp.LAST_KEY = 11;   if(interp.WAIT_FOR_KEY) { interp.WAIT_COMPLETE = true; } break;
                    case SDL_SCANCODE_V: interp.KEYBOARD[15] = true;    interp.LAST_KEY = 15;   if(interp.WAIT_FOR_KEY) { interp.WAIT_COMPLETE = true; } break;
                }
                
            }else if(e.type == SDL_EVENT_KEY_UP) {

                switch(e.key.scancode) {
                    case SDL_SCANCODE_1: interp.KEYBOARD[1] = false; break;
                    case SDL_SCANCODE_2: interp.KEYBOARD[2] = false; break;
                    case SDL_SCANCODE_3: interp.KEYBOARD[3] = false; break;
                    case SDL_SCANCODE_4: interp.KEYBOARD[12] = false; break;
                    case SDL_SCANCODE_Q: interp.KEYBOARD[4] = false; break;
                    case SDL_SCANCODE_W: interp.KEYBOARD[5] = false; break;
                    case SDL_SCANCODE_E: interp.KEYBOARD[6] = false; break;
                    case SDL_SCANCODE_R: interp.KEYBOARD[13] = false; break;
                    case SDL_SCANCODE_A: interp.KEYBOARD[7] = false; break;
                    case SDL_SCANCODE_S: interp.KEYBOARD[8] = false; break;
                    case SDL_SCANCODE_D: interp.KEYBOARD[9] = false; break;
                    case SDL_SCANCODE_F: interp.KEYBOARD[14] = false; break;
                    case SDL_SCANCODE_Z: interp.KEYBOARD[10] = false; break;
                    case SDL_SCANCODE_X: interp.KEYBOARD[0] = false; break;
                    case SDL_SCANCODE_C: interp.KEYBOARD[11] = false; break;
                    case SDL_SCANCODE_V: interp.KEYBOARD[15] = false; break;
                }
                
            }
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}