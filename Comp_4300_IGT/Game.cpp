#include "Game.h"
#include <optional>
#include <SFML/Graphics.hpp>
#include <fstream>
// Construction / Initialization

Game::Game() {
	init();
}

void Game::init() {
	loadConfig("config.txt");
	font.openFromFile("Floraison des Amours.ttf");
	window.create(sf::VideoMode({ WINDOW_WIDTH, WINDOW_HEIGHT }), "2D Platformer - 2 Player");
	window.setFramerateLimit(60);
	spawnPlayers();
	// Flush the spawn queue before the first frame
	entityManager.Update();
	gameView = window.getDefaultView();
}


// Load Config
void Game::loadConfig(const std::string& path) {
	std::ifstream file(path);
	std::string type;
	while (file >> type) {
		if (type == "Tile") {
			float x, y, w, h;
			file >> x >> y >> w >> h;
			auto tile = entityManager.AddEntity("Tile");
			tile->transform = std::make_shared<CTransform>(Vec2(x, y), Vec2(0, 0), 0.0f);
			tile->boundingBox = std::make_shared<CBoundingBox>(w, h);
			tile->sprite = std::make_shared<CSprite>(w, h, sf::Color(255, 80, 60));
		}
	}
}

// ---------------------------------------------------------------------------
// Spawners
// -------------------------------------------------------------------------

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
	player1->health		 = std::make_shared<CHealth>();

	// --- Player 2 (Arrow keys) --- red box, right side
	player2 = entityManager.AddEntity("Player");
	player2->transform   = std::make_shared<CTransform>(
		Vec2(WINDOW_WIDTH * 0.75f, spawnY), Vec2(0.0f, 0.0f), 0.0f);
	player2->boundingBox = std::make_shared<CBoundingBox>(PLAYER_W, PLAYER_H);
	player2->sprite      = std::make_shared<CSprite>(PLAYER_W, PLAYER_H, sf::Color(200, 50, 50));
	player2->input       = std::make_shared<CInput>();
	player2->health		 = std::make_shared<CHealth>();
}

// Main loop

void Game::Run() {
	while (Running) {
		entityManager.Update();
		sUserInput();
		sGravity();
		sMovement();
		sCollision();
		sCamera();
		sRender();
		currentFrame++;
	}
}

// Systems
void Game::sCamera() {
	if (!player1->transform || !player2->transform) { return; }

	Vec2 p1 = player1->transform->position;
	Vec2 p2 = player2->transform->position;

	// Midpoint between both players
	float cx = (p1.x + p2.x) / 2.0f;
	float cy = (p1.y + p2.y) / 2.0f;
	gameView.setCenter({ cx, cy });

	// Distance between players — zoom out to fit both + padding
	float dist = std::sqrt((p2.x - p1.x) * (p2.x - p1.x) +
		(p2.y - p1.y) * (p2.y - p1.y));
	float minSize = static_cast<float>(WINDOW_WIDTH);          // never zoom in past default
	float needed = std::max(minSize, dist * 1.6f);            
	float viewW = needed;
	float viewH = needed * (static_cast<float>(WINDOW_HEIGHT) / WINDOW_WIDTH);

	gameView.setSize({ viewW, viewH });
	window.setView(gameView);
}

void Game::sUserInput() {
	while (const std::optional<sf::Event> event = window.pollEvent()) {

		if (event->is<sf::Event::Closed>()) {
			Running = false;
		}

		if (const auto* kp = event->getIf<sf::Event::KeyPressed>()) {
			switch (kp->code) {
			case sf::Keyboard::Key::W: player1->input->jump  = true; break;
			case sf::Keyboard::Key::A: player1->input->left  = true; break;
			case sf::Keyboard::Key::D: player1->input->right = true; break;

			case sf::Keyboard::Key::Up: player2->input->jump	 = true; break;
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
			case sf::Keyboard::Key::Up: player2->input->jump	 = false; break;
			
			default: break;
			}
		}
	}
}

