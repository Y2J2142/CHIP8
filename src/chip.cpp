#include "chip.hpp"
#include <algorithm>
#include <bitset>
#include <fmt/core.h>
#include <fstream>
#include <random>

constexpr uint16_t get_addr(uint16_t opcode) { return opcode & 0x0FFF; }
constexpr uint8_t get_nibble(uint16_t opcode) { return opcode & 0x000F; }
constexpr uint8_t get_x(uint16_t opcode) { return (opcode & 0x0F00) >> 8; }
constexpr uint8_t get_y(uint16_t opcode) { return (opcode & 0x00F0) >> 4; }
constexpr uint8_t get_kk(uint16_t opcode) { return (opcode & 0x00FF); }

uint8_t random(short low, short high) {
  static std::random_device rd{};
  static std::mt19937 mt{rd()};
  std::uniform_int_distribution<short> dist(low, high);
  return dist(mt);
}

static_assert(get_addr(0xABCD) == 0xBCD);
static_assert(get_nibble(0xABCD) == 0xD);
static_assert(get_x(0xABCD) == 0xB);
static_assert(get_y(0xABCD) == 0xC);
static_assert(get_kk(0xABCD) == 0xCD);

void Chip8::tick() {
  const uint16_t opcode = (memory[pc] << 8) | memory[pc + 1];
  // fmt::print("opcode = {:X}\n", opcode);
  // fmt::print("pc = {:X}\n", pc);
  auto x = get_x(opcode);
  auto y = get_y(opcode);
  auto kk = get_kk(opcode);
  auto addr = get_addr(opcode);
  auto nibble = get_nibble(opcode);
  if (delay > 0)
    delay -= 1;
  if (sound_delay > 0)
    sound_delay -= 1;
  switch (opcode & 0xF000) {
  case 0x0:
    switch (opcode & 0x00FF) {
    case 0x00E0:
      std::ranges::fill(display.display, 0);
      break;
    case 0x00EE:
      pc = stack.top();
      stack.pop();
      pc -= 2;
      break;
    default:
      pc = addr;
      pc -= 2;
      break;
    }
    break;
  case 0x1000:
    pc = addr;
    pc -= 2;
    break;
  case 0x2000:
    stack.push(pc + 2);
    pc = addr;
    pc -= 2;
    break;
  case 0x3000:
    if (V[x] == kk)
      pc += 2;
    break;
  case 0x4000:
    if (V[x] != kk)
      pc += 2;
    break;
  case 0x5000:
    if (V[x] == V[y])
      pc += 2;
    break;
  case 0x6000:
    V[x] = kk;
    break;
  case 0x7000:
    V[x] += kk;
    break;
  case 0x8000:
    switch (opcode & 0x000F) {
    case 0x0:
      V[x] = V[y];
      break;
    case 0x0001:
      V[x] |= V[y];
      break;
    case 0x0002:
      V[x] &= V[y];
      break;
    case 0x0003:
      V[x] ^= V[y];
      break;
    case 0x0004: {
      auto sum = std::size_t(V[x]) + std::size_t(V[y]);

      V[x] = static_cast<uint8_t>(sum);
      V[0xF] = sum > 0xFF ? 1 : 0;
    } break;
    case 0x0005:
      V[0xF] = V[x] > V[y] ? 1 : 0;
      V[x] -= V[y];
      break;
    case 0x0006:
      V[0xF] = V[x] & 1;
      V[x] >>= 1;
      break;
    case 0x0007:
      V[0xF] = V[y] > V[x] ? 1 : 0;
      V[x] = V[y] - V[x];
      break;
    case 0x000E:
      V[0xF] = (V[x] >> 7) & 1;
      V[x] <<= 1;
      break;
    }
    break;
  case 0x9000:
    if (V[x] != V[y])
      pc += 2;
    break;
  case 0xA000:
    I = addr;
    break;
  case 0xB000:
    pc = V[0] + addr;
    break;
  case 0xC000: {
    auto rng = random(0, 0xFF);
    V[x] = rng & kk;
    pc += 2;
  } break;
  case 0xD000: {
    const auto width = 8;
    const auto height = nibble;
    V[0xF] = 0;

    for (auto i = 0u; i < height; i++) {
      auto sprite = memory[I + i];
      for (auto j = 0u; j < width; j++) {
        if ((sprite & 0x80) > 0)
          if (display.set_pixel(V[x] + j, V[y] + i))
            V[0xF] = 1;
        sprite <<= 1;
      }
    }
  } break;
  case 0xE000:
    switch (opcode & 0x00FF) {
    case 0x9E:
      if (keyboard[V[x]] == 1)
        pc += 2;
      break;
    case 0xA1:
      if (keyboard[V[x]] != 1)
        pc += 2;
      break;
    }
    break;
  case 0xF000:
    switch (opcode & 0x00FF) {
    case 0x07:
      V[x] = delay;
      break;
    case 0x0A:
      if (std::ranges::any_of(keyboard, [](uint8_t key) { return key == 1; })) {
        auto iter = std::ranges::find(keyboard, 1);
        V[x] = static_cast<uint8_t>(std::distance(keyboard.begin(), iter));
      } else
        pc -= 2;
      break;
    case 0x15:
      delay = V[x];
      break;
    case 0x18:
      sound_delay = V[x];
      break;
    case 0x1E:
      I += V[x];
      break;
    case 0x29:
      I = 5 * V[x];
      break;
    case 0x33:
      memory[I] = V[x] / 100;
      memory[I + 1] = (V[x] % 100) / 10;
      memory[I + 2] = V[x] % 10;
      break;
    case 0x55:
      for (auto i = 0u; i <= x; i++)
        memory[I + i] = V[i];

      break;
    case 0x65:
      for (auto i = 0u; i <= x; i++)
        V[i] = memory[I + i];
      break;
    }
  }
  pc += 2;
}

uint8_t Display::set_pixel(uint8_t x, uint8_t y) {
  auto xx = x % width;
  auto yy = y % height;

  auto collision = display[xx + (yy * width)] & 1;
  display[xx + (yy * width)] ^= 1;
  return collision;
}

void Chip8::load_rom(const std::string &str) {
  std::ifstream stream{str, std::ios::binary};
  if (!stream.is_open())
    throw std::exception{};
  std::copy(std::istreambuf_iterator<char>(stream),
            std::istreambuf_iterator<char>{}, memory.begin() + 0x200);
}

void Chip8::clear_keyboard() { std::ranges::fill(keyboard, 0); }