#pragma once
#include "Scene.hpp"
#include "KDTree.hpp"
#include <cmath>
#include <iostream>
#include <format>

class DrawScene : public Scene {

public:
	size_t n{ 10 }, m{ 10 };
	std::shared_ptr<Entity> nField;
	std::shared_ptr<Entity> mField;
	std::shared_ptr<Entity> resetButton;
	std::vector<std::shared_ptr<Entity>> blocks;
	KDTree<2, std::shared_ptr<Entity>> m_tree;
	sf::FloatRect gridRect{570, 10, 700, 700};
	sf::Color pathColor = sf::Color(154, 123, 79, 255);
	sf::Color obstacleColor = sf::Color(72, 66, 39, 255);
	sf::Color startColor = sf::Color(155, 155, 255, 255);
	sf::Color endColor = sf::Color(155, 155, 155, 255);
	bool isMousePressing{ false };

	std::shared_ptr<Entity> createEditText(const std::string initialText, unsigned fontSize, float left, float top) {
		auto entity = m_entityManager->addEntity();
		auto cEdit = entity->addComponent<CText>(initialText, m_config.font, fontSize);
		cEdit->text.setFillColor(sf::Color::White);
		cEdit->states.transform.translate(left, top);
		entity->addComponent<CClickable>([]() {return; });
		return entity;
	}
	std::shared_ptr<Entity> createLabel(const std::string initialText, unsigned fontSize, float left, float top, sf::Color color = sf::Color::White) {
		auto entity = m_entityManager->addEntity();
		auto cEdit = entity->addComponent<CText>(initialText, m_config.font, fontSize);
		cEdit->text.setFillColor(color);
		cEdit->states.transform.translate(left, top);
		cEdit->canEdit = false;
		entity->addComponent<CClickable>([]() {return; });
		return entity;
	}
	std::shared_ptr<Entity> createButton(float left, float top, float width, float height, std::function<void()> listener) {
		auto entity = m_entityManager->addEntity();
		auto vertexArr = sf::VertexArray(sf::Quads, 4);
		float halfWidth = width / 2.f, halfHeight = height / 2.f;
		vertexArr[0].position = { -halfWidth, -halfHeight };
		vertexArr[1].position = { halfWidth, -halfHeight };
		vertexArr[2].position = { halfWidth, halfHeight };
		vertexArr[3].position = { -halfWidth, halfHeight };
		auto cShape = entity->addComponent<CShape>(vertexArr);
		cShape->states.transform.translate(left, top);
		entity->addComponent<CClickable>(listener);
		return entity;
	}
	std::shared_ptr<Entity> createBlock(float left, float top, float width, float height, sf::Color color = sf::Color::White) {
		auto entity = m_entityManager->addEntity();
		auto vertexArr = sf::VertexArray(sf::Quads, 4);
		float halfWidth = width / 2.f, halfHeight = height / 2.f;
		vertexArr[0].position = { 0, 0 };
		vertexArr[1].position = { width, 0 };
		vertexArr[2].position = { width, height };
		vertexArr[3].position = { 0, height };
		vertexArr[0].color = color;
		vertexArr[1].color = color;
		vertexArr[2].color = color;
		vertexArr[3].color = color;
		auto cShape = entity->addComponent<CShape>(vertexArr);
		cShape->states.transform.translate(left, top);
		auto cBlock = entity->addComponent<CBlock>();

		entity->addComponent<CClickable>([cShape, cBlock, this]() {
			cBlock->isObstacle = true;
			size_t size = cShape->vertexArray.getVertexCount();
			for (size_t i = 0; i < size; ++i) {
				cShape->vertexArray[i].color = obstacleColor;
			}
			});
		return entity;
	}
	void updateNM() {
		auto nText = nField->getComponent<CText>();
		auto mText = mField->getComponent<CText>();
		this->n = std::stoi(std::string(nText->text.getString()));
		this->m = std::stoi(std::string(mText->text.getString()));
		std::cout << std::format("n:{}, m:{}\n", this->n, this->m);
	}
	void resetBlocks() {
		// Clear blocks
		for (auto& block : blocks) {
			block->destroy();
		}
		blocks.clear();
		// Create blocks (570, 10) -> (1270, 710) // 700 X 700
		float size = std::min(700.f / n, 700.f / m);
		float halfSize = (size - 1) / 2.f;
		blocks.reserve(n * m);
		std::vector<std::pair<std::array<float, 2>, std::shared_ptr<Entity>>> pointBlockPairs;
		for (size_t i = 0; i < n; ++i) {
			for (size_t j = 0; j < m; ++j) {
				float x = 570 + j * size, y = 10 + i * size;
				auto blockEntity = createBlock(x, y, size - 1, size - 1, pathColor);
				blocks.push_back(blockEntity);
				pointBlockPairs.push_back({ { x + halfSize , y + halfSize }, blockEntity });
			}
		}
		// Set grid Range
		gridRect.height = n * size;
		gridRect.width = m * size;

		// Build a K-D tree to efficiently search blocks
		m_tree.buildTree(pointBlockPairs);
	}

