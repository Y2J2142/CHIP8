#pragma once
#include <array>
#include <stack>

struct Chip8 {
    std::array<uint8_t, 4096> memory{0xF0,0x90,0x90,0x90,0xF0,0x20,0x60,0x20,0x20,0x70,0xF0,0x10,0xF0,0x80,0xF0,0xF0,0x10,0xF0,0x10,0xF0,0x90,0x90,0xF0,0x10,0x10,0xF0,0x80,0xF0,0x10,0xF0,0xF0,0x80,0xF0,0x90,0xF0,0xF0,0x10,0x20,0x40,0x40,0xF0,0x90,0xF0,0x90,0xF0,0xF0,0x90,0xF0,0x10,0xF0,0xF0,0x90,0xF0,0x90,0x90,0xE0,0x90,0xE0,0x90,0xE0,0xF0,0x80,0x80,0x80,0xF0,0xE0,0x90,0x90,0x90,0xE0,0xF0,0x80,0xF0,0x80,0xF0,0xF0,0x80,0xF0,0x80,0x80};
    std::array<uint8_t, 16> V{};
    uint8_t delay{};
    uint8_t sound_delay{};
    uint16_t pc{};
    uint16_t I{};
    std::stack<uint16_t> stack{};
    std::array<uint8_t, 16> keyboard{};
    std::array<uint8_t,64 * 32> display{}; 

    void tick();
};