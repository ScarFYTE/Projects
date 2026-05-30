#pragma once

#include "../EntityManager.h"
#include "../GameState.h"
#include "../Vec2.h"
#include <memory>

class AudioManager;

namespace HealthSystem {
void StartRespawn(GameState& m_state, Vec2& transitionCenter, float& transitionRadius, bool& m_isLoadingNextLevel, Vec2 focusPoint);
void ApplyReset(const std::shared_ptr<class Entity>& player1,
                const std::shared_ptr<class Entity>& player2,
                const Vec2& p1Spawn,
                const Vec2& p2Spawn,
                Vec2& transitionCenter,
                GameState& m_state);
void sHealth(EntityManager& entityManager,
             GameState& m_state,
             Vec2& transitionCenter,
             float& transitionRadius,
             bool& m_isLoadingNextLevel,
             AudioManager& audioManager,
             const char* gameOverTrack);
}
