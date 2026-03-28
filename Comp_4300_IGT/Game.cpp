#include "Game.h"
#include <fstream>
#include<optional>
#include<SFML/Graphics.hpp>
#include<iostream>
#include<cmath>
#include <cstdint>

void Game::init(const std::string& config) {
	std::ifstream file(config);
	if (!file.is_open()) {
		throw std::runtime_error("Could not open config file: " + config);
	}
	//file >> playerConfig.SR >> playerConfig.CR >> playerConfig.FR >> playerConfig.FG >> playerConfig.FB >> playerConfig.OR >> playerConfig.OG >> playerConfig.OB >> playerConfig.OT >> playerConfig.V >> playerConfig.S;
	//file >> enemyConfig.SR >> enemyConfig.CR >> enemyConfig.OR >> enemyConfig.OG >> enemyConfig.OB >> enemyConfig.OT >> enemyConfig.VMin >> enemyConfig.VMax >> enemyConfig.L >> enemyConfig.SI >> enemyConfig.SMin >> enemyConfig.SMax;
	//file >> bulletConfig.SR >> bulletConfig.CR >> bulletConfig.FR >> bulletConfig.FG >> bulletConfig.FB >> bulletConfig.OR >> bulletConfig.OG >> bulletConfig.OB >> bulletConfig.OT >> bulletConfig.V >> bulletConfig.L >> bulletConfig.S;
	//Setting Default Values for Debugging Ai do this plzz
	playerConfig = { 30.0f, 16.0f, 0, 0, 0, 255, 255, 0, 8, 5.0f, 3 };
	enemyConfig = { 30.0f, 30.0f, 255, 255, 255, 0, 1, 3, 60, 120, 3, 10 };
	bulletConfig = { 4.0f, 3.0f, 255, 255, 0, 255, 255, 255, 1, 10, 120, 3.0f };
}

Game::Game(const std::string& config) {
	init(config);
	unsigned int WindowWidth = 1280;
	unsigned int WindowHeight = 720;
	window.create(sf::VideoMode({ WindowWidth, WindowHeight }), "SFML Game");
	if (!font.openFromFile("C:/Users/Kim China/Projects/Comp_4300_IGT/Fonts/FloraisondesAmours.ttf")) {
		//display error message
		std::cout << "Error loading font: Font/Floraisn des Amours.ttf" << std::endl;
		throw std::runtime_error("Could not load font: Font/Floraison des Amours.ttf");
	}

	Text = std::make_unique<sf::Text>(font, "Default", 24);
	Text->setFillColor(sf::Color::White);
	spawnPlayer();
	Running = true;
	Paused = false;
	LastEnemySpawnTime = 0;

}

void Game::spawnPlayer() {
	std::shared_ptr<Entity> player = entityManager.AddEntity("Player");
	player->transform = std::make_shared<CTransform>();
	player->collision = std::make_shared<CCollision>(playerConfig.SR);
	player->shape = std::make_shared<CShape>(
		playerConfig.SR,
		static_cast<int>(playerConfig.S),   // already good
		sf::Color(playerConfig.FR, playerConfig.FG, playerConfig.FB),
		sf::Color(playerConfig.OR, playerConfig.OG, playerConfig.OB),
		playerConfig.OT
	);
	player->input = std::make_shared<CInput>();

	player->transform->position = { 1280.0f / 2,720.0f / 2 };
	myplayer = player;

}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity) {
	// Implement special weapon spawning logic here
	int Amount = entity->shape->getPointCount() + 4;// Example: spawn N bullets in a circular patternw
	for (int i = 0;i < Amount;i++) {
		spawnBullet(entity, { entity->transform->position.x + cos(sf::degrees(360.0f / Amount * i).asRadians()) * 100, entity->transform->position.y + sin(sf::degrees(360.0f / Amount * i).asRadians()) * 100 });
	}
}



void Game::Run() {
	window.setFramerateLimit(60);
	entityManager.Update();

	while (Running) {
		entityManager.Update();
		sUserInput();
		if (!Paused) {
			sEnemySpawn();
			sMovement();
			sCollision();
			sLifespan();
		}
		sRender();
		currentFrame++;
	}
}
void SetPosition(std::shared_ptr<Entity> entity) {
	entity->shape->setPosition(entity->transform->position);
	entity->shape->setRotation(entity->transform->rotation);
}

