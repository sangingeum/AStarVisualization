#pragma once
#include "Entity.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
class EntityManager {
	std::vector<std::shared_ptr<Entity>> m_entities;
	std::unordered_map<ComponentType, std::vector<std::shared_ptr<Entity>>> m_entityMap;
	std::vector<std::shared_ptr<Entity>> m_toAdd;
	size_t m_entityID = 0;
	void removeDestroyedEntities(std::vector<std::shared_ptr<Entity>>& entityList);
public:
	EntityManager();
	void update();
	void clear();
	size_t getNextID();
	std::shared_ptr<Entity> addEntity();
	std::vector<std::shared_ptr<Entity>>& getEntities();
	std::vector<std::shared_ptr<Entity>>& getEntities(ComponentType type);
};
