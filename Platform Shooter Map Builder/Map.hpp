#pragma once

struct Map {
	int *grid;
	sf::Vector2i size;

	Map(int _size_x, int _size_y) {
		size.x = _size_x;
		size.y = _size_y;

		grid = new int[size.x * size.y];
		zerofy();
	}

	void zerofy() {
		for (int i = 0; i < size.y * size.x; i++)
			grid[i] = 0;
	}

	void load_from_file() {
	}

	void dump_to_file() {
	}
};