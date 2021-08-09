#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <vector>

#include "Map.hpp"
#include "my_utils.hpp"

const float tile_size = 16.f;
const float step = 150.f;

int main() {
	std::vector<std::string> tilenames = get_tilenames();
	if (tilenames.empty()) {
		std::cout << "No textures loaded\n";
		system("pause");
	}
	else {
		std::cout << "Successfully loaded " << tilenames.size() << " textures\n";

		Map map(60, 40);
		sf::Vector2f offset = { 0.f, 0.f };
		sf::Vector2i last_mouse_pos = sf::Mouse::getPosition();
		float scale = 1.f;
		bool LMB_was_pressed = false;

		sf::Texture texture = get_texture(tilenames);
		sf::Sprite sprite;
		sprite.setTexture(texture);
		
		sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Platform Shooter Map Builder", sf::Style::Fullscreen);
		
		sf::Event event;
		std::chrono::system_clock::time_point last_frame_start = std::chrono::system_clock::now();
		while (window.isOpen()) {
			std::chrono::duration<float> elapsed_time_duration = std::chrono::system_clock::now() - last_frame_start;
			float elapsed_time = elapsed_time_duration.count();
			last_frame_start = std::chrono::system_clock::now();

			while (window.pollEvent(event)) {
				switch (event.type) {
					case sf::Event::Closed:
						window.close();
						break;
					case sf::Event::MouseWheelMoved:
						sf::Vector2i cur_mouse_pos = sf::Mouse::getPosition();
						if (event.mouseWheel.delta > 0) {
							offset.x -= (cur_mouse_pos.x - offset.x) * 0.1f;
							offset.y -= (cur_mouse_pos.y - offset.y) * 0.1f;
							scale *= 1.1f;
						}
						else {
							offset.x += (cur_mouse_pos.x - offset.x) * 0.0909f;
							offset.y += (cur_mouse_pos.y - offset.y) * 0.0909f;
							scale *= 0.9091f;
						}
						break;
				}
			}

			// INPUT HANDLING

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
				offset.x += step * elapsed_time;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
				offset.x -= step * elapsed_time;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
				offset.y += step * elapsed_time;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
				offset.y -= step * elapsed_time;
			}
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
				if (LMB_was_pressed) {
					sf::Vector2i tmp = sf::Mouse::getPosition() - last_mouse_pos;
					offset.x += tmp.x;
					offset.y += tmp.y;
				}
				LMB_was_pressed = true;
				last_mouse_pos = sf::Mouse::getPosition();
			}
			else
				LMB_was_pressed = false;

			// DRAW

			window.clear(sf::Color(151, 151, 151));

			sprite.setScale({ scale, scale });
			int *ptr = map.grid;

			for (int i = 0; i < map.size.y; i++) {
				for (int j = 0; j < map.size.x; j++) {
					sf::Vector2f pos = { j * tile_size, i * tile_size };
					int value = *ptr++;

					sprite.setPosition(pos * scale + offset);
					sprite.setTextureRect(sf::IntRect(value * 16, 0, 16, 16));

					window.draw(sprite);
				}
			}

			for (int i = 0; i <= map.size.x; i++)
				line(window, sf::Vector2f(i * tile_size, 0.f) * scale + offset, sf::Vector2f(i * tile_size, map.size.y * tile_size) * scale + offset);
			
			for (int i = 0; i <= map.size.y; i++)
				line(window, sf::Vector2f(0.f, i * tile_size) * scale + offset, sf::Vector2f(map.size.x * tile_size, i * tile_size) * scale + offset);

			window.display();
		}

	}
	return 0;
}
