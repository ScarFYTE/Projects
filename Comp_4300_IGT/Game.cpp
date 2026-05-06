#include "Game.h"
#include <fstream>
#include <optional>
#include <SFML/Graphics.hpp>
#include <iostream>

// ---------------------------------------------------------------------------
// Configuration constants
// ---------------------------------------------------------------------------
namespace GameConfig {
	constexpr unsigned int WINDOW_WIDTH  = 1280;
	constexpr unsigned int WINDOW_HEIGHT = 720;
	constexpr unsigned int FRAMERATE_LIMIT = 60;

	// Player dimensions and speed
	constexpr float PLAYER_WIDTH  = 40.0f;
	constexpr float PLAYER_HEIGHT = 40.0f;
	constexpr float PLAYER_SPEED  = 4.0f;

	// Gravity applied to vertical velocity each frame
	constexpr float GRAVITY = 0.5f;

	// Upward impulse when a player jumps
	constexpr float JUMP_VELOCITY = -12.0f;

	// Fast-fall multiplier (applied when pressing down in the air)
	constexpr float FAST_FALL = 1.5f;
}

// ---------------------------------------------------------------------------
// Initialization
// ---------------------------------------------------------------------------
void Game::init(const std::string& /*config*/) {
	// Reserved for future config-file parsing
}

Game::Game(const std::string& config) {
	init(config);

	window.create(sf::VideoMode({ GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT }), "2-Player Platformer");

	spawnPlayers();

	Running = true;
	Paused  = false;
}

// ---------------------------------------------------------------------------
// Main loop
// ---------------------------------------------------------------------------
void Game::Run() {
	window.setFramerateLimit(GameConfig::FRAMERATE_LIMIT);
	entityManager.Update();

	while (Running) {
		entityManager.Update();
		sUserInput();
		if (!Paused) {
			sMovement();
			sCollision();
		}
		sRender();
		currentFrame++;
	}
}

// ---------------------------------------------------------------------------
// Systems
// ---------------------------------------------------------------------------

void Game::sRender() {
	window.clear(sf::Color(30, 30, 30));

	for (auto& e : entityManager.GetEntities()) {
		if (e->transform && e->sprite) {
			e->sprite->setPosition(e->transform->position);
			window.draw(e->sprite->getShape());
		}
	}

	window.display();
}

void Game::sCollision() {
	const float W = static_cast<float>(window.getSize().x);
	const float H = static_cast<float>(window.getSize().y);

	for (auto& e : entityManager.GetEntities()) {
		if (!e->transform || !e->bbox) { continue; }

		float halfW = e->bbox->size.x / 2.0f;
		float halfH = e->bbox->size.y / 2.0f;
		Vec2& pos   = e->transform->position;
		Vec2& vel   = e->transform->velocity;

		// Left / right walls
		if (pos.x - halfW < 0.0f) {
			pos.x = halfW;
			vel.x = 0.0f;
		}
		else if (pos.x + halfW > W) {
			pos.x = W - halfW;
			vel.x = 0.0f;
		}

		// Ceiling
		if (pos.y - halfH < 0.0f) {
			pos.y = halfH;
			vel.y = 0.0f;
		}

		// Floor — land and mark as grounded
		if (pos.y + halfH >= H) {
			pos.y = H - halfH;
			vel.y = 0.0f;
			e->transform->onGround = true;
		}
		else {
			e->transform->onGround = false;
		}
	}
}

void Game::sMovement() {
	// Helper lambda: apply movement rules to one player
	auto movePlayer = [&](std::shared_ptr<Entity>& player) {
		if (!player || !player->input || !player->transform) { return; }

		CTransform& tf  = *player->transform;
		CInput&     inp = *player->input;

		// Horizontal movement
		if (inp.left) {
			tf.velocity.x = -GameConfig::PLAYER_SPEED;
		}
		else if (inp.right) {
			tf.velocity.x = GameConfig::PLAYER_SPEED;
		}
		else {
			tf.velocity.x = 0.0f;
		}

		// Jump — only from the ground
		if (inp.up && tf.onGround) {
			tf.velocity.y = GameConfig::JUMP_VELOCITY;
			tf.onGround   = false;
		}

		// Fast fall — apply a one-time downward impulse when the player presses down in the air
		if (inp.down && !tf.onGround && tf.velocity.y >= 0.0f && tf.velocity.y < GameConfig::PLAYER_SPEED * GameConfig::FAST_FALL) {
			tf.velocity.y = GameConfig::PLAYER_SPEED * GameConfig::FAST_FALL;
		}

		// Apply gravity
		tf.velocity.y += GameConfig::GRAVITY;

		// Integrate position
		tf.position.x += tf.velocity.x;
		tf.position.y += tf.velocity.y;
	};

	movePlayer(player1);
	movePlayer(player2);
}

