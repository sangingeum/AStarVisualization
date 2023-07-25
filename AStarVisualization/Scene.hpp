#pragma once
#include "EntityManager.hpp"
#include "Config.hpp"

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
	virtual void handleMouseInput(sf::Event& event) = 0;
	std::shared_ptr<EntityManager> getManager();
};