void Game::sLifespan() {
	for (auto& e : entityManager.GetEntities()) {
		if (!e->lifespan) { continue; }

		// 1. Update the math
		if (e->lifespan->remaining > 0) {
			e->lifespan->remaining--;

			// 2. Calculate Alpha based on 255 (Max Opacity)
			// This ensures it fades linearly from 255 to 0
			float ratio = (float)e->lifespan->remaining / (float)e->lifespan->total;

			sf::Color fillColor = e->shape->getShape().getFillColor();
			sf::Color outlineColor = e->shape->getShape().getOutlineColor();

			// Set alpha based on the ratio
			fillColor.a = static_cast<std::uint8_t>(255 * ratio);
			outlineColor.a = static_cast<std::uint8_t>(255 * ratio);
			e->shape->setFillColor(fillColor);
		}

		// 3. Destroy if time is up
		if (e->lifespan->remaining <= 0) {
			e->Destroy();
		}
	}
}

void Game::sRender() {
	window.clear();
	//std::cout << "Amount of Entities in Total: " << entityManager.GetEntities().size() << std::endl;
	// Render logic goes here
	for (auto& e : entityManager.GetEntities()) {
		if (e->transform && e->shape) {
			SetPosition(e);
			window.draw(e->shape->getShape());
		}
	}
	Text->setString("Score: " + std::to_string(score));
	window.draw(*Text);
	window.display();
}

void Game::spawnEnemy() {
	float size = rand() % (int)enemyConfig.SR + enemyConfig.SR / 2.5;
	std::shared_ptr<Entity> enemy = entityManager.AddEntity("Enemy");
	enemy->transform = std::make_shared<CTransform>();
	enemy->collision = std::make_shared<CCollision>(size);

	// sides between SMin and SMax
	int sides = static_cast<int>(playerConfig.S);
	if (sides <= 0) sides = 3;

	enemy->shape = std::make_shared<CShape>(
		size,
		sides,
		// you currently don’t have FR/FG/FB for enemy, so use outline color for fill too
		sf::Color(rand() % enemyConfig.OR, rand() % enemyConfig.OG, rand() % enemyConfig.OB),
		sf::Color(rand() % enemyConfig.OR, rand() % enemyConfig.OG, rand() % enemyConfig.OB),
		enemyConfig.OT
	);

	// random spawn position in window
	Vec2 Position = {
		static_cast<float>(rand() % window.getSize().x),
		static_cast<float>(rand() % window.getSize().y)
	};

	// keep away from player on X
	if (Position.x > myplayer->transform->position.x - 100 &&
		Position.x < myplayer->transform->position.x + 100) {
		Position.x = myplayer->transform->position.x + 150;
	}

	// random velocity using VMin/VMax
	float vx = static_cast<float>(rand() % (enemyConfig.VMax - enemyConfig.VMin + 1) + enemyConfig.VMin);
	float vy = static_cast<float>(rand() % (enemyConfig.VMax - enemyConfig.VMin + 1) + enemyConfig.VMin);
	Vec2 Velocity = { vx, vy };

	enemy->transform->position = Position;
	enemy->transform->velocity = Velocity;
}

