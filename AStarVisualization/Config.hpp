#pragma once
#include <string>
#include "SFML/Graphics.hpp"
#include <chrono>
using namespace std::chrono_literals;

// Singleton config
class GameConfig
{
	GameConfig() {
		// Load a font
		if (!font.loadFromFile("fonts/Roboto-Medium.ttf"))
			throw std::invalid_argument("Font not found");
	}
	GameConfig(const GameConfig&) = delete;
	void operator=(const GameConfig&) = delete;
	~GameConfig() = default;
public:

	static GameConfig& instance();
	unsigned frameRate = 144;
	std::chrono::milliseconds miliPerFrame = 2ms;
	int windowWidth = 1280;
	int widowHeight = 720;
	std::string windowName = "A* algorithm";
	const float pi = 3.14159265358979323846f;
	sf::Font font;

};



