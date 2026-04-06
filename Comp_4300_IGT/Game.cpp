#include "Game.h"
#include <fstream>
#include<optional>
#include<SFML/Graphics.hpp>
#include<iostream>
#include<cmath>
#include <cstdint>

// Magic numbers as constants
namespace GameConfig {
	constexpr unsigned int WINDOW_WIDTH = 1280;
	constexpr unsigned int WINDOW_HEIGHT = 720;
	constexpr unsigned int FRAMERATE_LIMIT = 60;
	constexpr int ENEMY_SPAWN_INTERVAL = 90;
	constexpr int SPECIAL_WEAPON_DURATION = 180;
	constexpr int SPECIAL_WEAPON_SPAWN_RATE = 10;
	constexpr int SPECIAL_WEAPON_COOLDOWN = 1800;
	constexpr float SPECIAL_WEAPON_RADIUS = 100.0f;
	constexpr float ENEMY_SPAWN_BUFFER = 100.0f;
	constexpr float ENEMY_SPAWN_OFFSET = 150.0f;
	constexpr float ENTITY_ROTATION_SPEED = 3.0f;
	constexpr int SCORE_PER_KILL = 10;
	constexpr std::uint8_t MAX_ALPHA = 255;
}

// ---------------------------------------------------------------------------
// Initialization
// ---------------------------------------------------------------------------
void Game::init(const std::string& config) {
	std::ifstream file(config);
	if (!file.is_open()) {
		throw std::runtime_error("Could not open config file: " + config);
	}
	//file >> playerConfig.SR >> playerConfig.CR >> playerConfig.FR >> playerConfig.FG >> playerConfig.FB >> playerConfig.OR >> playerConfig.OG >> playerConfig.OB >> playerConfig.OT >> playerConfig.V >> playerConfig.S;
	//file >> enemyConfig.SR >> enemyConfig.CR >> enemyConfig.OR >> enemyConfig.OG >> enemyConfig.OB >> enemyConfig.OT >> enemyConfig.VMin >> enemyConfig.VMax >> enemyConfig.L >> enemyConfig.SI >> enemyConfig.SMin >> enemyConfig.SMax;
	//file >> bulletConfig.SR >> bulletConfig.CR >> bulletConfig.FR >> bulletConfig.FG >> bulletConfig.FB >> bulletConfig.OR >> bulletConfig.OG >> bulletConfig.OB >> bulletConfig.OT >> bulletConfig.V >> bulletConfig.L >> bulletConfig.S;
	//Setting Default Values for Debugging
	playerConfig = { 30.0f, 16.0f, 0, 0, 0, 255, 255, 0, 8, 12, 3 };
	enemyConfig = { 30.0f, 30.0f, 255, 255, 255, 0, 1, 3, 90, 120, 3, 10 };
	bulletConfig = { 4.0f, 3.0f, 255, 255, 0, 255, 255, 255, 1, 10, 120, 3.0f };
}

