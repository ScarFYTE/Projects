#include "EntityManager.h"
#include "Entity.h"
#include "Components.h"



void EntityManager::Update() {
	// Move newly queued entities into the main list
	for (auto& entity : entitiesToAdd) {
		entities.push_back(entity);
	}
	entitiesToAdd.clear();
	RemoveDeadEntities();
}


std::shared_ptr<Entity>& EntityManager::AddEntity(const std::string& tag) {
	std::shared_ptr<Entity> e(new Entity(nextID++, tag));

	entitiesToAdd.push_back(e);
	tagMap[tag].push_back(e);

	return e;
}

void EntityManager::RemoveDeadEntities() {
	for (auto& [tag,vec] : tagMap) {
		vec.erase(std::remove_if(vec.begin(), vec.end(),
			[](const std::shared_ptr<Entity>& e) { return !e->IsActive()||!e; }),
			vec.end());
	}

	entities.erase(std::remove_if(entities.begin(), entities.end(),
		[](const std::shared_ptr<Entity>& e) { return !e->IsActive() || !e; }),
		entities.end());
}

const std::vector<std::shared_ptr<Entity>>& EntityManager::GetEntities() const {
	return entities;
}

const std::vector<std::shared_ptr<Entity>> EntityManager::GetEntities(const std::string& tag) {

	std::vector<std::shared_ptr<Entity>> taggedEntities; // This should be defined outside the loop to accumulate entities with the specified tag
	for (auto& entity : entities) {
		if (entity->GetTag() == tag) {
			taggedEntities.push_back(entity);
		}
	}
	return taggedEntities; // Return the vector of entities with the specified tag
}