void Game::sGravity() {
	for (auto& e : entityManager.GetEntities("Player")) {
		if (!e->transform) { continue; }

		
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
		if (e->input->jump && e->transform->onGround) { e->transform->velocity.y = JUMP_VELOCITY; e->transform->onGround = false; }
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

		//Tiles
		for (auto& Tile : entityManager.GetEntities("Tile")) {
			if(!Tile->transform || !Tile->boundingBox){ continue; }
			const float tileHW = Tile->boundingBox->halfSize.x;
			const float tileHH = Tile->boundingBox->halfSize.y;
			const float tileX  = Tile->transform->position.x;
			const float tileY  = Tile->transform->position.y;

			if (std::abs(e->transform->position.x - tileX) < hw + tileHW &&
				std::abs(e->transform->position.y - tileY) < hh + tileHH) {
				// Simple collision push the player out of the tile
				float overlapX = (hw + tileHW) - std::abs(e->transform->position.x - tileX);
				float overlapY = (hh + tileHH) - std::abs(e->transform->position.y - tileY);
				if (overlapX < overlapY) {
					// horizontal collision
					if (e->transform->position.x < tileX) {
						e->transform->position.x -= overlapX;
					} else {
						e->transform->position.x += overlapX;
					}
					e->transform->velocity.x = 0.0f;
				} else {
					// vertical collision
					if (e->transform->position.y < tileY) {
						e->transform->position.y -= overlapY;
						e->transform->onGround = true; // Player is on top of the tile
					} else {
						e->transform->position.y += overlapY;
					}
					e->transform->velocity.y = 0.0f;
				}
			}
		}
		for (auto& Other : entityManager.GetEntities("Player")) {
			if (Other == e || !Other->transform || !Other->boundingBox) { continue; }
			const float otherHW = Other->boundingBox->halfSize.x;
			const float otherHH = Other->boundingBox->halfSize.y;
			const float otherX = Other->transform->position.x;
			const float otherY = Other->transform->position.y;
			if (std::abs(e->transform->position.x - otherX) < hw + otherHW &&
				std::abs(e->transform->position.y - otherY) < hh + otherHH) {

				float overlapX = (hw + otherHW) - std::abs(e->transform->position.x - otherX);
				float overlapY = (hh + otherHH) - std::abs(e->transform->position.y - otherY);
				if (overlapX < overlapY) {
					// horizontal collision
					if (e->transform->position.x < otherX) {
						e->transform->position.x -= overlapX * 0.5f;
						Other->transform->position.x += overlapX * 0.5f;
					}
					else {
						e->transform->position.x += overlapX * 0.5f;
						Other->transform->position.x -= overlapX * 0.5f;
					}
					e->transform->velocity.x = 0.0f;
					Other->transform->velocity.x = 0.0f;
				}
				else {
					// vertical collision
					if (e->transform->position.y < otherY) {
						e->transform->position.y -= overlapY ;\
						e->transform->onGround = true; // Player is on top of the other player
						e->transform->velocity -= Other->transform->velocity/2;
					}
					else {
						Other->transform->position.y -= overlapY ;
						Other->transform->onGround = true;// Other is on top of the player
						Other->transform->velocity -= e->transform->velocity/2;
					}
				}
			}
		}
	}
}
void Game::KillPlayer(std::shared_ptr<Entity> e) {
	auto& h = e->health;

	h->lives--;
	h->isDead = true;
	h->respawnTimer = 180;  // 3 seconds at 60fps

	// Hide the player during respawn
	e->transform->velocity = { 0.0f, 0.0f };
	e->transform->position = { -9999.0f, -9999.0f }; // off screen
	e->GetBoundingBoxCopy(); // Save current bounding box before disabling
	e->boundingBox = nullptr;                 // disable collision

	//spawnDustParticles(e->transform->position, 12);   

	// Check game over
	if (h->lives <= 0) {
		// TODO: trigger game over state
	}
}

void Game::RespawnPlayer(std::shared_ptr<Entity> e) {
	auto& h = e->health;

	h->isDead = false;
	h->respawnTimer = 0;

	// Restore position and collision
	e->transform->position = h->spawnPoint;
	e->transform->velocity = { 0.0f, 0.0f };
	e->boundingBox = std::make_shared<CBoundingBox>(e->GetBoundingBoxCopy()); // retrieve Real bounding box from before death

	//spawnDustParticles(h->spawnPoint, 8);  // spawn puff
}

void Game::RenderHud() {

	sf::Text p1Text(font), p2Text(font);

	p1Text.setCharacterSize(20);
	p2Text.setCharacterSize(20);
	p1Text.setFillColor(sf::Color::Cyan);
	p2Text.setFillColor(sf::Color::Red);

	// Build lives string: "P1: ♥ ♥ ♥"
	std::string p1Lives = "P1: ";
	for (int i = 0; i < player1->health->lives; i++) { p1Lives += "<3 "; }
	if (player1->health->isRespawning()) {
		p1Lives += " (respawning " + std::to_string(player1->health->respawnTimer / 60 + 1) + "s)";
	}

	std::string p2Lives = "P2: ";
	for (int i = 0; i < player2->health->lives; i++) { p2Lives += "<3 ";}

	p1Text.setString(p1Lives);
	p2Text.setString(p2Lives);

	p1Text.setPosition({ 20.0f,  20.0f });
	p2Text.setPosition({ 20.0f,  50.0f });

	window.draw(p1Text);
	window.draw(p2Text);
}

void Game::sRender() {
	window.clear(sf::Color(30, 30, 50));
	window.setView(gameView);

	for (auto& e : entityManager.GetEntities()) {
		if (e->transform && e->sprite) {
			e->sprite->setPosition(e->transform->position);
			window.draw(e->sprite->getShape());
		}
	}

	window.setView(window.getDefaultView());
	RenderHud();
	window.display();
}