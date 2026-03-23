#pragma once
#include "Entity.h"
#include <vector>
#include <map>
class EntityManager {
private:
	std::vector<std::shared_ptr<Entity>> entities;
	std::map<std::string,std::shared_ptr<Entity>> tagMap;
	std::vector<std::shared_ptr<Entity>> entitiesToAdd;
	size_t nextID = 0;

public:

};