#include "GameSystem.hpp"

static std::string str{};

void GameSystem::render() {
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
	m_window.display();
}

void GameSystem::handleUserInput() {
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
void GameSystem::run() {
	while (m_window.isOpen())
	{
		update();
		handleUserInput();
		render();
	}
}