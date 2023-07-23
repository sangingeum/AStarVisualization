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
	if (hasComponent<CShape>()) {
		types.push_back(ComponentType::SHAPE);
	}
	if (hasComponent<CText>()) {
		types.push_back(ComponentType::TEXT);
	}
	if (hasComponent<CClickable>()) {
		types.push_back(ComponentType::CLICKABLE);
	}

	return types;
}
