#include "Game.h"
#include <optional>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <cstdint>
// Construction / Initialization

Game::Game() {
	init();
}

void Game::init() {
	loadConfig("config.txt");

	if (font.openFromFile("Fonts/Coolvetica Rg.otf")) {
		std::cout << "Font loed successfully." << std::endl;
	}
	else {
		std::cerr << "Failed to load font ##########." << std::endl;
	}
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
			tile->sprite = std::make_shared<CSprite>(w, h, sf::Color(100, 80, 60));
		}
	}
}

// ---------------------------------------------------------------------------
// Spawners
// -------------------------------------------------------------------------
void Game::spawnDustParticles(Vec2 position, int count, float directionX) {
	for (int i = 0; i < count; i++) {
		auto p = entityManager.AddEntity("Particle");

		// If directionX given, bias particles that way (skid kicks backwards)
		float biasX = (directionX != 0.0f) ? directionX * 1.5f : 0.0f;
		float vx = biasX + ((rand() % 200) - 100) / 100.0f;
		float vy = -((rand() % 150) + 30) / 100.0f;

		p->transform = std::make_shared<CTransform>(position, Vec2(vx, vy), 0.0f);
		p->sprite = std::make_shared<CSprite>(4.0f, 4.0f, sf::Color(200, 170, 120));
		p->particle = std::make_shared<CParticle>(18.0f, sf::Color(200, 170, 120));
	}
}

void Game::spawnGround() {
	auto ground = entityManager.AddEntity("Ground");

	const float w = static_cast<float>(WINDOW_WIDTH);
	const float cx = w * 0.5f;
	const float cy = static_cast<float>(WINDOW_HEIGHT) - GROUND_H * 0.5f;

	ground->transform = std::make_shared<CTransform>(Vec2(cx, cy), Vec2(0.0f, 0.0f), 0.0f);
	ground->boundingBox = std::make_shared<CBoundingBox>(w, GROUND_H);
	ground->sprite = std::make_shared<CSprite>(w, GROUND_H, sf::Color(100, 80, 60));
}

void Game::spawnPlayers() {
	const float groundTop = static_cast<float>(WINDOW_HEIGHT) - GROUND_H;
	const float spawnY = groundTop - PLAYER_H * 0.5f;

	// --- Player 1 (WASD) --- blue box, left side
	player1 = entityManager.AddEntity("Player");
	player1->transform = std::make_shared<CTransform>(P1_SPAWN, Vec2(0.0f, 0.0f), 0.0f);
	player1->boundingBox = std::make_shared<CBoundingBox>(PLAYER_W, PLAYER_H);
	player1->sprite = std::make_shared<CSprite>(PLAYER_W, PLAYER_H, sf::Color(50, 100, 200));
	player1->input = std::make_shared<CInput>();
	player1->health = std::make_shared<CHealth>();

	// --- Player 2 (Arrow keys) --- red box, right side
	player2 = entityManager.AddEntity("Player");
	player2->transform = std::make_shared<CTransform>(P2_SPAWN, Vec2(0.0f, 0.0f), 0.0f);
	player2->boundingBox = std::make_shared<CBoundingBox>(PLAYER_W, PLAYER_H);
	player2->sprite = std::make_shared<CSprite>(PLAYER_W, PLAYER_H, sf::Color(200, 50, 50));
	player2->input = std::make_shared<CInput>();
	player2->health = std::make_shared<CHealth>();
}

// Main loop

void Game::Run() {
	while (Running) {
		entityManager.Update();
		sUserInput();
		sGravity();
		sMovement();
		sHealth();
		sCollision();
		sCamera();
		sRender();
		currentFrame++;
	}
}

