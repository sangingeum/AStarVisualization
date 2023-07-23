#include "Scene.hpp"

Scene::Scene() : m_entityManager(std::make_shared<EntityManager>()) {}
std::shared_ptr<EntityManager> Scene::getManager() {
	return m_entityManager;
}

void Scene::baseInit() {
}
