#pragma once
#include"EntityManager.h"
#include"Entity.h"

#include<SFML/Graphics.hpp>

class Game {
	sf::RenderWindow window;
	EntityManager entityManager;

	int currentFrame = 0;
	bool Paused = false;
	bool Running = true;

	std::shared_ptr<Entity> player1;
	std::shared_ptr<Entity> player2;

	void init(const std::string& config);
	void SetPaused(bool Paused);

	void sMovement();
	void sUserInput();
	void sRender();
	void sCollision();

	void spawnPlayers();

public:
	Game(const std::string& config);

	void Run();

};