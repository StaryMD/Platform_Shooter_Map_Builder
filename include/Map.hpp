#pragma once

#include <queue>

struct Map {
  sf::Vector2i size;
  uint16_t *grid;

  Map() {
    size.x = size.y = 0;
    grid = 0;
  }

  Map(int _size_x, int _size_y) {
    size.x = _size_x;
    size.y = _size_y;

    grid = new uint16_t[size.x * size.y];
    zerofy();
  }

  ~Map() {
    if (grid)
      delete[] grid;
  }

  void resize(int size_x, int size_y) {
    size = {size_x, size_y};

    if (grid)
      delete[] grid;
    grid = new uint16_t[size.x * size.y];

    zerofy();
  }

  void zerofy() {
    uint16_t *end = grid + size.x * size.y;
    for (uint16_t *it = grid; it != end; it++)
      *it = 0;
  }

  bool is_inside(sf::Vector2i &pos) {
    return pos.x >= 0 && pos.y >= 0 && pos.x < size.x && pos.y < size.y;
  }

  void set_block(int x, int y, uint16_t id) {
    grid[y * size.x + x] =
        0b0111111111111111U & id | grid[y * size.x + x] & 0b1000000000000000U;
  }

  void set_block(sf::Vector2i &pos, uint16_t id) {
    grid[pos.y * size.x + pos.x] =
        0b0111111111111111U & id |
        grid[pos.y * size.x + pos.x] & 0b1000000000000000U;
  }

  uint16_t get_block(sf::Vector2i &pos) {
    return grid[pos.y * size.x + pos.x] & 0b0111111111111111U;
  }

  uint16_t get_block(int x, int y) {
    return grid[y * size.x + x] & 0b0111111111111111U;
  }

  void fill_set_block(sf::Vector2i &pos, uint16_t id) {
    std::queue<sf::Vector2i> q;
    uint16_t type = get_block(pos);
    q.push(pos);

    while (!q.empty()) {
      sf::Vector2i cur = q.front();
      q.pop();

      if (is_inside(cur) && get_block(cur) == type && get_block(cur) != id) {
        set_block(cur, id);

        q.push(cur + sf::Vector2i(0, 1));
        q.push(cur - sf::Vector2i(0, 1));
        q.push(cur + sf::Vector2i(1, 0));
        q.push(cur - sf::Vector2i(1, 0));
      }
    }
  }

  void set_solidity(int x, int y, bool solidity) {
    grid[y * size.x + x] = 0b0111111111111111U & grid[y * size.x + x] |
                           solidity * 0b1000000000000000U;
  }

  void set_solidity(sf::Vector2i &pos, bool solidity) {
    grid[pos.y * size.x + pos.x] =
        0b0111111111111111U & grid[pos.y * size.x + pos.x] |
        solidity * 0b1000000000000000U;
  }

  void fill_set_solidity(sf::Vector2i &pos, bool solidity) {
    std::queue<sf::Vector2i> q;
    q.push(pos);

    while (!q.empty()) {
      sf::Vector2i cur = q.front();
      q.pop();

      if (is_inside(cur) && get_solidity(cur) != solidity) {
        set_solidity(cur, solidity);

        q.push(cur + sf::Vector2i(0, 1));
        q.push(cur - sf::Vector2i(0, 1));
        q.push(cur + sf::Vector2i(1, 0));
        q.push(cur - sf::Vector2i(1, 0));
      }
    }
  }

  void inverse_solidity(int x, int y, bool solidity) {
    grid[y * size.x + x] = 0b0111111111111111U & grid[y * size.x + x] |
                           (!solidity) * 0b1000000000000000U;
  }

  bool get_solidity(sf::Vector2i &pos) {
    return grid[pos.y * size.x + pos.x] >> 14;
  }

  bool get_solidity(int x, int y) { return grid[y * size.x + x] >> 14; }

  void inverse_solidity(sf::Vector2i &pos) {
    grid[pos.y * size.x + pos.x] =
        0b0111111111111111U & grid[pos.y * size.x + pos.x] |
        (!get_solidity(pos)) * 0b1000000000000000U;
  }

  uint16_t *grid_end() { return grid + size.x * size.y; }
};