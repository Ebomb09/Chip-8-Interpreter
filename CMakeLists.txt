cmake_minimum_required(VERSION 3.27)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(Chip8Interpreter)

add_executable(c8i
    src/chip8.cpp
    src/main.cpp
)

target_include_directories(c8i PUBLIC 
    src 
    ext/SDL3-3.2.10/x86_64-w64-mingw32/include
)

target_link_directories(c8i PUBLIC 
    ext/SDL3-3.2.10/x86_64-w64-mingw32/lib
)

target_link_libraries(c8i PUBLIC 
    SDL3
)