#include "Entity.h"

void Entity::Destroy() {
	active = false;
}

bool Entity::IsActive() const {
	return active;
}

std::string& Entity::GetTag() const {
	return const_cast<std::string&>(tag);
}

size_t Entity::GetID() const {
	return id;
}