Game::Game(const std::string& config) {
	init(config);

	window.create(sf::VideoMode({ GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT }), "SFML Game");
	if (!font.openFromFile("C:/Users/Kim China/Projects/Comp_4300_IGT/Fonts/FloraisondesAmours.ttf")) {
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


void Game::Run() {
	window.setFramerateLimit(GameConfig::FRAMERATE_LIMIT);
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

// ---------------------------------------------------------------------------
// Systems
// ---------------------------------------------------------------------------

void Game::sRender() {
	window.clear();

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

void Game::sCollision() {
	// Boundary collision
	for (auto& e : entityManager.GetEntities()) {
		if (e->collision && e->shape && e->transform) {
			float radius = e->shape->getRadius();

			if (e->transform->position.x + radius > window.getSize().x ||
				e->transform->position.x - radius < 0) {
				e->transform->velocity.x *= -1;
			}

			if (e->transform->position.y + radius > window.getSize().y ||
				e->transform->position.y - radius < 0) {
				e->transform->velocity.y *= -1;
			}
		}
	}

	// Player and enemy collision
	for (auto& Enemies : entityManager.GetEntities("Enemy")) {
		if (myplayer->collision && Enemies->collision) {
			float dx = myplayer->transform->position.x - Enemies->transform->position.x;
			float dy = myplayer->transform->position.y - Enemies->transform->position.y;
			float distanceSquared = dx * dx + dy * dy;

			int combinedRadius = myplayer->shape->getRadius() + Enemies->shape->getRadius();
			if (distanceSquared <= combinedRadius * combinedRadius) {
				myplayer->Destroy();
				RespawnPlayer();
			}
		}

		// Bullet and enemy collision
		for (auto& Bullets : entityManager.GetEntities("Bullet")) {
			if (Bullets->collision && Enemies->collision) {
				float dx = Bullets->transform->position.x - Enemies->transform->position.x;
				float dy = Bullets->transform->position.y - Enemies->transform->position.y;
				float distanceSquared = dx * dx + dy * dy;

				int combinedRadius = Bullets->collision->radius + Enemies->collision->radius;

				if (distanceSquared <= combinedRadius * combinedRadius) {
					Bullets->Destroy();
					Enemies->Destroy();
					score += GameConfig::SCORE_PER_KILL;

					if (Enemies->shape->getRadius() > enemyConfig.SR / 2) {
						spawnSmallEnemy(Enemies);
					}
				}
			}
		}
	}
}

void Game::sLifespan() {
	for (auto& e : entityManager.GetEntities()) {
		if (!e->lifespan) { continue; }

		// Update lifespan
		if (e->lifespan->remaining > 0) {
			e->lifespan->remaining--;

			// Calculate alpha fade effect
			float ratio = static_cast<float>(e->lifespan->remaining) / static_cast<float>(e->lifespan->total);

			sf::Color fillColor = e->shape->getShape().getFillColor();
			sf::Color outlineColor = e->shape->getShape().getOutlineColor();

			fillColor.a = static_cast<std::uint8_t>(GameConfig::MAX_ALPHA * ratio);
			outlineColor.a = static_cast<std::uint8_t>(GameConfig::MAX_ALPHA * ratio);

			e->shape->setFillColor(fillColor);
			e->shape->getShape().setOutlineColor(outlineColor);
		}

		// Destroy if time is up
		if (e->lifespan->remaining <= 0) {
			e->Destroy();
		}
	}
}
void Game::sEnemySpawn() {
	if (currentFrame - LastEnemySpawnTime == GameConfig::ENEMY_SPAWN_INTERVAL) {
		spawnEnemy();
		LastEnemySpawnTime = currentFrame;
	}
}

void Game::sMovement() {
	// Reset player velocity
	myplayer->transform->velocity = { 0.0f, 0.0f };

	// Player movement
	if (myplayer->input->down && myplayer->transform->position.y + playerConfig.SR < window.getSize().y) {
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

	// Special weapon logic
	if (mIsSpecialActive) {
		int activeDuration = currentFrame - mSpecialStartTime;
		if (activeDuration < GameConfig::SPECIAL_WEAPON_DURATION) {
			if (activeDuration % GameConfig::SPECIAL_WEAPON_SPAWN_RATE == 0) {
				spawnSpecialWeapon(myplayer);
			}
		}
		else {
			LastPlayerSpecialWeaponTime = currentFrame;
			mIsSpecialActive = false;
		}
	}

	// Update all entity positions
	for (auto& e : entityManager.GetEntities()) {
		if (e->transform) {
			e->transform->position.x += e->transform->velocity.x;
			e->transform->position.y += e->transform->velocity.y;
			e->transform->rotation += GameConfig::ENTITY_ROTATION_SPEED;
		}
	}
}

void Game::sUserInput() {
	while (const std::optional<sf::Event> event = window.pollEvent()) {
		if (event->is<sf::Event::Closed>()) {
			Running = false;
		}

		if (event->is<sf::Event::KeyPressed>()) {
			const auto& keyPress = event->getIf<sf::Event::KeyPressed>();
			switch (keyPress->code) {
			case sf::Keyboard::Key::W:
				myplayer->input->up = true;
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
				if (!mIsSpecialActive && (currentFrame - LastPlayerSpecialWeaponTime >= GameConfig::SPECIAL_WEAPON_COOLDOWN)) {
					mIsSpecialActive = true;
					mSpecialStartTime = currentFrame;
					LastPlayerSpecialWeaponTime = currentFrame;
				}
				break;
			default:
				break;
			}
		}

		if (event->is<sf::Event::KeyReleased>()) {
			const auto& keyRelease = event->getIf<sf::Event::KeyReleased>();
			switch (keyRelease->code) {
			case sf::Keyboard::Key::W:
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
			default:
				break;
			}
		}

		if (event->is<sf::Event::MouseButtonPressed>()) {
			const auto& mousePress = event->getIf<sf::Event::MouseButtonPressed>();
			if (mousePress->button == sf::Mouse::Button::Left) {
				myplayer->input->shoot = true;
				Vec2 mousePos = { static_cast<float>(mousePress->position.x), static_cast<float>(mousePress->position.y) };
				spawnBullet(myplayer, mousePos);
			}
		}

		if (event->is<sf::Event::MouseButtonReleased>()) {
			const auto& mouseRelease = event->getIf<sf::Event::MouseButtonReleased>();
			if (mouseRelease->button == sf::Mouse::Button::Left) {
				myplayer->input->shoot = false;
			}
		}
	}
}



// ---------------------------------------------------------------------------
// Spawners
// ---------------------------------------------------------------------------

void Game::RespawnPlayer() {
	if (lives != 0) {
		spawnPlayer();
		lives--;
		return;
	}
	else {
		Running = false;
		return;
	}
}


void Game::spawnPlayer() {
	std::shared_ptr<Entity> player = entityManager.AddEntity("Player");

	player->transform = std::make_shared<CTransform>();
	player->collision = std::make_shared<CCollision>(playerConfig.SR);
	player->shape = std::make_shared<CShape>(
		playerConfig.SR,
		static_cast<int>(playerConfig.S),
		sf::Color(playerConfig.FR, playerConfig.FG, playerConfig.FB),
		sf::Color(playerConfig.OR, playerConfig.OG, playerConfig.OB),
		playerConfig.OT
	);
	player->input = std::make_shared<CInput>();

	player->transform->position = { GameConfig::WINDOW_WIDTH / 2.0f, GameConfig::WINDOW_HEIGHT / 2.0f };
	myplayer = player;
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity) {
	// Implement special weapon spawning logic here
	int Amount = entity->shape->getPointCount() + 4;// Example: spawn N bullets in a circular patternw
	for (int i = 0;i < Amount;i++) {
		//spawnBullet(entity, { entity->transform->position.x + cos(sf::degrees(360.0f / Amount * i).asRadians()) * 100, entity->transform->position.y + sin(sf::degrees(360.0f / Amount * i).asRadians()) * 100 });
		std::shared_ptr<Entity> bullet = entityManager.AddEntity("Bullet");
		bullet->transform = std::make_shared<CTransform>();
		bullet->collision = std::make_shared<CCollision>(bulletConfig.SR);
		bullet->lifespan = std::make_shared<CLifeSpan>(bulletConfig.L);
		bullet->shape = std::make_shared<CShape>(
			10.0f,
			myplayer->shape->getPointCount(), // Bullets are same shape as player but smaller
			sf::Color(50, 125 , 188),
			sf::Color(0, 0, 0),
			bulletConfig.OT
		);
		bullet->transform->position = entity->transform->position;
		Vec2 velocity = {
			cos(sf::degrees(360.0f / Amount * i).asRadians()) * 5.0f,
			sin(sf::degrees(360.0f / Amount * i).asRadians()) * 5.0f
		};
		bullet->transform->velocity = velocity;
	}
}


void Game::SetPosition(std::shared_ptr<Entity> entity) {
	entity->shape->setPosition(entity->transform->position);
	entity->shape->setRotation(entity->transform->rotation);
}


void Game::spawnEnemy() {
	float size = rand() % static_cast<int>(enemyConfig.SR) + enemyConfig.SR / 2.5f;
	std::shared_ptr<Entity> enemy = entityManager.AddEntity("Enemy");
	enemy->transform = std::make_shared<CTransform>();
	enemy->collision = std::make_shared<CCollision>(size);

	int sides = static_cast<int>(playerConfig.S);
	if (sides <= 0) sides = 3;

	enemy->shape = std::make_shared<CShape>(
		size,
		sides,
		sf::Color(rand() % enemyConfig.OR, rand() % enemyConfig.OG, rand() % enemyConfig.OB),
		sf::Color(rand() % enemyConfig.OR, rand() % enemyConfig.OG, rand() % enemyConfig.OB),
		enemyConfig.OT
	);

	// Random spawn position
	Vec2 Position = {
		static_cast<float>(rand() % window.getSize().x),
		static_cast<float>(rand() % window.getSize().y)
	};

	// Keep away from player
	if (Position.x > myplayer->transform->position.x - GameConfig::ENEMY_SPAWN_BUFFER &&
		Position.x < myplayer->transform->position.x + GameConfig::ENEMY_SPAWN_BUFFER) {
		Position.x = myplayer->transform->position.x + GameConfig::ENEMY_SPAWN_OFFSET;
	}

	// Random velocity
	float vx = static_cast<float>(rand() % static_cast<int>(enemyConfig.VMax - enemyConfig.VMin + 1) + static_cast<int>(enemyConfig.VMin));
	float vy = static_cast<float>(rand() % static_cast<int>(enemyConfig.VMax - enemyConfig.VMin + 1) + static_cast<int>(enemyConfig.VMin));
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
			Amount,
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
		myplayer->shape->getPointCount(),
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
