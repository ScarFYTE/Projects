#include "Game.h"
#include "Constants.h"

#include <cstdlib>
#include <iostream>

namespace {
void SpawnPlayersImpl(EntityManager& entityManager, std::shared_ptr<Entity>& player1, std::shared_ptr<Entity>& player2, const Vec2& p1Spawn, const Vec2& p2Spawn) {
    player1 = entityManager.AddEntity("Player");
    player1->transform = std::make_shared<CTransform>(p1Spawn, Vec2(0.0f, 0.0f), 0.0f);
    player1->boundingBox = std::make_shared<CBoundingBox>(GameConstants::kPlayerW, GameConstants::kPlayerH);
    player1->sprite = std::make_shared<CSprite>(GameConstants::kPlayerW, GameConstants::kPlayerH, GameConstants::kPlayer1Color);
    player1->input = std::make_shared<CInput>();
    player1->health = std::make_shared<CHealth>();

    player2 = entityManager.AddEntity("Player");
    player2->transform = std::make_shared<CTransform>(p2Spawn, Vec2(0.0f, 0.0f), 0.0f);
    player2->boundingBox = std::make_shared<CBoundingBox>(GameConstants::kPlayerW, GameConstants::kPlayerH);
    player2->sprite = std::make_shared<CSprite>(GameConstants::kPlayerW, GameConstants::kPlayerH, GameConstants::kPlayer2Color);
    player2->input = std::make_shared<CInput>();
    player2->health = std::make_shared<CHealth>();
}

void SpawnDustParticlesImpl(EntityManager& entityManager, Vec2 position, int count, float directionX) {
    for (int i = 0; i < count; i++) {
        auto p = entityManager.AddEntity("Particle");
        float biasX = (directionX != 0.0f) ? directionX * GameConstants::kDustBiasMultiplier : 0.0f;
        float vx = biasX + ((rand() % GameConstants::kDustRandRangeX) - GameConstants::kDustRandOffsetX) / GameConstants::kDustRandDivisor;
        float vy = -((rand() % GameConstants::kDustRandRangeY) + GameConstants::kDustRandOffsetY) / GameConstants::kDustRandDivisor;
        p->transform = std::make_shared<CTransform>(position, Vec2(vx, vy), 0.0f);
        p->sprite = std::make_shared<CSprite>(GameConstants::kDustSize, GameConstants::kDustSize, GameConstants::kDustColor);
        p->particle = std::make_shared<CParticle>(GameConstants::kDustLifetime, GameConstants::kDustColor);
    }
}
}

Game::Game() { init(); }

void Game::init() {
    levelQueue.enqueue(GameConstants::kLevel1Path);
    levelQueue.enqueue(GameConstants::kLevel2Path);
    levelQueue.enqueue(GameConstants::kLevel3Path);

    if (!levelQueue.isEmpty()) {
        currentLevelPath = levelQueue.front();
        levelQueue.dequeue();
        auto result = LevelLoader::load(currentLevelPath, entityManager, textureCache);
        P1_SPAWN = result.p1Spawn; P2_SPAWN = result.p2Spawn; m_hasBackground = result.hasBackground; currentBackgroundKey = result.backgroundKey;
        if (!result.musicTrack.empty()) { audioManager.pushMusic(result.musicTrack, GameConstants::kMusicVolume); }
    }

    font.openFromFile(GameConstants::kFontPath);
    p1HeartTex.loadFromFile(GameConstants::kPlayer1HeartsPath);
    p2HeartTex.loadFromFile(GameConstants::kPlayer2HeartsPath);
    audioManager.init(GameConstants::kJumpSoundPath, GameConstants::kButtonSoundPath, GameConstants::kJumpVolume, GameConstants::kButtonVolume);

    window.create(sf::VideoMode({ GameConstants::kWindowWidth, GameConstants::kWindowHeight }), GameConstants::kWindowTitle);
    window.setFramerateLimit(GameConstants::kFrameLimit);
    spawnPlayers();
    entityManager.Update();
    gameView = window.getDefaultView();

    audioManager.pushMusic(GameConstants::kMenuMusicPath, GameConstants::kMusicVolume);
}