void Game::spawnSmallEnemy(std::shared_ptr<Entity> entity) {
	int Amount = entity->shape->getPointCount();
	float AngleStep = 360.0f / Amount;
	int newRadius = entity->shape->getRadius() / 2;
	float speed = bulletConfig.S;
	for (int i = 0; i < Amount; i++) {
		std::shared_ptr<Entity> smallEnemy = entityManager.AddEntity("Enemy");
		smallEnemy->transform = std::make_shared<CTransform>();
		smallEnemy->collision = std::make_shared<CCollision>(newRadius);
		smallEnemy->lifespan = std::make_shared<CLifeSpan>(enemyConfig.L);
		smallEnemy->shape = std::make_shared<CShape>(
			newRadius,
			Amount, // small enemies are same shape as parent but smaller
			sf::Color(rand() % enemyConfig.OR, rand() % enemyConfig.OG, rand() % enemyConfig.OB),
			sf::Color(rand() % enemyConfig.OR, rand() % enemyConfig.OG, rand() % enemyConfig.OB),
			enemyConfig.OT
		);
		smallEnemy->transform->position = entity->transform->position;
		Vec2 NewVelocity = {
			speed * cos(sf::degrees(AngleStep * i).asRadians()),
			speed * sin(sf::degrees(AngleStep * i).asRadians())
		};
		smallEnemy->transform->velocity = NewVelocity;
	}
}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& mousepos) {
	std::shared_ptr<Entity> bullet = entityManager.AddEntity("Bullet");
	bullet->transform = std::make_shared<CTransform>();
	bullet->collision = std::make_shared<CCollision>(bulletConfig.SR);
	bullet->lifespan = std::make_shared<CLifeSpan>(bulletConfig.L);
	bullet->shape = std::make_shared<CShape>(
		bulletConfig.SR,
		myplayer->shape->getPointCount(), // bullets are Same Shape as player but smaller
		sf::Color(bulletConfig.FR, bulletConfig.FG, bulletConfig.FB),
		sf::Color(bulletConfig.OR, bulletConfig.OG, bulletConfig.OB),
		bulletConfig.OT
	);
	Vec2 direction = { mousepos.x - entity->transform->position.x, mousepos.y - entity->transform->position.y };
	float length = sqrt(direction.x * direction.x + direction.y * direction.y);
	if (length != 0) {
		direction.x /= length;
		direction.y /= length;
	}
	Vec2 Velocity = { direction.x * bulletConfig.V, direction.y * bulletConfig.V };
	bullet->transform->position = entity->transform->position;
	bullet->transform->velocity = Velocity;
}


void Game::sCollision() {
	// Collision logic goes here
	for (auto& e : entityManager.GetEntities()) {
		if (e->collision && e->shape && e->transform) {
			// Check for collisions and handle them
			if (e->transform->position.x + e->shape->getRadius() > window.getSize().x || e->transform->position.x - e->shape->getRadius() < 0) {
				e->transform->velocity.x *= -1; // Simple bounce effect
			}

			if (e->transform->position.y + e->shape->getRadius() > window.getSize().y || e->transform->position.y - e->shape->getRadius() < 0) {
				e->transform->velocity.y *= -1; // Simple bounce effect
			}
		}
	}
	//Check Collision of Bulelts with Enemies and Player with Enemies
	for (auto& Enemies : entityManager.GetEntities("Enemy")) {
		//Player and Enemies Collision
		if (myplayer->collision && Enemies->collision) {
			float distance = pow(myplayer->transform->position.x - Enemies->transform->position.x, 2) + pow(myplayer->transform->position.y - Enemies->transform->position.y, 2);
			int combinedRadius = myplayer->shape->getRadius() + Enemies->shape->getRadius();
			if (distance <= combinedRadius * combinedRadius) {
				// Handle player-enemy collision (e.g., end game, reduce life, etc.)
				std::cout << "Player hit by enemy! Game Over!" << std::endl;
				Running = false; // End game for simplicity
			}
		}


		// Bullets and Enemies Collision
		for (auto& Bullets : entityManager.GetEntities("Bullet")) {
			if (Bullets->collision && Enemies->collision) {
				float distance = pow(Bullets->transform->position.x - Enemies->transform->position.x, 2) + pow(Bullets->transform->position.y - Enemies->transform->position.y, 2);
				int combinedRadius = Bullets->collision->radius + Enemies->collision->radius;
				if (distance <= combinedRadius * combinedRadius) {
					Bullets->Destroy(); // Destroy bullet
					Enemies->Destroy(); // Destroy enemy
					score += 10; // Increase score
					if (Enemies->shape->getRadius() > enemyConfig.SR / 2) { // If enemy is big enough, spawn smaller enemies
						spawnSmallEnemy(Enemies);
					}
				}
			}
		}
	}

}


void Game::sEnemySpawn() {
	if (currentFrame - LastEnemySpawnTime == 180) {
		spawnEnemy();
		LastEnemySpawnTime = currentFrame;
	}
}