// Systems
void Game::sCamera() {

	// Only consider living players for camera calculation
	bool p1Alive = player1->transform && !player1->health->isDead;
	bool p2Alive = player2->transform && !player2->health->isDead;

	// Neither alive — freeze camera where it is
	if (!p1Alive && !p2Alive) { return; }

	float cx, cy, viewW, viewH;

	if (p1Alive && p2Alive) {
		// Both alive
		Vec2 p1 = player1->transform->position;
		Vec2 p2 = player2->transform->position;

		cx = (p1.x + p2.x) / 2.0f;
		cy = (p1.y + p2.y) / 2.0f;

		float dist = std::sqrt((p2.x - p1.x) * (p2.x - p1.x) +
			(p2.y - p1.y) * (p2.y - p1.y));
		float needed = std::max(static_cast<float>(WINDOW_WIDTH), dist * 1.6f);
		viewW = needed;
		viewH = needed * (static_cast<float>(WINDOW_HEIGHT) / WINDOW_WIDTH);

	}
	else {
		// One player dead 
		Vec2 pos = p1Alive ? player1->transform->position
			: player2->transform->position;
		cx = pos.x;
		cy = pos.y;
		viewW = static_cast<float>(WINDOW_WIDTH);
		viewH = static_cast<float>(WINDOW_HEIGHT);
	}

	// Smooth lerp so camera doesn't snap
	sf::Vector2f current = gameView.getCenter();
	sf::Vector2f target = { cx, cy };
	gameView.setCenter(current + (target - current) * 0.1f);

	sf::Vector2f currentSize = gameView.getSize();
	sf::Vector2f targetSize = { viewW, viewH };
	gameView.setSize(currentSize + (targetSize - currentSize) * 0.1f);

	window.setView(gameView);
}

void Game::sUserInput() {
	while (const std::optional<sf::Event> event = window.pollEvent()) {

		if (event->is<sf::Event::Closed>()) {
			Running = false;
		}

		if (const auto* kp = event->getIf<sf::Event::KeyPressed>()) {
			switch (kp->code) {
			case sf::Keyboard::Key::W: player1->input->jump = true; break;
			case sf::Keyboard::Key::A: player1->input->left = true; break;
			case sf::Keyboard::Key::D: player1->input->right = true; break;

			case sf::Keyboard::Key::Up: player2->input->jump = true; break;
			case sf::Keyboard::Key::Left:  player2->input->left = true; break;
			case sf::Keyboard::Key::Right: player2->input->right = true; break;

			default: break;
			}
		}

		if (const auto* kr = event->getIf<sf::Event::KeyReleased>()) {
			switch (kr->code) {
			case sf::Keyboard::Key::A: player1->input->left = false; break;
			case sf::Keyboard::Key::D: player1->input->right = false; break;
			case sf::Keyboard::Key::W: player1->input->jump = false; break;
			case sf::Keyboard::Key::Left:  player2->input->left = false; break;
			case sf::Keyboard::Key::Right: player2->input->right = false; break;
			case sf::Keyboard::Key::Up: player2->input->jump = false; break;

			default: break;
			}
		}
	}
}

void Game::sGravity() {
	for (auto& e : entityManager.GetEntities("Player")) {
		if (!e->transform) { continue; }

		auto& t = e->transform;

		if (t->coyoteFrames > 0) t->coyoteFrames--;

		bool wasOnGround = t->onGround;

		t->onGround = false;
		if (wasOnGround && !t->onGround && t->velocity.y >= 0.0f) {
			t->coyoteFrames = 8;  // 8 frames of window to still allow jump after leaving ground
		}

		t->velocity.y += GRAVITY;
		if (t->velocity.y > MAX_FALL_SPEED) {
			t->velocity.y = MAX_FALL_SPEED;
		}
	}
}

