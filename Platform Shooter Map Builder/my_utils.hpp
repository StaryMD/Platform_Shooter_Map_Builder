#pragma once

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
		tilename.erase(tilename.begin(), tilename.begin() + 13); // delete path

		if (tilename.substr(tilename.size() - 4, 4) != ".png") { // check if it's PNG
			std::cout << tilename << " not a PNG\n";
		}
		tilename.erase((tilename.size() - 4, 4));

		tilenames.push_back(tilename);
	}

	sort(tilenames.begin(), tilenames.end());
	return tilenames;
}
