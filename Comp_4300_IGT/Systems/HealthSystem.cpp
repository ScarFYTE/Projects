#include "HealthSystem.h"

#include "../AudioManager.h"
#include "../Constants.h"
#include "../Entity.h"

namespace HealthSystem {
void StartRespawn(GameState& m_state, Vec2& transitionCenter, float& transitionRadius, bool& m_isLoadingNextLevel, Vec2 focusPoint) {
    if (m_state == GameState::RespawnFadeOut || m_state == GameState::RespawnFadeIn) { return; }

    m_state = GameState::RespawnFadeOut;
    transitionCenter = focusPoint;
    transitionRadius = GameConstants::kTransitionRadiusStart;

    m_isLoadingNextLevel = false;
}

void ApplyReset(const std::shared_ptr<Entity>& player1,
                const std::shared_ptr<Entity>& player2,
                const Vec2& p1Spawn,
                const Vec2& p2Spawn,
                Vec2& transitionCenter,
                GameState& m_state) {
    player1->transform->position = p1Spawn;
    player1->transform->velocity = { 0.0f, 0.0f };
    player2->transform->position = p2Spawn;
    player2->transform->velocity = { 0.0f, 0.0f };

    transitionCenter = Vec2((p1Spawn.x + p2Spawn.x) * 0.5f, (p1Spawn.y + p2Spawn.y) * 0.5f);
    m_state = GameState::RespawnFadeIn;
}

void sHealth(EntityManager& entityManager,
             GameState& m_state,
             Vec2& transitionCenter,
             float& transitionRadius,
             bool& m_isLoadingNextLevel,
             AudioManager& audioManager,
             const char* gameOverTrack) {
    for (auto& e : entityManager.GetEntities("Player")) {
        if (!e->health || !e->transform) { continue; }

        if (e->transform->position.y > GameConstants::kWindowHeight + GameConstants::kFallDeathOffset) {
            e->health->lives--;

            if (e->health->lives <= 0) {
                m_state = GameState::GameOver;
                audioManager.pushMusic(gameOverTrack, GameConstants::kMusicVolume);
            }
            else {
                StartRespawn(m_state, transitionCenter, transitionRadius, m_isLoadingNextLevel, e->transform->position);
            }
            return;
        }
    }
}
} // namespace HealthSystem
