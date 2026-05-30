#pragma once

#include "../EntityManager.h"
#include "../GameState.h"
#include "../Vec2.h"
#include <memory>

class Entity;
class AudioManager;

namespace InteractSystem {
void sInteract(EntityManager& entityManager,
               const std::shared_ptr<Entity>& player1,
               const std::shared_ptr<Entity>& player2,
               AudioManager& audioManager);

void sWinCondition(EntityManager& entityManager,
                   GameState& m_state,
                   Vec2& transitionCenter,
                   float& transitionRadius,
                   bool& m_isLoadingNextLevel);
}
