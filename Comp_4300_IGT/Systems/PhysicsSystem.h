#pragma once

#include "../EntityManager.h"
#include "../Vec2.h"
#include <memory>

class Entity;
class AudioManager;

namespace PhysicsSystem {
void sGravity(EntityManager& entityManager);
void sMovement(EntityManager& entityManager, AudioManager& audioManager,
               const std::shared_ptr<Entity>& player1,
               const std::shared_ptr<Entity>& player2,
               void (*spawnDustParticles)(EntityManager&, Vec2, int, float));
void sCollision(EntityManager& entityManager,
                void (*spawnDustParticles)(EntityManager&, Vec2, int, float));
void sMovingPlatform(EntityManager& entityManager);
}
