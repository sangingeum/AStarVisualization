#include "GameSystem.hpp"


void GameSystem::render() {
	PROFILE_FUNCTION();
	if (m_curScene->getNeedRender()) {
		{
			PROFILE_SCOPE("Render::draw");
			m_window.clear();
			auto manager = m_curScene->getManager();
			// Draw Shapes
			for (auto& entity : manager->getEntities(ComponentType::SHAPE)) {
				auto cShape = entity->getComponent<CShape>();
				m_window.draw(cShape->vertexArray, cShape->states);
			}
			// Draw Texts
			for (auto& entity : manager->getEntities(ComponentType::TEXT)) {
				auto cText = entity->getComponent<CText>();
				m_window.draw(cText->text, cText->states);
			}
		}
		{	
			PROFILE_SCOPE("Render::display");
			m_window.display();
		}
	}
}

void GameSystem::handleUserInput() {
	PROFILE_FUNCTION();
	sf::Event event;
	while (m_window.pollEvent(event))
	{	
		if (event.type == sf::Event::Closed)
			m_window.close();
		// Let the current scent to handle user inputs
		m_curScene->handleKeyBoardInput(event);
		m_curScene->handleMouseInput(event);
	}
	
}

void GameSystem::update() {
	PROFILE_FUNCTION();
	auto manager = m_curScene->getManager();
	manager->update();
	m_curScene->update(m_window);
}

void GameSystem::setScene(std::unique_ptr<Scene>&& scene) {
	m_curScene = std::move(scene);
	m_curScene->init();
}

GameSystem::GameSystem() : m_config(GameConfig::instance()), m_window(sf::VideoMode(m_config.windowWidth, m_config.widowHeight), m_config.windowName.c_str())
{
	m_window.setFramerateLimit(m_config.frameRate);
	setScene(std::make_unique<MainScene>());
}

void GameSystem::wait() {
	PROFILE_FUNCTION();
	using namespace std::chrono_literals;
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(m_clock.now() - m_loopStartTime);
	auto remainingTime = std::chrono::milliseconds(m_config.miliPerFrame) - elapsed;
	if (remainingTime > std::chrono::milliseconds(0)) {
		std::this_thread::sleep_for(remainingTime);
	}
}

void GameSystem::run() {
	while (m_window.isOpen())
	{
		PROFILE_SCOPE("Game Loop");
		m_loopStartTime = m_clock.now();
		handleUserInput();
		update();
		render();
		wait();
	}
}