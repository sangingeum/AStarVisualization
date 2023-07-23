#include "EntityManager.hpp"

EntityManager::EntityManager() {};
void EntityManager::update() {
	// Add
	for (auto entity : m_toAdd) {
		m_entities.push_back(entity);
		for (auto type : entity->getComponentTypes()) {
			m_entityMap[type].push_back(entity);
		}
	}
	m_toAdd.clear();
	//Remove
	for (auto& [type, entityList] : m_entityMap) {
		removeDestroyedEntities(entityList);
	}
	removeDestroyedEntities(m_entities);
}

void EntityManager::clear() {
	m_entities.clear();
	for (auto& [type, entityList] : m_entityMap) {
		entityList.clear();
	}
	m_toAdd.clear();
}

void EntityManager::removeDestroyedEntities(std::vector<std::shared_ptr<Entity>>& entityList) {
	entityList.erase(std::remove_if(entityList.begin(), entityList.end(),
		[](const std::shared_ptr<Entity>& entity) {return entity->isDestroyed(); }),
		entityList.end());
}

size_t EntityManager::getNextID() {
	return m_entityID++;
}

// delayed add - to avoid iterator invalidation
std::shared_ptr<Entity> EntityManager::addEntity() {
	auto entity = std::shared_ptr<Entity>(new Entity(getNextID()));
	m_toAdd.push_back(entity);
	return entity;
}

std::vector<std::shared_ptr<Entity>>& EntityManager::getEntities() {
	return m_entities;
}
std::vector<std::shared_ptr<Entity>>& EntityManager::getEntities(ComponentType type) {
	return m_entityMap[type];
}