	void init() override {
		int fieldLeft = 100, fieldTop = 100;
		// nField
		nField = createEditText(std::to_string(n), 36, fieldLeft + 50, fieldTop);
		// mField
		mField = createEditText(std::to_string(m), 36, fieldLeft + 50, fieldTop + 50);
		// nLabel
		createLabel("N:", 36, fieldLeft, fieldTop);
		// mLabel
		createLabel("M:", 36, fieldLeft, fieldTop + 50);
		// Create Blocks
		resetBlocks();
		// Create reset button
		resetButton = createButton(fieldLeft + 50, fieldTop + 150, 100, 50, [this]() {
			updateNM();
			resetBlocks();
			});
		// Create reset button label
		createLabel("Reset", 36, fieldLeft, fieldTop + 125, sf::Color::Blue);
	}

	void handleMouseInput(sf::Event& event) override {

		if (event.type == sf::Event::MouseButtonPressed) {
			isMousePressing = true;
			if (event.mouseButton.button == sf::Mouse::Left) {
				std::cout << "the left button was pressed" << std::endl;
				std::cout << "mouse x: " << event.mouseButton.x << std::endl;
				std::cout << "mouse y: " << event.mouseButton.y << std::endl;
				float mouseX = event.mouseButton.x;
				float mouseY = event.mouseButton.y;
				// Check text fields
				for (auto& entity : { nField, mField }) {
					auto cText = entity->getComponent<CText>();
					// Reset focus
					std::cout << "Lose focus\n";
					cText->focused = false;
					auto& text = cText->text;
					text.setStyle(sf::Text::Style::Regular);
					// Set focus
					auto bound = cText->states.transform.transformRect(text.getGlobalBounds());
					if (bound.contains(mouseX, mouseY)) {
						std::cout << "New focus\n";
						cText->focused = true;
						text.setStyle(sf::Text::Style::Underlined);
					}
				}
				// Check buttons
				for (auto& entity : { resetButton }) {
					auto cShape = entity->getComponent<CShape>();
					auto bound = cShape->states.transform.transformRect(cShape->vertexArray.getBounds());
					if (bound.contains(mouseX, mouseY)) {
						auto cClick = entity->getComponent<CClickable>();
						cClick->onClickListener();
					}
				}
			}
		}
		else if (event.type == sf::Event::MouseButtonReleased)
			isMousePressing = false;


	}

	void handleKeyBoardInput(sf::Event& event) override {
		if (event.type == sf::Event::TextEntered) {
			// Edit focused text
			for (auto& entity : { nField, mField }) {
				auto cText = entity->getComponent<CText>();
				auto& text = cText->text;
				auto& str = text.getString();
				auto code = event.text.unicode;
				if (cText->focused) {
					std::cout << "Editting Focused text\n";
					// backspace
					if (code == 8) {
						size_t size = str.getSize();
						if (size <= 1)
							text.setString("0");
						else
							text.setString(str.substring(0, size - 1));
					}
					// number
					else if (48 <= code && code <= 57) {
						size_t size = str.getSize();
						if (str == "0")
							text.setString(static_cast<char>(code));
						else
							text.setString(str + static_cast<char>(code));
					}
				}
			}
		}
	}

	void update(sf::RenderWindow& window) {
		if (isMousePressing) {
			auto mousePos = sf::Mouse::getPosition(window);
			float mouseX = mousePos.x;
			float mouseY = mousePos.y;
			std::cout << mouseX << " " << mouseY << "\n";
			// Handle clicked button
			if (gridRect.contains(mouseX, mouseY)) {
				auto nearestButton = m_tree.findNearestNeighbor({ mouseX, mouseY }).second;
				auto cClick = nearestButton->getComponent<CClickable>();
				cClick->onClickListener();
			}
		}
	}
};


