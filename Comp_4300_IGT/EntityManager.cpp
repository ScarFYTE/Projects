#include "EntityManager.h"
#include "Entity.h"
#include "Components.h"



void EntityManager::Update() {
	// Add new entities to the main list
	for (auto& entity : entitiesToAdd) {
		entities.push_back(entity);
	}
	entitiesToAdd.clear();
	// Update all active entities
	for (auto& entity : entities) {
		if (entity->IsActive()) {
			// Update logic for the entity can be added here
		}
	}
	RemoveDeadEntities();
}


std::shared_ptr<Entity>& EntityManager::AddEntity(const std::string& tag) {
	auto entity = std::make_shared<Entity>(nextID++, tag);

	entitiesToAdd.push_back(entity);
	tagMap[tag] = entity;

	return entity;
}

void EntityManager::RemoveDeadEntities() {
	// Remove entities that are not active
	for(auto & e : entities) {
		if(!e->IsActive()) {
			entities.erase(std::remove(entities.begin(), entities.end(), e), entities.end());
			tagMap.erase(e->GetTag());
		}
	}
}

const std::vector<std::shared_ptr<Entity>>& EntityManager::GetEntities() const {
	return entities;
}

const std::vector<std::shared_ptr<Entity>>& EntityManager::GetEntities(const std::string& tag) {

	std::vector<std::shared_ptr<Entity>> taggedEntities; // This should be defined outside the loop to accumulate entities with the specified tag
	for (auto& entity : entities) {
		if(entity->GetTag() == tag) {
			taggedEntities.push_back(entity);
		}
	}
	return taggedEntities; // Return the vector of entities with the specified tag
}