void Game::sUserInput() {
	while (const std::optional<sf::Event> event = window.pollEvent()) {
		if (event->is<sf::Event::Closed>()) {
			Running = false;
		}

		if (event->is<sf::Event::KeyPressed>()) {
			const auto& kp = event->getIf<sf::Event::KeyPressed>();
			switch (kp->code) {
			// Player 1 — WASD
			case sf::Keyboard::Key::W: player1->input->up    = true; break;
			case sf::Keyboard::Key::S: player1->input->down  = true; break;
			case sf::Keyboard::Key::A: player1->input->left  = true; break;
			case sf::Keyboard::Key::D: player1->input->right = true; break;

			// Player 2 — Arrow keys
			case sf::Keyboard::Key::Up:    player2->input->up    = true; break;
			case sf::Keyboard::Key::Down:  player2->input->down  = true; break;
			case sf::Keyboard::Key::Left:  player2->input->left  = true; break;
			case sf::Keyboard::Key::Right: player2->input->right = true; break;

			case sf::Keyboard::Key::P:
				SetPaused(!Paused);
				break;
			default:
				break;
			}
		}

		if (event->is<sf::Event::KeyReleased>()) {
			const auto& kr = event->getIf<sf::Event::KeyReleased>();
			switch (kr->code) {
			// Player 1
			case sf::Keyboard::Key::W: player1->input->up    = false; break;
			case sf::Keyboard::Key::S: player1->input->down  = false; break;
			case sf::Keyboard::Key::A: player1->input->left  = false; break;
			case sf::Keyboard::Key::D: player1->input->right = false; break;

			// Player 2
			case sf::Keyboard::Key::Up:    player2->input->up    = false; break;
			case sf::Keyboard::Key::Down:  player2->input->down  = false; break;
			case sf::Keyboard::Key::Left:  player2->input->left  = false; break;
			case sf::Keyboard::Key::Right: player2->input->right = false; break;

			default:
				break;
			}
		}
	}
}

// ---------------------------------------------------------------------------
// Spawners
// ---------------------------------------------------------------------------

void Game::spawnPlayers() {
	// Player 1 — blue box, starts on the left
	player1 = entityManager.AddEntity("Player");
	player1->transform = std::make_shared<CTransform>();
	player1->bbox      = std::make_shared<CBoundingBox>(GameConfig::PLAYER_WIDTH, GameConfig::PLAYER_HEIGHT);
	player1->sprite    = std::make_shared<CSprite>(GameConfig::PLAYER_WIDTH, GameConfig::PLAYER_HEIGHT, sf::Color(70, 130, 180));
	player1->input     = std::make_shared<CInput>();
	player1->transform->position = { GameConfig::WINDOW_WIDTH * 0.25f,
	                                 static_cast<float>(GameConfig::WINDOW_HEIGHT) - GameConfig::PLAYER_HEIGHT / 2.0f };

	// Player 2 — red box, starts on the right
	player2 = entityManager.AddEntity("Player");
	player2->transform = std::make_shared<CTransform>();
	player2->bbox      = std::make_shared<CBoundingBox>(GameConfig::PLAYER_WIDTH, GameConfig::PLAYER_HEIGHT);
	player2->sprite    = std::make_shared<CSprite>(GameConfig::PLAYER_WIDTH, GameConfig::PLAYER_HEIGHT, sf::Color(200, 60, 60));
	player2->input     = std::make_shared<CInput>();
	player2->transform->position = { GameConfig::WINDOW_WIDTH * 0.75f,
	                                 static_cast<float>(GameConfig::WINDOW_HEIGHT) - GameConfig::PLAYER_HEIGHT / 2.0f };
}

void Game::SetPaused(bool paused) {
	Paused = paused;
}
