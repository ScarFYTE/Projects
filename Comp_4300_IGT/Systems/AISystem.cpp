#include "AISystem.h"

#include "../AudioManager.h"
#include "../Constants.h"
#include "../Entity.h"
#include "HealthSystem.h"

#include <cmath>

namespace AISystem {
void sPatrol(EntityManager& entityManager) {
    for (auto& e : entityManager.GetEntities("Enemy")) {
        if (!e->patrol || !e->transform) { continue; }

        auto& patrol = e->patrol;
        auto& t = e->transform;

        if (patrol->waypoints.empty()) { continue; }

        Vec2 target = patrol->waypoints[patrol->currentTarget];
        Vec2 delta = target - t->position;
        float dist = std::sqrt(delta.x * delta.x + delta.y * delta.y);

        if (dist < GameConstants::kPatrolTargetThreshold) {
            patrol->currentTarget =
                (patrol->currentTarget + 1) % static_cast<int>(patrol->waypoints.size());
        }
        else {
            Vec2 dir = delta * (1.0f / dist);
            t->position += dir * patrol->speed;
            patrol->facingRight = (dir.x > 0.0f);
        }
    }
}

void sSight(EntityManager& entityManager,
            GameState& m_state,
            void (*startRespawn)(GameState&, Vec2&, float&, bool&, Vec2),
            Vec2& transitionCenter,
            float& transitionRadius,
            bool& m_isLoadingNextLevel,
            const char* gameOverTrack,
            ::AudioManager& audioManager) {
    for (auto& enemy : entityManager.GetEntities("Enemy")) {
        if (!enemy->sight || !enemy->transform) { continue; }

        auto& sight = enemy->sight;
        Vec2 enemyPos = enemy->transform->position;

        Vec2 facing = { 1.0f, 0.0f };
        if (enemy->patrol) {
            facing = enemy->patrol->facingRight
                ? Vec2(1.0f, 0.0f)
                : Vec2(-1.0f, 0.0f);
        }

        float cosHalf = std::cos(sight->halfAngleDeg * GameConstants::kPi / 180.0f);

        for (auto& player : entityManager.GetEntities("Player")) {
            if (!player->transform || !player->health) { continue; }
            if (player->health->isDead) { continue; }

            Vec2 toPlayer = player->transform->position - enemyPos;
            float dist = std::sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);

            if (dist < sight->range && dist > 0.0f) {
                Vec2 dir = toPlayer * (1.0f / dist);
                float dot = facing.x * dir.x + facing.y * dir.y;

                if (dot > cosHalf) {
                    player->health->lives--;
                    if (player->health->lives <= 0) {
                        m_state = GameState::GameOver;
						audioManager.pushMusic(gameOverTrack, GameConstants::kMusicVolume);
                    }
                    else {
                        startRespawn(m_state, transitionCenter, transitionRadius, m_isLoadingNextLevel, player->transform->position);
                    }
                    return;
                }
            }
        }
    }
}
} // namespace AISystem
