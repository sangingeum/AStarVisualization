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

	m_window.display();
}

void GameSystem::handleUserInput() {
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			m_window.close();
		if (event.type == sf::Event::Resized)
		{
			std::cout << "new width: " << event.size.width << std::endl;
			std::cout << "new height: " << event.size.height << std::endl;
		}
		if (event.type == sf::Event::LostFocus)
			std::cout << "LostFocus\n";

		if (event.type == sf::Event::GainedFocus)
			std::cout << "GainedFocus\n";

		// Edit focused text
		else if (m_focus && event.type == sf::Event::TextEntered)
		{
			if (m_focus->hasComponent<CText>()) {
				auto cText = m_focus->getComponent<CText>();
				if (cText->canEdit) {
					const auto& curStr = cText->text.getString();
					// backSpace
					if (event.text.unicode == 8) {
						if (curStr.getSize() > 1)
							cText->text.setString(curStr.substring(0, curStr.getSize() - 1));
						else
							cText->text.setString("0");
					}
					// number
					else if (48 <= event.text.unicode && event.text.unicode <= 57) {
						if (curStr.getSize() <= 1 && curStr == "0")
							cText->text.setString(static_cast<char>(event.text.unicode));
						else
							cText->text.setString(curStr + static_cast<char>(event.text.unicode));
					}
				}
			}
		}
		// Handle click
		if (event.type == sf::Event::MouseButtonPressed)
		{
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				std::cout << "the right button was pressed" << std::endl;
				std::cout << "mouse x: " << event.mouseButton.x << std::endl;
				std::cout << "mouse y: " << event.mouseButton.y << std::endl;
				auto manager = m_curScene->getManager();
				// Lose focus
				std::cout << "Focus lost" << std::endl;
				m_focus.reset();
				// Find clicked entity
				auto& entities = manager->getEntities(ComponentType::CLICKABLE);
				for (auto& entity : entities) {
					auto cClick = entity->getComponent<CClickable>();
					if (cClick->isActive) {
						// Boundary check for Edit Texts
						if (entity->hasComponent<CText>()) {
							auto cText = entity->getComponent<CText>();
							if (cText->canEdit) {
								auto bound = cText->states.transform.transformRect(cText->text.getGlobalBounds());
								if (bound.contains(event.mouseButton.x, event.mouseButton.y)) {
									m_focus = entity;
									cClick->onClickListener();
									std::cout << "New focus" << std::endl;
									std::cout << std::format("{}, {}, {}, {}\n", bound.top, bound.left, bound.width, bound.height);
									break;
								}
							}
						}
						// Boundary check for Shapes
						else if (entity->hasComponent<CShape>()) {
							auto cShape = entity->getComponent<CShape>();
							auto bound = cShape->states.transform.transformRect(cShape->vertexArray.getBounds());
							std::cout << std::format("{}, {}, {}, {}\n", bound.top, bound.left, bound.width, bound.height);
							if (bound.contains(event.mouseButton.x, event.mouseButton.y)) {
								cClick->onClickListener();
								std::cout << "Clicked a shape" << std::endl;
								break;
							}
						}
					}
				}
			}
		}
		/*
		if (event.type == sf::Event::MouseMoved)
		{
			std::cout << "new mouse x: " << event.mouseMove.x << std::endl;
			std::cout << "new mouse y: " << event.mouseMove.y << std::endl;
		}
		*/
		if (event.type == sf::Event::MouseEntered)
			std::cout << "the mouse cursor has entered the window" << std::endl;

		if (event.type == sf::Event::MouseLeft)
			std::cout << "the mouse cursor has left the window" << std::endl;

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