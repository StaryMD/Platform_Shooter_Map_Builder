#pragma once

struct Map {
	int *grid, size_x, size_y;
	
	Map(int _size_x, int _size_y) {
		size_x = _size_x;
		size_y = _size_y;

		grid = new int(size_x * size_y);
	}

	void dump() {

	}

};
