#include "Game.h"
#include <optional>
#include <SFML/Graphics.hpp>

// ---------------------------------------------------------------------------
// Construction / Initialization
// ---------------------------------------------------------------------------

Game::Game() {
	init();
}

void Game::init() {
	window.create(sf::VideoMode({ WINDOW_WIDTH, WINDOW_HEIGHT }), "2D Platformer - 2 Player");
	window.setFramerateLimit(60);
	spawnGround();
	spawnPlayers();
	// Flush the spawn queue before the first frame
	entityManager.Update();
}

// ---------------------------------------------------------------------------
// Spawners
// ---------------------------------------------------------------------------

void Game::spawnGround() {
	auto ground = entityManager.AddEntity("Ground");

	const float w = static_cast<float>(WINDOW_WIDTH);
	const float cx = w * 0.5f;
	const float cy = static_cast<float>(WINDOW_HEIGHT) - GROUND_H * 0.5f;

	ground->transform   = std::make_shared<CTransform>(Vec2(cx, cy), Vec2(0.0f, 0.0f), 0.0f);
	ground->boundingBox = std::make_shared<CBoundingBox>(w, GROUND_H);
	ground->sprite      = std::make_shared<CSprite>(w, GROUND_H, sf::Color(100, 80, 60));
}

void Game::spawnPlayers() {
	const float groundTop = static_cast<float>(WINDOW_HEIGHT) - GROUND_H;
	const float spawnY    = groundTop - PLAYER_H * 0.5f;

	// --- Player 1 (WASD) --- blue box, left side
	player1 = entityManager.AddEntity("Player");
	player1->transform   = std::make_shared<CTransform>(
		Vec2(WINDOW_WIDTH * 0.25f, spawnY), Vec2(0.0f, 0.0f), 0.0f);
	player1->boundingBox = std::make_shared<CBoundingBox>(PLAYER_W, PLAYER_H);
	player1->sprite      = std::make_shared<CSprite>(PLAYER_W, PLAYER_H, sf::Color(50, 100, 200));
	player1->input       = std::make_shared<CInput>();

	// --- Player 2 (Arrow keys) --- red box, right side
	player2 = entityManager.AddEntity("Player");
	player2->transform   = std::make_shared<CTransform>(
		Vec2(WINDOW_WIDTH * 0.75f, spawnY), Vec2(0.0f, 0.0f), 0.0f);
	player2->boundingBox = std::make_shared<CBoundingBox>(PLAYER_W, PLAYER_H);
	player2->sprite      = std::make_shared<CSprite>(PLAYER_W, PLAYER_H, sf::Color(200, 50, 50));
	player2->input       = std::make_shared<CInput>();
}

// ---------------------------------------------------------------------------
// Main loop
// ---------------------------------------------------------------------------

void Game::Run() {
	while (Running) {
		entityManager.Update();
		sUserInput();
		sGravity();
		sMovement();
		sCollision();
		sRender();
		currentFrame++;
	}
}

// ---------------------------------------------------------------------------
// Systems
// ---------------------------------------------------------------------------

void Game::sUserInput() {
	while (const std::optional<sf::Event> event = window.pollEvent()) {

		if (event->is<sf::Event::Closed>()) {
			Running = false;
		}

		if (const auto* kp = event->getIf<sf::Event::KeyPressed>()) {
			switch (kp->code) {
			// --- Player 1 (WASD) ---
			case sf::Keyboard::Key::W:
				if (player1->transform->onGround) {
					player1->transform->velocity.y = JUMP_VELOCITY;
				}
				break;
			case sf::Keyboard::Key::A: player1->input->left  = true; break;
			case sf::Keyboard::Key::D: player1->input->right = true; break;

			// --- Player 2 (Arrow keys) ---
			case sf::Keyboard::Key::Up:
				if (player2->transform->onGround) {
					player2->transform->velocity.y = JUMP_VELOCITY;
				}
				break;
			case sf::Keyboard::Key::Left:  player2->input->left  = true; break;
			case sf::Keyboard::Key::Right: player2->input->right = true; break;

			default: break;
			}
		}

		if (const auto* kr = event->getIf<sf::Event::KeyReleased>()) {
			switch (kr->code) {
			case sf::Keyboard::Key::A: player1->input->left  = false; break;
			case sf::Keyboard::Key::D: player1->input->right = false; break;
			case sf::Keyboard::Key::W: player1->input->jump  = false; break;
			case sf::Keyboard::Key::Left:  player2->input->left  = false; break;
			case sf::Keyboard::Key::Right: player2->input->right = false; break;
			case sf::Keyboard::Key::Up: player1->input->jump	 = false; break;
			
			default: break;
			}
		}
	}
}

void Game::sGravity() {
	for (auto& e : entityManager.GetEntities("Player")) {
		if (!e->transform) { continue; }

		// Reset ground flag every frame; sCollision will re-set it if still grounded
		e->transform->onGround = false;

		e->transform->velocity.y += GRAVITY;
		if (e->transform->velocity.y > MAX_FALL_SPEED) {
			e->transform->velocity.y = MAX_FALL_SPEED;
		}
	}
}

void Game::sMovement() {
	for (auto& e : entityManager.GetEntities("Player")) {
		if (!e->transform || !e->input) { continue; }

		// Horizontal input overrides x-velocity each frame
		e->transform->velocity.x = 0.0f;
		if (e->input->left)  { e->transform->velocity.x = -MOVE_SPEED; }
		if (e->input->right) { e->transform->velocity.x =  MOVE_SPEED; }
		if (e->input->jump) { e->transform->velocity.y = JUMP_VELOCITY; }
		e->transform->position.x += e->transform->velocity.x;
		e->transform->position.y += e->transform->velocity.y;
	}
}

void Game::sCollision() {
	const float groundTop = static_cast<float>(WINDOW_HEIGHT) - GROUND_H;
	const float wf        = static_cast<float>(WINDOW_WIDTH);

	for (auto& e : entityManager.GetEntities("Player")) {
		if (!e->transform || !e->boundingBox) { continue; }

		const float hw = e->boundingBox->halfSize.x;
		const float hh = e->boundingBox->halfSize.y;

		// Floor
		if (e->transform->position.y + hh >= groundTop) {
			e->transform->position.y = groundTop - hh;
			e->transform->velocity.y = 0.0f;
			e->transform->onGround   = true;
		}

		// Ceiling
		if (e->transform->position.y - hh < 0.0f) {
			e->transform->position.y = hh;
			e->transform->velocity.y = 0.0f;
		}

		// Left wall
		if (e->transform->position.x - hw < 0.0f) {
			e->transform->position.x = hw;
			e->transform->velocity.x = 0.0f;
		}

		// Right wall
		if (e->transform->position.x + hw > wf) {
			e->transform->position.x = wf - hw;
			e->transform->velocity.x = 0.0f;
		}
	}
}

void Game::sRender() {
	window.clear(sf::Color(30, 30, 50));

	for (auto& e : entityManager.GetEntities()) {
		if (e->transform && e->sprite) {
			e->sprite->setPosition(e->transform->position);
			window.draw(e->sprite->getShape());
		}
	}

	window.display();
}