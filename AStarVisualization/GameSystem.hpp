#pragma once
#include "Scene.hpp"
#include "SFML/Graphics.hpp"
#include "Vec2.hpp"
#include <iostream>
#include <algorithm>
class GameSystem
{	
	GameConfig& m_config;
	sf::RenderWindow m_window;
	std::unique_ptr<Scene> m_curScene;
	void render();
	void update();
	void setScene(std::unique_ptr<Scene>&& scene);
	void handleUserInput();
public:
	GameSystem();
	void run();
};
