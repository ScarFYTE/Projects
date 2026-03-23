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