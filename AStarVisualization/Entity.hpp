#pragma once
#include "Component.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <tuple>

typedef std::tuple<
	std::shared_ptr<CRender>
	,std::shared_ptr<CRayCast>
	, std::shared_ptr<CFlollowCursor>
> ComponentTuple;


class Entity {
	friend class EntityManager;
	size_t m_id{ 0 };
	bool m_toDestroy = false;
	ComponentTuple m_components;
	Entity(size_t id);
public:
	void destroy();
	bool isDestroyed();
	size_t getId();
	template <class T, class... TArgs>  std::shared_ptr<T> addComponent(TArgs&&... mArgs);
	template <class T> std::shared_ptr<T> getComponent();
	template <class T> void removeComponent();
	template <class T> bool hasComponent();
	std::vector<ComponentType> getComponentTypes();
};

template <class T, class... TArgs> 
std::shared_ptr<T> Entity::addComponent(TArgs&&... mArgs) {
	auto newComp = std::shared_ptr<T>(new T(std::forward<TArgs>(mArgs)...));
	std::get<std::shared_ptr<T>>(m_components) = newComp;
	return newComp;
}

template <class T> void Entity::removeComponent() {
	std::get<std::shared_ptr<T>>(m_components).reset();
}

template <class T> std::shared_ptr<T> Entity::getComponent() {
	return std::get<std::shared_ptr<T>>(m_components);
}

template <class T> bool Entity::hasComponent() {
	if (std::get<std::shared_ptr<T>>(m_components))
		return true;
	return false;
}