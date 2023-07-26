#pragma once
#include "MainScene.hpp"
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

