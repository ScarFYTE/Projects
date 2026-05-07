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

	// -----------------------------------------------------------------------
	// Members
	// -----------------------------------------------------------------------
	sf::RenderWindow window;
	EntityManager    entityManager;
	bool Running      = true;
	int  currentFrame = 0;

	std::shared_ptr<Entity> player1;  // WASD
	std::shared_ptr<Entity> player2;  // Arrow keys

	// -----------------------------------------------------------------------
	// Internal helpers
	// -----------------------------------------------------------------------
	void init();
	void loadConfig(const std::string& path);
	void spawnGround();
	void spawnPlayers();

	// Systems
	void sUserInput();
	void sGravity();
	void sMovement();
	void sCollision();
	void sRender();

public:


	Game();
	void Run();
};