void Game::Run() {
    while (m_isRunning) {
        entityManager.Update();
        InputSystem::sUserInput(window, m_isRunning, m_state, m_selectedOption, entityManager, player1, player2, levelQueue, currentLevelPath, P1_SPAWN, P2_SPAWN, m_hasBackground, currentBackgroundKey, textureCache, gameView, audioManager, SpawnPlayersImpl);

        if (m_hasBackground && !currentBackgroundKey.empty()) {
            sf::Texture& bgTex = getTexture(currentBackgroundKey); backgroundSprite.setTexture(bgTex, true);
            if (bgTex.getSize().x > 0 && bgTex.getSize().y > 0) {
                float scaleX = static_cast<float>(GameConstants::kWindowWidth) / bgTex.getSize().x;
                float scaleY = static_cast<float>(GameConstants::kWindowHeight) / bgTex.getSize().y;
                backgroundSprite.setScale({ scaleX, scaleY });
            }
        }

        if (m_state == GameState::RespawnFadeOut) {
            transitionRadius -= GameConstants::kTransitionFadeSpeed;
            if (transitionRadius <= 0.0f) {
                transitionRadius = 0.0f;
                if (m_isLoadingNextLevel) LoadNextLevel();
                else HealthSystem::ApplyReset(player1, player2, P1_SPAWN, P2_SPAWN, transitionCenter, m_state);
            }
        }
        else if (m_state == GameState::RespawnFadeIn) {
            transitionRadius += GameConstants::kTransitionFadeSpeed;
            if (transitionRadius >= GameConstants::kTransitionRadiusStart) { m_state = GameState::Playing; }
        }

        if (m_state == GameState::Playing) {
            PhysicsSystem::sGravity(entityManager);
            PhysicsSystem::sMovement(entityManager, audioManager, player1, player2, SpawnDustParticlesImpl);
            AISystem::sPatrol(entityManager);
            AISystem::sSight(entityManager, m_state, HealthSystem::StartRespawn, transitionCenter, transitionRadius, m_isLoadingNextLevel, GameConstants::kGameOverMusicPath, audioManager);
            HealthSystem::sHealth(entityManager, m_state, transitionCenter, transitionRadius, m_isLoadingNextLevel, audioManager, GameConstants::kGameOverMusicPath);
            InteractSystem::sInteract(entityManager, player1, player2, audioManager);
            PhysicsSystem::sMovingPlatform(entityManager);
            InteractSystem::sWinCondition(entityManager, m_state, transitionCenter, transitionRadius, m_isLoadingNextLevel);
            PhysicsSystem::sCollision(entityManager, SpawnDustParticlesImpl);
            RenderSystem::sCamera(window, gameView, player1, player2);
        }

        RenderSystem::sRender(window, gameView, entityManager, font, p1HeartTex, p2HeartTex, player1, player2, m_state, m_selectedOption, m_hasBackground, backgroundSprite, transitionRadius, transitionCenter);
        m_currentFrame++;
    }
}

void Game::LoadNextLevel() {
    if (levelQueue.isEmpty()) { m_state = GameState::GameWon; audioManager.pushMusic(GameConstants::kMenuMusicPath, GameConstants::kMusicVolume); return; }
    currentLevelPath = levelQueue.front(); levelQueue.dequeue(); entityManager = EntityManager();
    auto result = LevelLoader::load(currentLevelPath, entityManager, textureCache);
    P1_SPAWN = result.p1Spawn; P2_SPAWN = result.p2Spawn; m_hasBackground = result.hasBackground; currentBackgroundKey = result.backgroundKey;
    if (!result.musicTrack.empty()) { audioManager.pushMusic(result.musicTrack, GameConstants::kMusicVolume); }
    spawnPlayers(); entityManager.Update(); transitionCenter = Vec2((P1_SPAWN.x + P2_SPAWN.x) * 0.5f, (P1_SPAWN.y + P2_SPAWN.y) * 0.5f); m_state = GameState::RespawnFadeIn;
}

void Game::StartWipe(Vec2 focusPoint, bool advancingLevel) {
    if (m_state == GameState::RespawnFadeOut || m_state == GameState::RespawnFadeIn) { return; }
    m_state = GameState::RespawnFadeOut; transitionCenter = focusPoint; transitionRadius = GameConstants::kTransitionRadiusStart; m_isLoadingNextLevel = advancingLevel;
}

void Game::spawnGround() {
    auto ground = entityManager.AddEntity("Ground");
    const float w = static_cast<float>(GameConstants::kWindowWidth);
    const float cx = w * 0.5f;
    const float cy = static_cast<float>(GameConstants::kWindowHeight) - GameConstants::kGroundH * 0.5f;
    ground->transform = std::make_shared<CTransform>(Vec2(cx, cy), Vec2(0.0f, 0.0f), 0.0f);
    ground->boundingBox = std::make_shared<CBoundingBox>(w, GameConstants::kGroundH);
    ground->sprite = std::make_shared<CSprite>(w, GameConstants::kGroundH, GameConstants::kGroundColor);
}

void Game::spawnPlayers() { SpawnPlayersImpl(entityManager, player1, player2, P1_SPAWN, P2_SPAWN); }

void Game::spawnDustParticles(Vec2 position, int count, float directionX) { SpawnDustParticlesImpl(entityManager, position, count, directionX); }

sf::Texture& Game::getTexture(const std::string& name) {
    auto it = textureCache.find(name); if (it != textureCache.end()) { return it->second; }
    std::string path = std::string(GameConstants::kTexturesDirectory) + name + GameConstants::kTextureExtension;
    if (!textureCache[name].loadFromFile(path)) {
        std::cerr << "!!! ERROR: Could not find " << path << " !!!" << std::endl;
        sf::Image pinkImage; pinkImage.resize({ GameConstants::kFallbackTextureSize, GameConstants::kFallbackTextureSize }, sf::Color::Magenta);
        textureCache[name].loadFromImage(pinkImage);
    }
    textureCache[name].setSmooth(false);
    return textureCache[name];
}
