#include "GameSystem.hpp"

static std::string str{};

void GameSystem::render() {
	m_window.clear();
	sf::Text text;
	// select the font
	text.setFont(m_config.font); // font is a sf::Font

	// set the string to display
	text.setString(str);

	// set the character size
	text.setCharacterSize(24); // in pixels, not points!

	// set the color
	text.setFillColor(sf::Color::White);
	
	// set the text style
	// text.setStyle(sf::Text::Bold | sf::Text::Underlined);
	sf::Transform t;
	//t.translate(500, 500);

	// inside the main loop, between window.clear() and window.display()
	m_window.draw(text, t);
	auto bound = text.getGlobalBounds();
	std::cout << bound.left << " " << bound.top << " " << bound.width << " " << bound.top << "\n";
	//m_window.draw(text);
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


		else if (event.type == sf::Event::TextEntered)
		{
			// BackSpace
			if (event.text.unicode == 8) {
				if(!str.empty())
					str.pop_back();
			}
			else if (event.text.unicode < 128)
				//std::cout << "ASCII character typed: " << static_cast<char>(event.text.unicode) << std::endl;
				str.push_back(static_cast<char>(event.text.unicode));
		}

		if (event.type == sf::Event::MouseWheelScrolled)
		{
			if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
				std::cout << "wheel type: vertical" << std::endl;
			else if (event.mouseWheelScroll.wheel == sf::Mouse::HorizontalWheel)
				std::cout << "wheel type: horizontal" << std::endl;
			else
				std::cout << "wheel type: unknown" << std::endl;
			std::cout << "wheel movement: " << event.mouseWheelScroll.delta << std::endl;
			std::cout << "mouse x: " << event.mouseWheelScroll.x << std::endl;
			std::cout << "mouse y: " << event.mouseWheelScroll.y << std::endl;
		}
		if (event.type == sf::Event::MouseButtonPressed)
		{
			if (event.mouseButton.button == sf::Mouse::Right)
			{
				std::cout << "the right button was pressed" << std::endl;
				std::cout << "mouse x: " << event.mouseButton.x << std::endl;
				std::cout << "mouse y: " << event.mouseButton.y << std::endl;
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
	setScene(std::make_unique<Scene1>());
}
void GameSystem::run() {
	while (m_window.isOpen())
	{
		update();
		handleUserInput();
		render();
	}
}