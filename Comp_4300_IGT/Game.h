#pragma once

#include "AudioManager.h"
#include "Constants.h"
#include "Entity.h"
#include "EntityManager.h"
#include "GameState.h"
#include "LevelQueue.h"
#include "LevelLoader.h"
#include "Systems/AISystem.h"
#include "Systems/HealthSystem.h"
#include "Systems/InputSystem.h"
#include "Systems/InteractSystem.h"
#include "Systems/PhysicsSystem.h"
#include "Systems/RenderSystem.h"

#include <SFML/Graphics.hpp>
#include <map>
#include <memory>
#include <string>

class Game {
    Vec2 P1_SPAWN = Vec2(GameConstants::kWindowWidth * 0.25f, GameConstants::kWindowHeight - GameConstants::kGroundH - GameConstants::kPlayerH * 0.5f);
    Vec2 P2_SPAWN = Vec2(GameConstants::kWindowWidth * 0.5f, GameConstants::kWindowHeight - GameConstants::kGroundH - GameConstants::kPlayerH * 0.5f);

    AudioManager audioManager;

    sf::RenderWindow window;
    sf::Font font;
    sf::Texture p1HeartTex;
    sf::Texture p2HeartTex;

    std::map<std::string, sf::Texture> textureCache;
    sf::Texture& getTexture(const std::string& name);

    sf::Texture dummyBgTex;
    sf::Sprite backgroundSprite{ dummyBgTex };
    std::string currentBackgroundKey;
    bool m_hasBackground = false;

    LevelQueue levelQueue;
    std::string currentLevelPath;
    bool m_isLoadingNextLevel = false;

    EntityManager entityManager;
    bool m_isRunning = true;
    int m_currentFrame = 0;

    float transitionRadius = GameConstants::kTransitionRadiusStart;
    Vec2 transitionCenter = { 0.0f, 0.0f };

    sf::View gameView;

    std::shared_ptr<Entity> player1;
    std::shared_ptr<Entity> player2;

    GameState m_state = GameState::StartMenu;
    int m_selectedOption = 0;

    void init();
    void spawnGround();
    void spawnPlayers();
    void spawnDustParticles(Vec2 position, int count = 6, float directionX = 0.0f);

    void LoadNextLevel();
    void StartWipe(Vec2 focusPoint, bool advancingLevel = false);

public:
    Game();
    void Run();
};
