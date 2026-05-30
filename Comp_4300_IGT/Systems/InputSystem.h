#pragma once

#include "../AudioManager.h"
#include "../EntityManager.h"
#include "../GameState.h"
#include "../LevelQueue.h"
#include "../Vec2.h"
#include <SFML/Graphics.hpp>
#include <map>
#include <memory>
#include <string>

class Entity;

namespace InputSystem {
void sUserInput(sf::RenderWindow& window,
                bool& m_isRunning,
                GameState& m_state,
                int& m_selectedOption,
                EntityManager& entityManager,
                std::shared_ptr<Entity>& player1,
                std::shared_ptr<Entity>& player2,
                LevelQueue& levelQueue,
                std::string& currentLevelPath,
                Vec2& p1Spawn,
                Vec2& p2Spawn,
                bool& m_hasBackground,
                std::string& currentBackgroundKey,
                std::map<std::string, sf::Texture>& textureCache,
                sf::View& gameView,
                AudioManager& audioManager,
                void (*spawnPlayers)(EntityManager&, std::shared_ptr<Entity>&, std::shared_ptr<Entity>&, const Vec2&, const Vec2&));
}
