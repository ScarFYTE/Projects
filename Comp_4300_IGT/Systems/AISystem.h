#pragma once


#include "../EntityManager.h"
#include "../GameState.h"
#include "../Vec2.h"

class AudioManager;

namespace AISystem {
void sPatrol(EntityManager& entityManager);
void sSight(EntityManager& entityManager,
            GameState& m_state,
            void (*startRespawn)(GameState&, Vec2&, float&, bool&, Vec2),
            Vec2& transitionCenter,
            float& transitionRadius,
            bool& m_isLoadingNextLevel,
            const char* gameOverTrack,
            ::AudioManager& audioManager);
}
