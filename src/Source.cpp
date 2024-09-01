#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <vector>

#include "Map.hpp"
#include "my_utils.hpp"
#include "map_porting.hpp"

int main() {
	sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Platform Shooter Map Builder", sf::Style::Fullscreen);

	Map map;

	sf::Vector2f offset = { 0.f, 0.f };
	sf::Vector2i last_mouse_pos = sf::Mouse::getPosition();
	float scale = 1.f;
	bool LMB_was_pressed = false;
	bool RMB_was_pressed = false;

	// CONSTANTS
	const int tile_size = 16;
	const float step = 150.f;
	const int minitiles_per_row = 6;
	const float minitile_space_between = 14.f;
	const float minitile_box_size = window.getSize().x * 0.25f;
	float minitile_size = (minitile_box_size - minitile_space_between * (minitiles_per_row + 1)) / minitiles_per_row;
	// ------------

	std::vector<std::string> tilenames = get_tilenames();
	sf::Texture tile_texture = get_tile_textures(tilenames);
	sf::Sprite tile_sprite;
	tile_sprite.setTexture(tile_texture);

	sf::Texture buttons_texture = get_button_textures();
	sf::Sprite close_button_sprite, menu_button_sprite, load_button_sprite, save_button_sprite;
	int button_size = buttons_texture.getSize().y;

	sf::IntRect menu_button_rect = get_menu_button_rect(window, button_size * 4);
	sf::IntRect close_button_rect = get_close_button_rect(window, button_size * 4);
	sf::IntRect load_button_rect = get_load_button_rect(window, button_size * 4);
	sf::IntRect save_button_rect = get_save_button_rect(window, button_size * 4);
	bool menu_open = false, solidity_mode = false;
	bool menu_button_hover = true, close_button_hover = true, load_button_hover = true, save_button_hover = true;
	bool scroll_down_was_pressed = false;
	bool scroll_up_was_pressed = false;
	int starting_row = 0;
	int selected_tile = 0;
	sf::Vector2i last_changed_tile;

	menu_button_sprite.setPosition((float)menu_button_rect.left, (float)menu_button_rect.top);
	menu_button_sprite.setTexture(buttons_texture);
	menu_button_sprite.setScale({ 4.f, 4.f });

	close_button_sprite.setPosition((float)close_button_rect.left, (float)close_button_rect.top);
	close_button_sprite.setTexture(buttons_texture);
	close_button_sprite.setScale({ 4.f, 4.f });

	load_button_sprite.setPosition((float)load_button_rect.left, (float)load_button_rect.top);
	load_button_sprite.setTexture(buttons_texture);
	load_button_sprite.setScale({ 4.f, 4.f });

	save_button_sprite.setPosition((float)save_button_rect.left, (float)save_button_rect.top);
	save_button_sprite.setTexture(buttons_texture);
	save_button_sprite.setScale({ 4.f, 4.f });

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

					starting_row = std::min(starting_row, (int)tilenames.size() / minitiles_per_row - 1);
					starting_row = std::max(starting_row, 0);
				}
				break;
			}
		}

		// INPUT HANDLING

		if (menu_open && sf::Mouse::getPosition().x >= window.getSize().x * 0.7f) { // Hovers on the menu
			if (!scroll_up_was_pressed && (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W)))
				starting_row--;
			if (!scroll_down_was_pressed && (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S)))
				starting_row++;
			starting_row = std::min(starting_row, (int)tilenames.size() / minitiles_per_row - 1);
			starting_row = std::max(starting_row, 0);

			sf::IntRect minitile_box(int(0.725f * window.getSize().x + minitile_space_between * 0.5f), int(20.f + button_size * 4.f + minitile_space_between * 0.5f), int(minitile_box_size - minitile_space_between), int(minitile_box_size - minitile_space_between));
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && minitile_box.contains(sf::Mouse::getPosition().x, sf::Mouse::getPosition().y)) {
				sf::Vector2f look_at(float(sf::Mouse::getPosition().x), float(sf::Mouse::getPosition().y));
				look_at.x -= minitile_box.left;
				look_at.y -= minitile_box.top;
				look_at /= minitile_size + minitile_space_between;
				selected_tile = (starting_row + int(look_at.y)) * minitiles_per_row + int(look_at.x);
				selected_tile = std::min(int(tilenames.size()) - 1, selected_tile);
			}

			if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !LMB_was_pressed) {
				if (close_button_rect.contains(sf::Mouse::getPosition()))
					window.close();

				if (load_button_rect.contains(sf::Mouse::getPosition())) {
					std::string input_str;
					std::cout << "Enter the name of the map to be loaded:\n";
					std::cin >> input_str;

					if (input_str == ".") {
						std::cout << "You opted for the creation of a new map\n";
						int x, y;
						std::cout << "Size X = ";
						std::cin >> x;
						std::cout << "Size Y = ";
						std::cin >> y;

						map.resize(x, y);
						tilenames = get_tilenames();
						tile_texture = get_tile_textures(tilenames);
						tile_sprite.setTexture(tile_texture);
					}
					else {
						map_load_from_file(input_str, map);
						tilenames = get_tilenames();
						tile_texture = get_tile_textures(tilenames);
						tile_sprite.setTexture(tile_texture);
					}
					std::cin.sync();
					std::cout << "Successfully loaded the map!\n\n";
				}

				if (save_button_rect.contains(sf::Mouse::getPosition())) {
					std::string input_str;
					std::cout << "Enter the name of the new map:\n";
					std::cin >> input_str;

					map_save_to_file(input_str, map);

					std::cout << "Successfully saved the new map!\n\n";
					std::cin.sync();
				}
			}
		}
		else {
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A))
				offset.x += step * elapsed_time * scale;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))
				offset.x -= step * elapsed_time * scale;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W))
				offset.y += step * elapsed_time * scale;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S))
				offset.y -= step * elapsed_time * scale;

			if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && RMB_was_pressed) {
				sf::Vector2i tmp = sf::Mouse::getPosition() - last_mouse_pos;
				offset.x += tmp.x;
				offset.y += tmp.y;
			}

			sf::IntRect tile_box = sf::IntRect(int(offset.x), int(offset.y), int(map.size.x * tile_size * scale), int(map.size.y * tile_size * scale));
			if (tile_box.contains(sf::Mouse::getPosition()) && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
				sf::Vector2i pos = sf::Mouse::getPosition();
				float tmp = 1.f / (scale * tile_size);
				pos.x = int((pos.x - offset.x) * tmp);
				pos.y = int((pos.y - offset.y) * tmp);

				if (!solidity_mode) {
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
						map.fill_set_block(pos, selected_tile);
					else
						map.set_block(pos, selected_tile);
				}
				else {
					if (last_changed_tile != pos) {
						if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
							map.fill_set_solidity(last_changed_tile = pos, !map.get_solidity(pos));
						else
							map.inverse_solidity(last_changed_tile = pos);
					}
				}
			}
		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !LMB_was_pressed && menu_button_rect.contains(sf::Mouse::getPosition())) {
			menu_open = !menu_open;
		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Middle)) {
			sf::IntRect tile_box = sf::IntRect(int(offset.x), int(offset.y), int(map.size.x * tile_size * scale), int(map.size.y * tile_size * scale));

			if (tile_box.contains(sf::Mouse::getPosition())) {
				sf::Vector2i pos = sf::Mouse::getPosition();
				float tmp = 1.f / (scale * tile_size);
				pos.x = int((pos.x - offset.x) * tmp);
				pos.y = int((pos.y - offset.y) * tmp);

				selected_tile = map.get_block(pos);
			}
		}

		solidity_mode = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift);
		scroll_up_was_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W);
		scroll_down_was_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S);
		LMB_was_pressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);
		RMB_was_pressed = sf::Mouse::isButtonPressed(sf::Mouse::Right);
		last_mouse_pos = sf::Mouse::getPosition();

		// DRAW
		window.clear(sf::Color(151, 151, 151));

		if (!solidity_mode) {
			tile_sprite.setScale({ scale, scale });
			for (int i = 0; i < map.size.y; i++) {
				for (int j = 0; j < map.size.x; j++) {
					sf::Vector2f pos = sf::Vector2f(float(j * tile_size), float(i * tile_size));
					int value = map.get_block(j, i);

					tile_sprite.setPosition(pos * scale + offset);
					tile_sprite.setTextureRect(sf::IntRect(value * tile_size, 0, tile_size, tile_size));

					window.draw(tile_sprite);
				}
			}
		}
		else {
			sf::RectangleShape shape;
			shape.setSize({ scale * tile_size, scale * tile_size });
			for (int i = 0; i < map.size.y; i++) {
				for (int j = 0; j < map.size.x; j++) {
					sf::Vector2f pos = sf::Vector2f(float(j * tile_size), float(i * tile_size));
					int value = !map.get_solidity(j, i);

					shape.setPosition(pos * scale + offset);
					shape.setFillColor(sf::Color(value * 255, value * 255, value * 255));

					window.draw(shape);
				}
			}
		}

		for (int i = 0; i <= map.size.x; i++)
			line(window, sf::Vector2f(float(i * tile_size), 0.f) * scale + offset, sf::Vector2f(float(i * tile_size), float(map.size.y * tile_size)) * scale + offset);

		for (int i = 0; i <= map.size.y; i++)
			line(window, sf::Vector2f(0.f, float(i * tile_size)) * scale + offset, sf::Vector2f(float(map.size.x * tile_size), float(i * tile_size)) * scale + offset);

		// DRAW the menu
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
			box.setSize(sf::Vector2f(minitile_box_size, minitile_box_size));
			window.draw(box);

			// Highlight the selected minitile
			sf::Vector2f minitile_box_position = box.getPosition();
			if (selected_tile >= starting_row * minitiles_per_row && selected_tile < std::min(int(tilenames.size()), ((starting_row + minitiles_per_row) * minitiles_per_row))) {
				int col = selected_tile % minitiles_per_row;
				int row = (selected_tile - starting_row * minitiles_per_row) / minitiles_per_row;
				float x = minitile_space_between * (col)+minitile_size * col + minitile_box_position.x + minitile_space_between * 0.5f;
				float y = minitile_space_between * (row)+minitile_size * row + minitile_box_position.y + minitile_space_between * 0.5f;

				box.setPosition(x, y);
				box.setFillColor(sf::Color(255, 0, 0, 180 + 75 * is_mouse_on_menu));
				box.setSize(sf::Vector2f(minitile_space_between + minitile_size, minitile_space_between + minitile_size));

				window.draw(box);
			}

			// Draw every minitile in the box
			box.setSize({ minitile_size, minitile_size });
			box.setFillColor(sf::Color(255, 0, 0));
			tile_sprite.setScale({ minitile_size / tile_size, minitile_size / tile_size });
			for (int j = 0, i = starting_row * minitiles_per_row, last_block = -1; i < (int)tilenames.size() && j < minitiles_per_row * minitiles_per_row; i++, j++) {
				int col = j % minitiles_per_row;
				int row = j / minitiles_per_row;
				float x = minitile_space_between * (col + 1) + minitile_size * col + minitile_box_position.x;
				float y = minitile_space_between * (row + 1) + minitile_size * row + minitile_box_position.y;

				tile_sprite.setPosition(x, y);
				if (i != last_block)
					tile_sprite.setTextureRect(sf::IntRect((last_block = i) * tile_size, 0, tile_size, tile_size));

				window.draw(tile_sprite);
			}

			if (!close_button_rect.contains(sf::Mouse::getPosition())) {
				if (close_button_hover) {
					close_button_sprite.setTextureRect(sf::IntRect(0, 0, button_size, button_size));
					close_button_sprite.setColor(sf::Color(255, 255, 255, 127 + 128 * menu_open));
					close_button_hover = false;
				}
			}
			else {
				if (!close_button_hover) {
					close_button_sprite.setTextureRect(sf::IntRect(button_size, 0, button_size, button_size));
					close_button_sprite.setColor(sf::Color(255, 255, 255, 255));
					close_button_hover = true;
				}
			}

			window.draw(close_button_sprite);

			if (!load_button_rect.contains(sf::Mouse::getPosition())) {
				if (load_button_hover) {
					load_button_sprite.setTextureRect(sf::IntRect(button_size * 2, 0, button_size, button_size));
					load_button_sprite.setColor(sf::Color(255, 255, 255, 127 + 128 * menu_open));
					load_button_hover = false;
				}
			}
			else {
				if (!load_button_hover) {
					load_button_sprite.setTextureRect(sf::IntRect(button_size * 3, 0, button_size, button_size));
					load_button_sprite.setColor(sf::Color(255, 255, 255, 255));
					load_button_hover = true;
				}
			}
			window.draw(load_button_sprite);

			if (!save_button_rect.contains(sf::Mouse::getPosition())) {
				if (save_button_hover) {
					save_button_sprite.setTextureRect(sf::IntRect(button_size * 6, 0, button_size, button_size));
					save_button_sprite.setColor(sf::Color(255, 255, 255, 127 + 128 * menu_open));
					save_button_hover = false;
				}
			}
			else {
				if (!save_button_hover) {
					save_button_sprite.setTextureRect(sf::IntRect(button_size * 7, 0, button_size, button_size));
					save_button_sprite.setColor(sf::Color(255, 255, 255, 255));
					save_button_hover = true;
				}
			}
			window.draw(save_button_sprite);
		}

		if (!menu_button_rect.contains(sf::Mouse::getPosition())) {
			if (menu_button_hover) {
				menu_button_sprite.setTextureRect(sf::IntRect(button_size * 4, 0, button_size, button_size));
				menu_button_sprite.setColor(sf::Color(255, 255, 255, 127 + 128 * menu_open));
				menu_button_hover = false;
			}
		}
		else {
			if (!menu_button_hover) {
				menu_button_sprite.setTextureRect(sf::IntRect(button_size * 5, 0, button_size, button_size));
				menu_button_sprite.setColor(sf::Color(255, 255, 255, 255));
				menu_button_hover = true;
			}
		}
		window.draw(menu_button_sprite);

		window.display();
	}

	return 0;
}
