#pragma once
#include "MainScene.hpp"
#include "Profiler.hpp"
#include <chrono>
#include <thread>

class GameSystem
{
	GameConfig& m_config;
	sf::RenderWindow m_window;
	std::unique_ptr<Scene> m_curScene;
	std::chrono::steady_clock m_clock;
	std::chrono::steady_clock::time_point m_loopStartTime;
	void render();
	void update();
	void setScene(std::unique_ptr<Scene>&& scene);
	void handleUserInput();
	void wait();
public:
	GameSystem();
	void run();
};

