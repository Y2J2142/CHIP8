#include "chip.hpp"
#include <SFML/Graphics.hpp>

int main(int argc, char **argv) {
  sf::RenderWindow window(sf::VideoMode(640, 320), "SFML window");
  window.setFramerateLimit(540);
  Chip8 chip{};
  chip.load_rom(argv[1]);
  sf::Image img;
  sf::Texture txt{};
  sf::Sprite sprite{};

  while (window.isOpen()) {
    // Process events
    sf::Event event;
    while (window.pollEvent(event)) {
      // Close window: exit
      if (event.type == sf::Event::Closed)
        window.close();
    }
    chip.clear_keyboard();
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) {
      chip.keyboard[0] = 1;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) {
      chip.keyboard[1] = 1;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) {
      chip.keyboard[2] = 1;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4)) {
      chip.keyboard[3] = 1;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
      chip.keyboard[4] = 1;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
      chip.keyboard[5] = 1;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
      chip.keyboard[6] = 1;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
      chip.keyboard[7] = 1;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
      chip.keyboard[8] = 1;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
      chip.keyboard[9] = 1;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
      chip.keyboard[10] = 1;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F)) {
      chip.keyboard[11] = 1;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
      chip.keyboard[12] = 1;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::X)) {
      chip.keyboard[13] = 1;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
      chip.keyboard[14] = 1;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::V)) {
      chip.keyboard[15] = 1;
    }
    chip.tick();
    auto &display = chip.display.display;

    img.create(64, 32);
    for (auto i = 0u; i < 64; i++)
      for (auto j = 0u; j < 32; j++)
        if (display[i + (j * 64)] == 1)
          img.setPixel(i, j, sf::Color::Cyan);
    txt.create(64, 32);
    txt.update(img);
    sprite.setTexture(txt);
    sprite.setScale(10, 10);
    window.clear();
    window.draw(sprite);

    // Clear screen
    // Update the window
    window.display();
  }
  return 0;
}