#include "Game.h"
#include <fstream>"
#include<SFML/Graphics.hpp>

void Game::init(const std::string& config) {
	std::ifstream file(config);
	if (!file.is_open()) {
		throw std::runtime_error("Could not open config file: " + config);
	}
	file >> playerConfig.SR >> playerConfig.CR >> playerConfig.FR >> playerConfig.FG >> playerConfig.FB >> playerConfig.OR >> playerConfig.OG >> playerConfig.OB >> playerConfig.OT >> playerConfig.V >> playerConfig.S;
	file >> enemyConfig.SR >> enemyConfig.CR >> enemyConfig.OR >> enemyConfig.OG >> enemyConfig.OB >> enemyConfig.OT >> enemyConfig.VMin >> enemyConfig.VMax >> enemyConfig.L >> enemyConfig.SI >> enemyConfig.SMin >> enemyConfig.SMax;
	file >> bulletConfig.SR >> bulletConfig.CR >> bulletConfig.FR >> bulletConfig.FG >> bulletConfig.FB >> bulletConfig.OR >> bulletConfig.OG >> bulletConfig.OB >> bulletConfig.OT >> bulletConfig.V >> bulletConfig.L >> bulletConfig.S;
}

Game::Game(const std::string& config) {
	init(config);
	int WindowWidth = 800;
	int WindowHeight = 600;
	window.create(sf::VideoMode({ 800, 600 }), "SFML Game");
	font.openFromFile("assets/arial.ttf");
	Text = std::make_unique<sf::Text>();
	Text->setFont(font);
	Text->setCharacterSize(24);
	Text->setFillColor(sf::Color::White);
	spawnPlayer();
}

void Game::spawnPlayer() {
	std::shared_ptr<Entity> player = entityManager.AddEntity("Player");
	player->transform = std::make_shared<CTransform>();
	player->collision = std::make_shared<CCollision>(playerConfig.SR);
	player->shape = std::make_shared<CShape>(playerConfig.FR, playerConfig.FG, playerConfig.FB, playerConfig.OR, playerConfig.OG, playerConfig.OB, playerConfig.OT);
	player->input = std::make_shared<CInput>();
	
	myplayer = player;

}



void Game::Run() {
	while (Running) {
		sUserInput();
		if (!Paused) {
			sMovement();
			sEnemySpawn();
			sCollision();
		}
		sRender();
		currentFrame++;
	}
}

void Game::sCollision() {
	// Collision logic goes here
	for (auto& e : entityManager.GetEntities()) {
		if(e->collision && e->shape){
			// Check for collisions and handle them
			if(e->transform->position.x + e->shape->getRadius() >window.getSize().x || e->transform->position.x - e->shape->getRadius() < 0 ){
				e->transform->velocity.x *= -1; // Simple bounce effect
			}
			
			if (e->transform->position.y + e->shape->getRadius() > window.getSize().y || e->transform->position.y - e->shape->getRadius() < 0) {
				e->transform->velocity.y *= -1; // Simple bounce effect
			}
		}
	}
}

void Game::sEnemySpawn() {
	if (currentFrame - LastEnemySpawnTime >= enemyConfig.SI) {
		spawnEnemy();
		LastEnemySpawnTime = currentFrame;
	}
}

void Game::sMovement() {
	// Movement logic goes here
	//User Input here

	


	//for all entities with a transform component, update their position based on their velocity except UserInput component
	for (auto& e : entityManager.GetEntities()) {
		if (e->transform) {
			e->transform->position.x += e->transform->velocity.x;
			e->transform->position.y += e->transform->velocity.y;
		}
	}
}

void Game::sUserInput() {
	// User input handling goes here
}