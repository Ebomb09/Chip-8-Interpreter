#include <iostream>
#include <iomanip>
#include <cstring>
#include "SDL3/SDl.h"
#include "chip8.h"

int main(int argc, char* argv[]) {

    // Program options
    int opt_scale = 10;
    int opt_clock = 1000;
    bool opt_help = false;
    int opt_program = -1;

    for(int i = 1; i < argc; i ++) {
        std::string option = argv[i];
        
        if(option == "--help") {
            opt_help = true;

        }else if(option == "--scale") {
            i ++;
            if(i < argc) opt_scale = std::stoi(argv[i]);

        }else if(option == "--clock") {
            i ++;
            if(i < argc) opt_clock = std::stoi(argv[i]);

        }else {
            opt_program = i;
        }
    }

    if(opt_help || opt_program == -1) {
        std::cout << "Usage: " << argv[0] << " [options] program\n";
        std::cout << "\t--scale <display scale>\tSets the base pixel size of the original 64x32 screen\n";
        std::cout << "\t--clock <clock rate>\tSets the number of instructions completed per second";
        return 1;
    }

    // Initialize SDL
    if(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        std::cerr << "SDLInitError: " << SDL_GetError();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Chip-8 Interpreter", chip8::DISPLAY_WIDTH * opt_scale, chip8::DISPLAY_HEIGHT * opt_scale, 0);
    if(!window) {
        std::cerr << "SDLWindowError: " << SDL_GetError();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    if(!renderer) {
        std::cerr << "SDLRendererError: " << SDL_GetError();
        return 1;
    }

    // Load the tone sound effect
    Uint8* audio_buffer;
    Uint32 audio_length;
    SDL_AudioSpec audio_spec;
    if(!SDL_LoadWAV("std/tone.wav", &audio_spec, &audio_buffer, &audio_length)) {
        std::cerr << "SDLAudioError: " << SDL_GetError();
        return 1;
    }

    // Open an audio stream
    SDL_AudioStream* audio_stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audio_spec, NULL, NULL);
    if(audio_stream == NULL) {
        std::cerr << "SDLAudioError: " << SDL_GetError();
        return 1;
    }
    SDL_ResumeAudioStreamDevice(audio_stream);

    // Initialize the interpreter
    chip8 interp;

    int success = interp.init(argv[opt_program]);
    if(success != chip8::ERROR::NONE) {

        switch(success) {

        case chip8::ERROR::STD_FILE_NOT_FOUND:
            std::cerr << "InitError: std/reserved.ch8 not found\n";
            break;

        case chip8::ERROR::PROGRAM_FILE_NOT_FOUND:
            std::cerr << "InitError: " << argv[opt_program] << " not found\n";
            break;
        }

        return 1;
    }

    bool exit = false;

    const int CLOCK_RATE = opt_clock;   // Hz
    const int DELAY_RATE = 60;          // Hz

    Uint64 clock = SDL_GetTicks();
    Uint64 delay = SDL_GetTicks();

    while(!exit) {

        // Play tone when SountTimer is active and no tone is played
        if(interp.ST > 0 && SDL_GetAudioStreamAvailable(audio_stream) < (int)audio_length) {
            SDL_PutAudioStreamData(audio_stream, audio_buffer, audio_length);
        }

        // Clock
        if(SDL_GetTicks() - clock >= 1000 / CLOCK_RATE) {

            if(interp.step() != chip8::ERROR::NONE) {
                std::cerr << "RuntimeError: Illegal Instruction at: " << interp.PC << "\n";
                exit = true;
            }
            clock = SDL_GetTicks();
        }

        // Delay
        if(SDL_GetTicks() - delay >= 1000 / DELAY_RATE) {
            interp.timer();

            // Render pixels
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            for(int i = 0; i < chip8::DISPLAY_WIDTH; i ++) {
                for(int j = 0; j < chip8::DISPLAY_HEIGHT; j ++) {
    
                    if(interp.PIXEL[i][j] == 1) {
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                        SDL_FRect dest {
                            (float)(i * opt_scale),
                            (float)(j * opt_scale),
                            (float)(opt_scale),
                            (float)(opt_scale)
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

    SDL_free(audio_buffer);
    SDL_DestroyAudioStream(audio_stream);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}