#pragma once
#include "EntityManager.hpp"
#include "Config.hpp"
#include <cmath>
class Scene
{
protected:
	std::shared_ptr<EntityManager> m_entityManager;
	sf::Color m_blockColor = sf::Color::White;
public:
	virtual ~Scene() = default;
	Scene();
	virtual void init() = 0;
	std::shared_ptr<EntityManager> getManager();
	void baseInit();
};

//Concrete Scenes
class Scene1 : public Scene {

public:
	void init() override {
		baseInit();
	}
};

