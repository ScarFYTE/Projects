#include "Entity.h"

Entity::Entity(const size_t id, const std::string& tag) : id(id), tag(tag) {}

Entity::~Entity() {}

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