void Game::sMovement() {
	for (auto& e : entityManager.GetEntities("Player")) {
		if (!e->transform || !e->input) { continue; }

		auto& t = e->transform;
		auto& in = e->input;

		// --- Jump buffer ---
		if (in->jump) {
			t->JumpBufferFrames = 8;
			in->jump = false;
		}
		if (t->JumpBufferFrames > 0) { t->JumpBufferFrames--; }

		bool canJump = t->onGround || t->coyoteFrames > 0;
		if (t->JumpBufferFrames > 0 && canJump) {
			t->velocity.y = JUMP_VELOCITY;
			t->onGround = false;
			t->coyoteFrames = 0;
			t->JumpBufferFrames = 0;
			spawnDustParticles(
				Vec2(t->position.x, t->position.y + e->boundingBox->halfSize.y), 8
			);
		}

		// --- Horizontal acceleration ---
		bool pushingLeft = in->left && !in->right;
		bool pushingRight = in->right && !in->left;

		// Direction change — player is moving one way and input is the other
		bool turningLeft = pushingLeft && t->velocity.x > 0.5f;
		bool turningRight = pushingRight && t->velocity.x < -0.5f;
		bool turning = turningLeft || turningRight;

		if (turning) {
			// Skid: decelerate faster + spawn directional dust
			t->velocity.x *= TURN_FRICTION;

			// Dust only on first frame of turn (avoid spamming every frame)
			if (std::abs(t->velocity.x) > 1.5f) {
				Vec2 dustPos = Vec2(t->position.x, t->position.y + e->boundingBox->halfSize.y);
				// In the turning block — kick dust opposite to velocity (backwards from skid)
				float skidDir = (t->velocity.x > 0.0f) ? 1.0f : -1.0f; // same dir as movement
				spawnDustParticles(dustPos, 5, skidDir);
			}

		}
		else if (pushingLeft) {
			t->velocity.x -= ACCELERATION;

		}
		else if (pushingRight) {
			t->velocity.x += ACCELERATION;

		}
		else {
			// No input — apply friction
			t->velocity.x *= FRICTION;

			// Stop completely below threshold to prevent infinite sliding
			if (std::abs(t->velocity.x) < 0.15f) {
				t->velocity.x = 0.0f;
			}
		}

		// Cap horizontal speed
		t->velocity.x = std::clamp(t->velocity.x, -MAX_MOVE_SPEED, MAX_MOVE_SPEED);

		// Apply movement
		t->position.x += t->velocity.x;
		t->position.y += t->velocity.y;
	}
}
void Game::sCollision() {
	const float groundTop = static_cast<float>(WINDOW_HEIGHT) - GROUND_H;
	const float wf = static_cast<float>(WINDOW_WIDTH);

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
			if (!Tile->transform || !Tile->boundingBox) { continue; }
			const float tileHW = Tile->boundingBox->halfSize.x;
			const float tileHH = Tile->boundingBox->halfSize.y;
			const float tileX = Tile->transform->position.x;
			const float tileY = Tile->transform->position.y;

			if (std::abs(e->transform->position.x - tileX) < hw + tileHW &&
				std::abs(e->transform->position.y - tileY) < hh + tileHH) {
				// Simple collision push the player out of the tile
				float overlapX = (hw + tileHW) - std::abs(e->transform->position.x - tileX);
				float overlapY = (hh + tileHH) - std::abs(e->transform->position.y - tileY);
				if (overlapX < overlapY) {
					// horizontal collision
					if (e->transform->position.x < tileX) {
						e->transform->position.x -= overlapX;
					}
					else {
						e->transform->position.x += overlapX;
					}
					e->transform->velocity.x = 0.0f;
				}
				else {
					// vertical collision
					if (e->transform->position.y < tileY) {
						e->transform->position.y -= overlapY;

						// Only spawn dust if actually falling onto tile (not bonking head)
						if (!e->transform->onGround && e->transform->velocity.y > 1.0f) {
							spawnDustParticles(Vec2(e->transform->position.x,
								e->transform->position.y + e->boundingBox->halfSize.y), 6);
						}

						e->transform->onGround = true;
					}
					else {
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
						e->transform->position.y -= overlapY;\
							e->transform->onGround = true; // Player is on top of the other player
						e->transform->velocity -= Other->transform->velocity / 2;
					}
					else {
						Other->transform->position.y -= overlapY;
						Other->transform->onGround = true;// Other is on top of the s
						Other->transform->velocity -= e->transform->velocity / 2;
					}
				}
			}
		}
	}
}
void Game::sParticle() {
	for (auto& e : entityManager.GetEntities("Particle")) {
		if (!e->particle || !e->transform || !e->sprite) { continue; }

		auto& p = e->particle;
		auto& t = e->transform;

		p->age += 1.0f;

		if (p->age >= p->lifetime) {
			e->Destroy();
			continue;
		}

		// Light gravity — floatier than players
		t->velocity.y += 0.08f;
		t->position.x += t->velocity.x;
		t->position.y += t->velocity.y;

		// Fade out
		sf::Color c = p->color;
		c.a = static_cast<std::uint8_t>(p->alpha() * 255);
		e->sprite->getShape().setFillColor(c);
	}
}

