#pragma once

#include "../EntityManager.h"
#include "../GameState.h"
#include "../Vec2.h"
#include <SFML/Graphics.hpp>
#include <memory>

class Entity;

namespace RenderSystem {
void sCamera(sf::RenderWindow& window, sf::View& gameView,
             const std::shared_ptr<Entity>& player1,
             const std::shared_ptr<Entity>& player2);

void sRender(sf::RenderWindow& window,
             sf::View& gameView,
             EntityManager& entityManager,
             sf::Font& font,
             sf::Texture& p1HeartTex,
             sf::Texture& p2HeartTex,
             const std::shared_ptr<Entity>& player1,
             const std::shared_ptr<Entity>& player2,
             GameState m_state,
             int m_selectedOption,
             bool m_hasBackground,
             sf::Sprite& backgroundSprite,
             float transitionRadius,
             const Vec2& transitionCenter);
}
