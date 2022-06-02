#include "chip.hpp"
#include <algorithm>
#include <random>
constexpr uint16_t get_addr(uint16_t opcode) {
    return opcode & 0x0FFF;
}
constexpr uint16_t get_nibble(uint16_t opcode) {
    return opcode & 0x000F;
}
constexpr uint16_t get_x(uint16_t opcode) {
    return (opcode & 0x0F00) >> 8;
}
constexpr uint16_t get_y(uint16_t opcode) {
    return (opcode & 0x00F0) >> 4;
}
constexpr uint16_t get_kk(uint16_t opcode) {
    return (opcode & 0x00FF);
}

static_assert(get_addr(0xABCD) == 0xBCD);
static_assert(get_nibble(0xABCD) == 0xD);
static_assert(get_x(0xABCD) == 0xB);
static_assert(get_y(0xABCD) == 0xC);
static_assert(get_kk(0xABCD) == 0xCD);

void Chip8::tick() {
    uint16_t opcode;
    std::memcpy(&opcode, &memory[pc], sizeof(opcode));
    auto x = get_x(opcode);
    auto y = get_y(opcode);
    auto kk = get_kk(opcode);
    auto addr = get_addr(opcode);
    auto nibble = get_nibble(opcode);
    switch(opcode & 0xF000) {
        case 0x0:
            switch(opcode & 0x00FF) {
                case 0x00E0:
                    std::ranges::fill(display, 0);
                    pc += 2;
                    break;
                case 0x00EE:
                    pc = stack.top();
                    stack.pop();
                    pc -=1;
                    break;
                default:
                    pc = addr;
                    break;
            }
        break;
        case 0x1000:
            pc = addr;
            break;
        case 0x2000:
            stack.push(pc);
            pc = addr;
            break;
        case 0x3000:
            if(V[x] == kk)
                pc += 2;
            pc += 2;
            break;
        case 0x4000:
            if(V[x] != kk)
                pc += 2;
            pc += 2;
            break;
        case 0x5000:
            if(V[x] == V[y])
                pc += 2;
            pc += 2;
            break;
        case 0x6000:
            V[x] = static_cast<uint8_t>(kk);
            pc += 2;
            break;
        case 0x7000:
            V[x] += kk;
            pc += 2;
            break;
        case 0x8000:
            switch(opcode & 0x000F) {
                case 0x0:
                    V[x] = V[y];
                    pc += 2;
                    break;
                case 0x0001:
                    V[x] |= V[y];
                    pc += 2;
                    break;
                case 0x0002:
                    V[x] &= V[y];
                    pc += 2;
                    break;
                case 0x0003:
                    V[x] ^= V[y];
                    pc += 2;
                    break;
                case 0x0004: {

                    auto temp = V[x];
                    V[x] += V[y];
                    V[0xF] = temp > V[x] ? 1 : 0;
                    pc += 2;
                    break;
                }
                case 0x0005:
                    V[0xF] = V[y] > V[x] ? 1 : 0;
                    V[x] -= V[y];
                    pc += 2;
                    break;
                case 0x0006:
                    V[0xF] = V[x] & 1;
                    V[x] >>= 1;
                    pc += 2;
                    break;
                case 0x0007:
                    V[0xF] = V[x] > V[y] ? 1 : 0;
                    V[x] = V[y] - V[x];
                    pc += 2;
                    break;
                case 0x000E:
                    V[0xF] = (V[x] >> 7) & 1;
                    V[x] <<= 1;
                    pc += 2;
                    break;
            }
        case 0x9000:
            if(V[x] != V[y])
                pc += 2;
            pc += 2;
            break;
        case 0xA000:
            I = addr;
            pc += 2;
            break;
        case 0xB000:
            pc = V[0] + addr;
            break;
        case 0xC000:
        {
            auto random = 155;
            V[x] = random & kk;
            pc += 2;
        }
            break;
        case 0xD000:
            //todo
            pc += 2;
            break;
        case 0xE000:
            switch(opcode & 0x00FF) {
                case 0x9E:
                    if(keyboard[V[x]] == 1)
                        pc += 2;
                    pc += 2;
                    break;
                case 0xA1:
                    if(keyboard[V[x]] != 1)
                        pc += 2;
                    pc += 2;
                    break;
            }
            break;
        case 0xF000:
            switch(opcode & 0x00FF) {
                case 0x07:
                    V[x] = delay;
                    pc += 2;
                    break;
                case 0x0A:
                    if(std::ranges::any_of(keyboard, [](uint8_t key){return key == 1;})) {
                        auto iter = std::ranges::find(keyboard, 1);
                        V[x] = static_cast<uint8_t>(std::distance(keyboard.begin(), iter));
                        pc += 2;
                    }
                    break;
                case 0x15:
                    delay = V[x];
                    pc += 2;
                    break;
                case 0x18:
                    sound_delay = V[x];
                    pc += 2;
                    break;
                case 0x1E:
                    I += V[x];
                    pc += 2;
                    break;
                case 0x29:
                    I = 5 * V[x];
                    pc += 2;
                    break;
                case 0x33:
                    memory[I] = V[x] / 100;
                    memory[I + 1] = (V[x] % 100) / 10;
                    memory[I + 2] = V[x] % 10;
                    pc += 2;
                    break;
                case 0x55:
                    std::copy(V.begin(), V.end(), memory.begin() + I);
                    pc += 2;
                    break;
                case 0x65:
                    std::copy_n(memory.begin() + I, V.size(), V.begin());
                    pc += 2;
                    break;

            }
        
    }
    
}