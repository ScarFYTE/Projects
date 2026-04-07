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
		if (entity->lifespan) {
			entity->lifespan->remaining--;
			if (entity->lifespan->remaining <= 0) {
				entity->active = false;
			}
		}
	}
	RemoveDeadEntities();
}


std::shared_ptr<Entity>& EntityManager::AddEntity(const std::string& tag) {
	std::shared_ptr<Entity> e(new Entity(nextID++, tag));

	entitiesToAdd.push_back(e);
	tagMap[tag] = e;

	return e;
}

void EntityManager::RemoveDeadEntities() {
	for (auto& e : entities) {
		if (e && !e->IsActive()) {
			tagMap.erase(e->GetTag());
		}
	}

	for (auto it = entities.begin(); it != entities.end(); /* no increment here */) {
		if (!*it || !(*it)->IsActive()) {
			it = entities.erase(it); // erase returns the next valid iterator
		}
		else {
			++it;
		}
	}
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