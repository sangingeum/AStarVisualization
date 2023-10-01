#pragma once
#include "Scene.hpp"
#include "KDTree.hpp"
#include "AStar.hpp"
#include <cmath>

//Concrete Scenes
class MainScene : public Scene {

private:
	// Graph vertex
	struct Vertex {
		std::pair<float, float> pos;
		float gScore, fScore;
		size_t parent;
		std::shared_ptr<Entity> block;
	};

	size_t n{ 10 }, m{ 10 };
	std::shared_ptr<Entity> nField;
	std::shared_ptr<Entity> mField;

	std::shared_ptr<Entity> resetButton;
	std::shared_ptr<Entity> startButton;
	float startN{};
	float startM{};
	float endN{};
	float endM{};
	std::vector<std::shared_ptr<Entity>> blocks;
	KDTree<2, std::shared_ptr<Entity>> tree;
	AdjacencyListGraph<Vertex> graph{0};

	bool m_mousePressingSecondTime{ false };
	float m_lastMouseX{ 0 }, m_lastMouseY{ 0 };
	int m_mouseSplits{ 5 };

	sf::FloatRect gridRect{570, 10, 700, 700};
	sf::Color pathColor = sf::Color(154, 123, 79, 255);
	sf::Color obstacleColor = sf::Color(72, 66, 39, 255);
	sf::Color startColor = sf::Color::Blue;
	sf::Color endColor = sf::Color::Red;
	sf::Color grayColor = sf::Color(155, 155, 155, 255);
	bool isMousePressing{ false };
	bool leftPressing{ false };
	bool AStarStarted{ false };
	bool needRender{ false };
	float guidanceScaler{ 1.0f };
public:
	void init() override;
	void handleMouseInput(sf::Event& event) override;
	void handleKeyBoardInput(sf::Event& event) override;
	void update(sf::RenderWindow& window) override;
	bool getNeedRender() override;
private:
	std::shared_ptr<Entity> createEditText(const std::string initialText, unsigned fontSize, float left, float top);
	std::shared_ptr<Entity> createLabel(const std::string initialText, unsigned fontSize, float left, float top, sf::Color color = sf::Color::White);
	std::shared_ptr<Entity> createButton(float left, float top, float width, float height, std::function<void()> listener);
	std::shared_ptr<Entity> createBlock(float left, float top, float width, float height, sf::Color color = sf::Color::White);
	void updateNM();
	void setColor(std::shared_ptr<CShape> component, sf::Color color);
	void setText(std::shared_ptr<CText> component, std::string newText);
	void setTextStyle(std::shared_ptr<CText> component, sf::Text::Style style);
	void runAStar();
	void resetBlocks();
};