void Game::sHealth() {
	for (auto& e : entityManager.GetEntities("Player")) {
		if (!e->health || !e->transform) { continue; }

		auto& h = e->health;
		auto& t = e->transform;


		if (!h->isDead && t->position.y > WINDOW_HEIGHT + 100.0f) {
			KillPlayer(e);
		}

		// respawn timer tick
		if (h->isRespawning()) {
			h->respawnTimer--;

			if (h->respawnTimer <= 0) {
				RespawnPlayer(e);
			}
		}

		// check lives all lost
		if (!h->isDead && h->lives <= 0) {
			//yet remains
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
	e->SaveBoundingbox(); // Save current bounding box before disabling
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
	p1Text.setCharacterSize(24);
	p2Text.setCharacterSize(24);

	p1Text.setFillColor(sf::Color::Cyan);
	p2Text.setFillColor(sf::Color::White);

	// Build lives string: "P1: ♥ ♥ ♥"
	std::string p1Lives = "P1: ";
	for (int i = 0; i < player1->health->lives; i++) { p1Lives += "<3 "; }
	if (player1->health->isRespawning()) {
		p1Lives += " (respawning " + std::to_string(player1->health->respawnTimer / 60 + 1) + "s)";
	}

	std::string p2Lives = "P2: ";
	for (int i = 0; i < player2->health->lives; i++) { p2Lives += "<3 "; }

	p1Text.setString(p1Lives);
	p2Text.setString(p2Lives);

	p1Text.setPosition({ 20.f,  20.f });
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
	sParticle(); // Update and render particles after all entities so they appear on top

	window.setView(window.getDefaultView());
	RenderHud();
	window.display();
}

void Game::sMovingPlatform() {
	for (auto& e : entityManager.GetEntities()) {
		if (!e->movingPlatform || !e->transform || !e->boundingBox) { continue; }

		Vec2 target = e->movingPlatform->triggered ? e->movingPlatform->posB : e->movingPlatform->posA;
		Vec2 current = e->transform->position;

		Vec2 direction = target - current;

		float distSq = (direction.x * direction.x) + (direction.y * direction.y);
		float speedSq = e->movingPlatform->speed * e->movingPlatform->speed;

		Vec2 moveAmount(0.0f, 0.0f);

		if (distSq > speedSq) {
			Vec2 normDir = direction.Normalize();
			moveAmount = normDir * e->movingPlatform->speed;
			e->transform->position += moveAmount;
		}
		else {
			moveAmount = target - current; // Exact distance left to snap
			e->transform->position = target;
		}

		for (auto& player : entityManager.GetEntities("Player")) {
			if (!player->transform || !player->boundingBox) { continue; }
			float platTop = current.y - e->boundingBox->halfSize.y;
			float platLeft = current.x - e->boundingBox->halfSize.x;
			float platRight = current.x + e->boundingBox->halfSize.x;

			float playerBottom = player->transform->position.y + player->boundingBox->halfSize.y;
			float playerX = player->transform->position.x;

			if (std::abs(playerBottom - platTop) < 3.0f && playerX > platLeft && playerX < platRight) {
				// Apply the exact same movement to the player
				player->transform->position += moveAmount;
			}
		}
	}
}