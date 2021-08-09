#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <vector>

#include "Map.hpp"
#include "my_utils.hpp"

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

		// CONSTANTS
		const int tile_size = 16;
		const float step = 150.f;
		const int minitile_row_count = 6;
		const float minitile_space = 14.f;
		// ------------

		sf::Texture texture = get_textures(tilenames);
		sf::Sprite sprite;
		sprite.setTexture(texture);

		sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Platform Shooter Map Builder", sf::Style::Fullscreen);

		sf::Texture buttons = get_button_textures();
		sf::Sprite button_sprite;
		int button_size = buttons.getSize().y;
		sf::IntRect button_rect = get_button_rect(window, button_size * 4);
		button_sprite.setPosition(window.getSize().x - button_size * 4 - 10.f, 10);
		button_sprite.setTexture(buttons);
		button_sprite.setScale({ 4.f, 4.f });
		bool menu_open = false, button_hover = true;
		int starting_row = 0;
		bool scroll_down_was_pressed = false;
		bool scroll_up_was_pressed = false;

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
					if (!(menu_open && sf::Mouse::getPosition().x >= window.getSize().x * 0.7f)) {
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
					}
					if (menu_open && sf::Mouse::getPosition().x >= window.getSize().x * 0.7f) {
						if (event.mouseWheel.delta > 0)
							starting_row--;
						else
							starting_row++;

						starting_row = std::min(starting_row, (int)tilenames.size() / minitile_row_count - 1);
						starting_row = std::max(starting_row, 0);
					}
					break;
				}
			}

			// INPUT HANDLING

			if (!(menu_open && sf::Mouse::getPosition().x >= window.getSize().x * 0.7f)) {
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A))
					offset.x += step * elapsed_time;
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))
					offset.x -= step * elapsed_time;
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W))
					offset.y += step * elapsed_time;
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S))
					offset.y -= step * elapsed_time;
				if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
					if (LMB_was_pressed) {
						sf::Vector2i tmp = sf::Mouse::getPosition() - last_mouse_pos;
						offset.x += tmp.x;
						offset.y += tmp.y;
					}
				}
			}
			LMB_was_pressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);
			last_mouse_pos = sf::Mouse::getPosition();

			if (menu_open && sf::Mouse::getPosition().x >= window.getSize().x * 0.7f) {
				if (!scroll_up_was_pressed && (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W)))
					starting_row--;
				if (!scroll_down_was_pressed && (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S)))
					starting_row++;

				starting_row = std::min(starting_row, (int)tilenames.size() / minitile_row_count - 1);
				starting_row = std::max(starting_row, 0);
			}
			scroll_up_was_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W);
			scroll_down_was_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S);

			// DRAW

			window.clear(sf::Color(151, 151, 151));

			sprite.setScale({ scale, scale });
			int *ptr = map.grid;

			for (int i = 0; i < map.size.y; i++)
				for (int j = 0; j < map.size.x; j++) {
					sf::Vector2f pos = sf::Vector2f(float(j * tile_size), float(i * tile_size));
					int value = *ptr++;

					sprite.setPosition(pos * scale + offset);
					sprite.setTextureRect(sf::IntRect(value * tile_size, 0, tile_size, tile_size));

					window.draw(sprite);
				}

			for (int i = 0; i <= map.size.x; i++)
				line(window, sf::Vector2f(float(i * tile_size), 0.f) * scale + offset, sf::Vector2f(float(i * tile_size), float(map.size.y * tile_size)) * scale + offset);

			for (int i = 0; i <= map.size.y; i++)
				line(window, sf::Vector2f(0.f, float(i * tile_size)) * scale + offset, sf::Vector2f(float(map.size.x * tile_size), float(i * tile_size)) * scale + offset);

			if (menu_open) {
				bool is_mouse_on_menu = sf::Mouse::getPosition().x >= window.getSize().x * 0.7f;
				sf::RectangleShape box;

				// Draw the menu box
				box.setPosition(sf::Vector2f(0.7f * window.getSize().x, 0.f));
				box.setFillColor(sf::Color(51, 51, 51, 180 + 75 * is_mouse_on_menu));
				box.setSize(sf::Vector2f(0.31f * window.getSize().x, (float)window.getSize().y));
				window.draw(box);

				// Draw the box of minitiles
				box.setPosition(sf::Vector2f(0.725f * window.getSize().x, 20 + button_size * 4.f));
				box.setFillColor(sf::Color(100, 100, 100, 180 + 75 * is_mouse_on_menu));
				float minitile_box_size = window.getSize().x * 0.25f;
				box.setSize(sf::Vector2f(minitile_box_size, minitile_box_size));
				window.draw(box);

				// Draw every minitile in the box
				sf::Vector2f minitile_box_position = box.getPosition();
				float minitile_size = (minitile_box_size - minitile_space * (minitile_row_count + 1)) / minitile_row_count;
				box.setSize({ minitile_size, minitile_size });
				box.setFillColor(sf::Color(255, 0, 0));
				sprite.setScale({ minitile_size / tile_size, minitile_size / tile_size });
				for (int j = 0, i = starting_row * minitile_row_count; i < (int)tilenames.size() && j < minitile_row_count * minitile_row_count; i++, j++) {
					int col = j % minitile_row_count;
					int row = j / minitile_row_count;
					float x = minitile_space * (col + 1) + minitile_size * col + minitile_box_position.x;
					float y = minitile_space * (row + 1) + minitile_size * row + minitile_box_position.y;

					sprite.setPosition(x, y);
					sprite.setTextureRect(sf::IntRect(i * tile_size, 0, tile_size, tile_size));

					window.draw(sprite);
				}
			}
			
			if (!button_rect.contains(sf::Mouse::getPosition())) {
				if (button_hover) {
					button_sprite.setTextureRect(sf::IntRect(0, 0, button_size, button_size));
					button_sprite.setColor(sf::Color(255, 255, 255, 127 + 128 * menu_open));
					button_hover = false;
				}
			}
			else {
				if (!button_hover) {
					button_sprite.setTextureRect(sf::IntRect(button_size, 0, button_size, button_size));
					button_sprite.setColor(sf::Color(255, 255, 255, 255));
					button_hover = true;
				}

				if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !LMB_was_pressed)
					menu_open = !menu_open;
			}
			
			window.draw(button_sprite);

			window.display();
		}
	}
	return 0;
}