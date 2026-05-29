#pragma once
#include "AudioManager.h"
#include "Entity.h"
#include "EntityManager.h"
#include "GameState.h"
#include "LevelQueue.h"
#include "Systems/AISystem.h"
#include "Systems/HealthSystem.h"
#include "Systems/InputSystem.h"
#include "Systems/InteractSystem.h"
#include "Systems/PhysicsSystem.h"
#include "Systems/RenderSystem.h"
#include <SFML/Graphics.hpp>
#include <map>
#include <string>

class Game {
static constexpr unsigned int WINDOW_WIDTH = 1280;
static constexpr unsigned int WINDOW_HEIGHT = 720;
static constexpr float GRAVITY = 0.5f;
static constexpr float MAX_FALL_SPEED = 15.0f;
static constexpr float JUMP_VELOCITY = -8.0f;
static constexpr float MOVE_SPEED = 1.5f;
static constexpr float PLAYER_W = 24.0f;
static constexpr float PLAYER_H = 24.0f;
static constexpr float GROUND_H = 20.0f;
static constexpr float ACCELERATION = 0.8f;
static constexpr float FRICTION = 0.75f;
static constexpr float TURN_FRICTION = 0.55f;
static constexpr float MAX_MOVE_SPEED = 7.0f;
Vec2 P1_SPAWN = Vec2(WINDOW_WIDTH * 0.25f, WINDOW_HEIGHT - GROUND_H - PLAYER_H * 0.5f);
Vec2 P2_SPAWN = Vec2(WINDOW_WIDTH * 0.5f, WINDOW_HEIGHT - GROUND_H - PLAYER_H * 0.5f);

AudioManager audioManager;

sf::RenderWindow window;
sf::Font font;
sf::Texture p1HeartTex;
sf::Texture p2HeartTex;

std::map<std::string, sf::Texture> textureCache;
sf::Texture& getTexture(const std::string& name);

sf::Texture dummyBgTex;
sf::Sprite backgroundSprite{ dummyBgTex };
bool m_hasBackground = false;

LevelQueue levelQueue;
std::string currentLevelPath;
bool m_isLoadingNextLevel = false;

EntityManager entityManager;
bool m_isRunning = true;
int m_currentFrame = 0;

float transitionRadius = 3000.0f;
Vec2 transitionCenter = { 0.0f, 0.0f };

sf::View gameView;
float baseZoom = 1.0f;

std::shared_ptr<Entity> player1;  // WASD
std::shared_ptr<Entity> player2;  // Arrow keys

GameState m_state = GameState::StartMenu;
int m_selectedOption = 0; // 0 = play, 1 = exit

void init();
void spawnGround();
void spawnPlayers();
void spawnDustParticles(Vec2 position, int count = 6, float directionX = 0.0f);

// Levels
void LoadNextLevel();
void StartWipe(Vec2 focusPoint, bool advancingLevel = false);

public:
Game();
void Run();
};
