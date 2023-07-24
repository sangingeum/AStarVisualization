#pragma once
#include "EntityManager.hpp"
#include "Config.hpp"
#include <cmath>
#include <iostream>
#include <format>
class Scene
{
protected:
	std::shared_ptr<EntityManager> m_entityManager;
	sf::Color m_blockColor = sf::Color::White;
	GameConfig& m_config;
public:
	virtual ~Scene() = default;
	Scene() : m_entityManager(std::make_shared<EntityManager>()), m_config(GameConfig::instance()) {}
	virtual void init() = 0;
	std::shared_ptr<EntityManager> getManager();
};

//Concrete Scenes
class MainScene : public Scene {

public:
	size_t n{ 5 }, m{ 5 };
	std::shared_ptr<Entity> nField;
	std::shared_ptr<Entity> mField;
	std::vector<std::shared_ptr<Entity>> blocks;

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
		entity->addComponent<CClickable>([cShape]() {
			cShape->states.transform.translate(10, 10);
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
		blocks.reserve(n * m);
		for (size_t i = 0; i < n; ++i) {
			for (size_t j = 0; j < m; ++j) {
				blocks.push_back(createBlock(570 + j * size, 10 + i * size, size-1, size-1, sf::Color(154, 123, 79, 255)));
			}
		}
	}

	void init() override {
		int fieldLeft = 100, fieldTop = 100;
		// nField
		nField = createEditText("5", 36, fieldLeft + 50, fieldTop);
		// mField
		mField = createEditText("5", 36, fieldLeft + 50, fieldTop + 50);
		// nLabel
		createLabel("N:", 36, fieldLeft, fieldTop);
		// mLabel
		createLabel("M:", 36, fieldLeft, fieldTop + 50);
		// Create Blocks
		resetBlocks();

		// Create reset button
		createButton(fieldLeft + 50, fieldTop + 150, 100, 50, [this]() {
			updateNM();
			resetBlocks();
			});

		// Create reset button label
		createLabel("Reset", 36, fieldLeft, fieldTop + 125, sf::Color::Blue);
	}
};