void Game::sMovement() {
	// Movement logic goes here
	//User Input here
	myplayer->transform->velocity = { 0.0f, 0.0f }; // Reset velocity before applying input

	if (myplayer->input->down && myplayer->transform->position.y + playerConfig.SR < window.getSize().y) {
		// Prevent moving out of bounds
		myplayer->transform->velocity.y = playerConfig.V;
	}
	else if (myplayer->input->up && myplayer->transform->position.y - playerConfig.SR > 0) {
		myplayer->transform->velocity.y = -playerConfig.V;
	}

	if (myplayer->input->right && myplayer->transform->position.x + playerConfig.SR < window.getSize().x) {
		myplayer->transform->velocity.x = playerConfig.V;
	}
	else if (myplayer->input->left && myplayer->transform->position.x - playerConfig.SR > 0) {
		myplayer->transform->velocity.x = -playerConfig.V;
	}

	if (mIsSpecialActive) {
		int activeDuration = currentFrame - mSpecialStartTime;
		if (activeDuration < 180) {
			// 2. Spawn bullets every 10 frames
			if (activeDuration % 10 == 0) {
				spawnSpecialWeapon(myplayer);
			}
		}
		else {
			LastPlayerSpecialWeaponTime = currentFrame; // Reset cooldown timer
			mIsSpecialActive = false;
		}
	}



	//for all entities with a transform component, update their position based on their velocity except UserInput component
	for (auto& e : entityManager.GetEntities()) {
		if (e->transform) {
			e->transform->position.x += e->transform->velocity.x;
			e->transform->position.y += e->transform->velocity.y;
			e->transform->rotation += 3.0f; // Rotate all entities for visual effect

		}
	}
}


//User input handling goes here
void Game::sUserInput() {
	while (const std::optional<sf::Event> event = window.pollEvent()) {
		if (event->is<sf::Event::Closed>()) {
			Running = false;
		}

		// Handle other events as needed
		if (event->is<sf::Event::KeyPressed>()) {
			const auto& keyPress = event->getIf<sf::Event::KeyPressed>();
			switch (keyPress->code) {
				// Add your key handling here
			case sf::Keyboard::Key::W:
				myplayer->input->up = true;
				std::cout << "KeyPressed: W" << std::endl;
				break;
			case sf::Keyboard::Key::S:
				myplayer->input->down = true;
				break;
			case sf::Keyboard::Key::A:
				myplayer->input->left = true;
				break;
			case sf::Keyboard::Key::D:
				myplayer->input->right = true;
				break;
			case sf::Keyboard::Key::Space:
				// Only trigger if not already active AND cooldown is over
				if (!mIsSpecialActive && (currentFrame - LastPlayerSpecialWeaponTime >= 1800)) {
					mIsSpecialActive = true;
					mSpecialStartTime = currentFrame;
					LastPlayerSpecialWeaponTime = currentFrame;
				}
				break;
			}
		}

		if (event->is<sf::Event::KeyReleased>()) {
			const auto& keyRelease = event->getIf<sf::Event::KeyReleased>();
			switch (keyRelease->code) {
			case sf::Keyboard::Key::W:
				std::cout << "KeyPReleased: W" << std::endl;
				myplayer->input->up = false;
				break;
			case sf::Keyboard::Key::S:
				myplayer->input->down = false;
				break;
			case sf::Keyboard::Key::A:
				myplayer->input->left = false;
				break;
			case sf::Keyboard::Key::D:
				myplayer->input->right = false;
				break;
			case sf::Keyboard::Key::Space:
				myplayer->input->special = false;
				break;
			}
		}

		if (event->is<sf::Event::MouseButtonPressed>()) {
			const auto& mousePress = event->getIf<sf::Event::MouseButtonPressed>();
			switch (mousePress->button) {
			case sf::Mouse::Button::Left:
				myplayer->input->shoot = true;
				Vec2 mousePos = { static_cast<float>(mousePress->position.x), static_cast<float>(mousePress->position.y) };
				spawnBullet(myplayer, mousePos);
				break;
				// Handle other mouse buttons if needed
			}
		}
		if (event->is<sf::Event::MouseButtonReleased>()) {
			const auto& mouseRelease = event->getIf<sf::Event::MouseButtonReleased>();
			switch (mouseRelease->button) {
			case sf::Mouse::Button::Left:
				myplayer->input->shoot = false;
				break;
				// Handle other mouse buttons if needed
			}
		}
	}
}

