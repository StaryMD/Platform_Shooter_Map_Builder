#pragma once
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <algorithm>
#include <experimental/filesystem>

void line(sf::RenderWindow &window, sf::Vector2f a, sf::Vector2f b) {
	sf::Vertex line[] = { a, b };
	line->color = sf::Color(0, 0, 0);
	(line + 1)->color = sf::Color(0, 0, 0);
	window.draw(line, 2, sf::Lines);
}

std::vector<std::string> get_tilenames() {
	std::vector<std::string> tilenames;
	std::string path = "assets/tiles";

	for (const auto &entry : std::experimental::filesystem::directory_iterator(path)) {
		std::string tilename = entry.path().string();
		tilenames.push_back(tilename);
	}

	sort(tilenames.begin(), tilenames.end());
	return tilenames;
}

void draw_ontop(sf::Image &image, std::string &path, int x, int y) {
	sf::Image temp;
	temp.loadFromFile(path);
	int tile_size = temp.getSize().x;
	for (int i = 0; i < tile_size; i++)
		for (int j = 0; j < tile_size; j++)
			image.setPixel(j + x, i + y, temp.getPixel(j, i));
}

sf::Texture get_texture(std::vector<std::string> &tilenames) {
	sf::Image image;
	int tile_size;
	{
		sf::Image temp;
		temp.loadFromFile(tilenames[0]);
		tile_size = temp.getSize().x;

		image.create(tile_size * tilenames.size(), tile_size, sf::Color::White);
		draw_ontop(image, tilenames[0], 0, 0);
	}

	for (int i = 1; i < (int)tilenames.size(); i++) {
		draw_ontop(image, tilenames[i], i * tile_size, 0);
	}
	sf::Texture texture;
	texture.loadFromImage(image);
	return texture;
}
