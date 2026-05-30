#include "EntityManager.h"
#include "Entity.h"
#include "Components.h"
#include <algorithm>

void EntityManager::Update() {
    for (auto& e : entitiesToAdd) {
        entities.push_back(e);
        tagMap[e->GetTag()].push_back(e);
    }
    entitiesToAdd.clear();
    RemoveDeadEntities();
}

std::shared_ptr<Entity>& EntityManager::AddEntity(const std::string& tag) {
    std::shared_ptr<Entity> e(new Entity(nextID++, tag));
    entitiesToAdd.push_back(e);
    return entitiesToAdd.back();
}

void EntityManager::RemoveDeadEntities() {
    for (auto& [tag, vec] : tagMap) {
        vec.erase(std::remove_if(vec.begin(), vec.end(),
            [](const std::shared_ptr<Entity>& e) { return !e || !e->IsActive(); }),
            vec.end());
    }

    entities.erase(std::remove_if(entities.begin(), entities.end(),
        [](const std::shared_ptr<Entity>& e) { return !e || !e->IsActive(); }),
        entities.end());
}

const std::vector<std::shared_ptr<Entity>>& EntityManager::GetEntities() const {
    return entities;
}

const std::vector<std::shared_ptr<Entity>>& EntityManager::GetEntities(const std::string& tag) {
    return tagMap[tag];
}
