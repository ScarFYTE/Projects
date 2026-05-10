#pragma once
#include "EntityManager.h"
#include "Entity.h"
#include <SFML/Graphics.hpp>

class Game {
	// -----------------------------------------------------------------------
	// Constants
	// -----------------------------------------------------------------------
	static constexpr unsigned int WINDOW_WIDTH  = 1280;
	static constexpr unsigned int WINDOW_HEIGHT = 720;
	static constexpr float GRAVITY        =  0.5f;
	static constexpr float MAX_FALL_SPEED =  15.0f;
	static constexpr float JUMP_VELOCITY  = -13.0f;
	static constexpr float MOVE_SPEED     =  5.0f;
	static constexpr float PLAYER_W       =  50.0f;
	static constexpr float PLAYER_H       =  50.0f;
	static constexpr float GROUND_H       =  20.0f;
	static constexpr float ACCELERATION = 1.2f;  // speed gained per frame
	static constexpr float FRICTION = 0.75f; // multiplier when no input (0-1, lower = more friction)
	static constexpr float TURN_FRICTION = 0.55f; // multiplier on direction change (faster stop)
	static constexpr float MAX_MOVE_SPEED = 7.0f;  // replaces MOVE_SPEED for horizontal cap
	Vec2 P1_SPAWN = Vec2(WINDOW_WIDTH * 0.25f, WINDOW_HEIGHT - GROUND_H - PLAYER_H * 0.5f);
	Vec2 P2_SPAWN = Vec2(WINDOW_WIDTH * 0.5f, WINDOW_HEIGHT - GROUND_H - PLAYER_H * 0.5f);

	// -----------------------------------------------------------------------
	// Members
	// -----------------------------------------------------------------------
	sf::RenderWindow window;
	sf::Font font;
	EntityManager    entityManager;
	bool Running      = true;
	int  currentFrame = 0;

	sf::View gameView;
	float baseZoom = 1.0f;

	std::shared_ptr<Entity> player1;  // WASD
	std::shared_ptr<Entity> player2;  // Arrow keys

	// -----------------------------------------------------------------------
	// Internal helpers
	// -----------------------------------------------------------------------
	void init();
	void loadConfig(const std::string& path);
	void spawnGround();
	void spawnPlayers();
	void spawnDustParticles(Vec2 position, int count = 6, float directionX = 0.0f);
	void RenderHud();
	void KillPlayer(std::shared_ptr<Entity> player);
	void RespawnPlayer(std::shared_ptr<Entity> player);
	// Systems
	void sUserInput();
	void sGravity();
	void sMovement();
	void sCollision();
	void sRender();
	void sCamera();
	void sParticle();
	void sHealth();
	void sMovingPlatform();

public:


	Game();
	void Run();
};