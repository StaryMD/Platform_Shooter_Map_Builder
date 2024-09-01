#pragma once
#include <algorithm>
#include <filesystem>
#include <unordered_set>

void line(sf::RenderWindow &window, sf::Vector2f a, sf::Vector2f b) {
  sf::Vertex line[] = {a, b};
  line->color = sf::Color(0, 0, 0);
  (line + 1)->color = sf::Color(0, 0, 0);
  window.draw(line, 2, sf::Lines);
}

std::vector<std::string> get_tilenames() {
  std::vector<std::string> tilenames;
  std::string path = "assets/tiles";

  for (const auto &entry : std::filesystem::directory_iterator(path)) {
    std::string tilename = entry.path().string();
    tilename.shrink_to_fit();
    tilenames.push_back(tilename);
  }
  tilenames.shrink_to_fit();

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

sf::Texture get_tile_textures(std::vector<std::string> &tilenames) {
  sf::Image image;
  int tile_size = 0;
  if (!tilenames.empty()) {
    sf::Image temp;
    temp.loadFromFile(tilenames.front());
    tile_size = temp.getSize().x;

    image.create(tile_size * tilenames.size(), tile_size, sf::Color::White);
    draw_ontop(image, tilenames.front(), 0, 0);
  }

  for (int i = 1; i < (int)tilenames.size(); i++)
    draw_ontop(image, tilenames[i], i * tile_size, 0);

  sf::Texture texture;
  texture.loadFromImage(image);
  return texture;
}

sf::Texture get_button_textures() {
  std::string path = "assets/menu";
  std::vector<std::string> button_names;

  for (const auto &entry : std::filesystem::directory_iterator(path)) {
    std::string button_name = entry.path().string();
    button_names.push_back(button_name);
  }

  sort(button_names.begin(), button_names.end());

  sf::Image image;
  int tile_size;
  {
    sf::Image temp;
    temp.loadFromFile(button_names.front());
    tile_size = temp.getSize().x;

    image.create(tile_size * button_names.size(), tile_size, sf::Color::White);
    draw_ontop(image, button_names.front(), 0, 0);
  }

  for (int i = 1; i < (int)button_names.size(); i++)
    draw_ontop(image, button_names[i], i * tile_size, 0);

  sf::Texture texture;
  texture.loadFromImage(image);
  return texture;
}

sf::IntRect get_menu_button_rect(sf::RenderWindow &window, int button_size) {
  return sf::IntRect(window.getSize().x - button_size - 10, 10, button_size,
                     button_size);
}

sf::IntRect get_close_button_rect(sf::RenderWindow &window, int button_size) {
  return sf::IntRect(window.getSize().x * 7 / 10 + 10, 10, button_size,
                     button_size);
}

sf::IntRect get_load_button_rect(sf::RenderWindow &window, int button_size) {
  return sf::IntRect(window.getSize().x * 0.725f,
                     50 + window.getSize().x / 4 + button_size, button_size,
                     button_size);
}

sf::IntRect get_save_button_rect(sf::RenderWindow &window, int button_size) {
  return sf::IntRect(window.getSize().x * 0.725f + button_size * 2,
                     50 + window.getSize().x / 4 + button_size, button_size,
                     button_size);
}

std::vector<uint16_t> get_unique_tiles(Map &map) {
  std::vector<uint16_t> unique_tiles;
  std::unordered_set<uint16_t> set;

  for (auto *it = map.grid; it != map.grid_end(); it++)
    set.insert(*it & 0b0111111111111111U);

  for (uint16_t x : set)
    unique_tiles.push_back(x);

  unique_tiles.shrink_to_fit();
  sort(unique_tiles.begin(), unique_tiles.end());
  return unique_tiles;
}
