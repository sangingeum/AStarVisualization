#include "Entity.hpp"
Entity::Entity(size_t id)
	: m_id(id)
{}
void Entity::destroy() {
	m_toDestroy = true;
}
bool Entity::isDestroyed() {
	return m_toDestroy;
}
size_t Entity::getId() {
	return m_id;
}

std::vector<ComponentType> Entity::getComponentTypes() {
	std::vector<ComponentType> types;
	if (hasComponent<CRender>()) {
		types.push_back(ComponentType::RENDER);
	}
	if (hasComponent<CRayCast>()) {
		types.push_back(ComponentType::RAYCAST);
	}
	if (hasComponent<CFlollowCursor>()) {
		types.push_back(ComponentType::FOLLOWCURSOR);
	}

	return types;
}
