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

	/*
	// Draw a Box for the focused entity
	if (m_focus) {
		if (m_focus->hasComponent<CText>()) {
			auto cText = m_focus->getComponent<CText>();
			auto bound = cText->states.transform.transformRect(cText->text.getGlobalBounds());
			auto rect = sf::RectangleShape(sf::Vector2f(bound.width + 2, bound.height + 2));
			rect.setFillColor(sf::Color::Transparent);
			rect.setOutlineColor(sf::Color::White);
			rect.setOutlineThickness(1);
			rect.setPosition(bound.left - 1, bound.top - 1);
			m_window.draw(rect);
		}
	}
	*/
	

	m_window.display();
}

void GameSystem::handleUserInput() {
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			m_window.close();
		else if (event.type == sf::Event::Resized)
		{
			std::cout << "new width: " << event.size.width << std::endl;
			std::cout << "new height: " << event.size.height << std::endl;
		}
		else if (event.type == sf::Event::LostFocus)
			std::cout << "LostFocus\n";

		else if (event.type == sf::Event::GainedFocus)
			std::cout << "GainedFocus\n";

		else if (event.type == sf::Event::MouseEntered)
			std::cout << "the mouse cursor has entered the window" << std::endl;

		else if (event.type == sf::Event::MouseLeft)
			std::cout << "the mouse cursor has left the window" << std::endl;
		else {
			m_curScene->handleKeyBoardInput(event);
			// Handle click
			m_curScene->handleMouseInput(event);
		}

	}
}

void GameSystem::update() {
	auto manager = m_curScene->getManager();
	manager->update();
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