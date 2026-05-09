#include "Game.h"
#include <optional>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <cstdint>
#include <cmath>

// ============================================================================
// Construction / Initialization
// ============================================================================

Game::Game() {
	init();
}

void Game::init() {
	loadConfig("Config.txt");

	if (font.loadFromFile("Fonts/Coolvetica Rg.otf")) {
		std::cout << "Font loaded successfully." << std::endl;
	} else {
		std::cerr << "Failed to load font." << std::endl;
	}
	window.create(sf::VideoMode({ WINDOW_WIDTH, WINDOW_HEIGHT }), "2D Co-op Puzzle Platformer");
	window.setFramerateLimit(60);
	spawnPlayers();
	entityManager.Update();
	gameView = window.getDefaultView();
}

// ============================================================================
// Config / Level Loading
// ============================================================================

void Game::loadConfig(const std::string& path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		std::cerr << "Could not open config: " << path << std::endl;
		return;
	}
	std::string type;
	while (file >> type) {
		if (type[0] == '#') {
			// Comment line — consume the rest
			std::string dummy;
			std::getline(file, dummy);
			continue;
		}

		if (type == "Tile") {
			float x, y, w, h;
			file >> x >> y >> w >> h;
			auto tile = entityManager.AddEntity("Tile");
			tile->transform   = std::make_shared<CTransform>(Vec2(x, y), Vec2(0, 0), 0.0f);
			tile->boundingBox = std::make_shared<CBoundingBox>(w, h);
			tile->sprite      = std::make_shared<CSprite>(w, h, sf::Color(100, 80, 60));

		} else if (type == "Spawn") {
			float p1x, p1y, p2x, p2y;
			file >> p1x >> p1y >> p2x >> p2y;
			P1_SPAWN = Vec2(p1x, p1y);
			P2_SPAWN = Vec2(p2x, p2y);

		} else if (type == "Enemy") {
			float x, y, wp1x, wp1y, wp2x, wp2y, speed, sightRange, sightAngle;
			file >> x >> y >> wp1x >> wp1y >> wp2x >> wp2y >> speed >> sightRange >> sightAngle;

			auto enemy = entityManager.AddEntity("Enemy");
			enemy->transform   = std::make_shared<CTransform>(Vec2(x, y), Vec2(0, 0), 0.0f);
			enemy->boundingBox = std::make_shared<CBoundingBox>(40.0f, 48.0f);
			enemy->sprite      = std::make_shared<CSprite>(40.0f, 48.0f, sf::Color(220, 120, 0));

			auto patrol = std::make_shared<CPatrol>();
			patrol->waypoints.push_back(Vec2(wp1x, wp1y));
			patrol->waypoints.push_back(Vec2(wp2x, wp2y));
			patrol->speed = speed;
			enemy->patrol = patrol;

			auto sight = std::make_shared<CSight>();
			sight->range        = sightRange;
			sight->halfAngleDeg = sightAngle;
			enemy->sight = sight;

		} else if (type == "Button") {
			float x, y, w, h;
			std::string linkedTag;
			int requiresStay;
			file >> x >> y >> w >> h >> linkedTag >> requiresStay;

			auto button = entityManager.AddEntity("Button");
			button->transform    = std::make_shared<CTransform>(Vec2(x, y), Vec2(0, 0), 0.0f);
			button->boundingBox  = std::make_shared<CBoundingBox>(w, h);
			button->sprite       = std::make_shared<CSprite>(w, h, sf::Color(200, 100, 0));

			auto inter = std::make_shared<CInteractable>();
			inter->linkedTag    = linkedTag;
			inter->requiresStay = (requiresStay != 0);
			button->interactable = inter;

		} else if (type == "Door") {
			std::string tag;
			float x, y, w, h, openX, openY;
			file >> tag >> x >> y >> w >> h >> openX >> openY;

			auto door = entityManager.AddEntity(tag);
			door->transform   = std::make_shared<CTransform>(Vec2(x, y), Vec2(0, 0), 0.0f);
			door->boundingBox = std::make_shared<CBoundingBox>(w, h);
			door->sprite      = std::make_shared<CSprite>(w, h, sf::Color(80, 60, 40));

			auto d = std::make_shared<CDoor>();
			d->closedPos     = Vec2(x, y);
			d->openPos       = Vec2(openX, openY);
			d->savedHalfSize = Vec2(w * 0.5f, h * 0.5f);
			door->door = d;

		} else if (type == "Platform") {
			std::string tag;
			float x, y, w, h, targetX, targetY, speed;
			std::string linkedTag;
			file >> tag >> x >> y >> w >> h >> targetX >> targetY >> speed >> linkedTag;

			auto plat = entityManager.AddEntity(tag);
			plat->transform   = std::make_shared<CTransform>(Vec2(x, y), Vec2(0, 0), 0.0f);
			plat->boundingBox = std::make_shared<CBoundingBox>(w, h);
			plat->sprite      = std::make_shared<CSprite>(w, h, sf::Color(60, 100, 160));

			auto mp = std::make_shared<CMovingPlatform>();
			mp->posA  = Vec2(x, y);
			mp->posB  = Vec2(targetX, targetY);
			mp->speed = speed;
			plat->movingPlatform = mp;

		} else if (type == "Checkpoint") {
			float x, y, p1sx, p1sy, p2sx, p2sy;
			file >> x >> y >> p1sx >> p1sy >> p2sx >> p2sy;

			auto cp = entityManager.AddEntity("Checkpoint");
			cp->transform   = std::make_shared<CTransform>(Vec2(x, y), Vec2(0, 0), 0.0f);
			cp->boundingBox = std::make_shared<CBoundingBox>(32.0f, 32.0f);
			cp->sprite      = std::make_shared<CSprite>(32.0f, 32.0f, sf::Color(0, 180, 255));

			auto c = std::make_shared<CCheckpoint>();
			c->p1Spawn = Vec2(p1sx, p1sy);
			c->p2Spawn = Vec2(p2sx, p2sy);
			cp->checkpoint = c;

		} else if (type == "Exit") {
			float x, y, w, h;
			file >> x >> y >> w >> h;

			auto exit = entityManager.AddEntity("Exit");
			exit->transform   = std::make_shared<CTransform>(Vec2(x, y), Vec2(0, 0), 0.0f);
			exit->boundingBox = std::make_shared<CBoundingBox>(w, h);
			exit->sprite      = std::make_shared<CSprite>(w, h, sf::Color(255, 220, 0));
			exit->exit_       = std::make_shared<CExit>();

		} else {
			// Unknown token — skip line
			std::string dummy;
			std::getline(file, dummy);
		}
	}
}

// ============================================================================
// Spawners
// ============================================================================

void Game::spawnDustParticles(Vec2 position, int count, float directionX) {
	for (int i = 0; i < count; i++) {
		auto p = entityManager.AddEntity("Particle");

		float biasX = (directionX != 0.0f) ? directionX * 1.5f : 0.0f;
		float vx = biasX + ((rand() % 200) - 100) / 100.0f;
		float vy = -((rand() % 150) + 30) / 100.0f;

		p->transform = std::make_shared<CTransform>(position, Vec2(vx, vy), 0.0f);
		p->sprite    = std::make_shared<CSprite>(4.0f, 4.0f, sf::Color(200, 170, 120));
		p->particle  = std::make_shared<CParticle>(18.0f, sf::Color(200, 170, 120));
	}
}

void Game::spawnPlayers() {
	// --- Player 1 (WASD) --- blue box
	player1 = entityManager.AddEntity("Player");
	player1->transform   = std::make_shared<CTransform>(P1_SPAWN, Vec2(0.0f, 0.0f), 0.0f);
	player1->boundingBox = std::make_shared<CBoundingBox>(PLAYER_W, PLAYER_H);
	player1->sprite      = std::make_shared<CSprite>(PLAYER_W, PLAYER_H, sf::Color(50, 100, 200));
	player1->input       = std::make_shared<CInput>();
	player1->health      = std::make_shared<CHealth>(P1_SPAWN);

	// --- Player 2 (Arrow keys) --- red box
	player2 = entityManager.AddEntity("Player");
	player2->transform   = std::make_shared<CTransform>(P2_SPAWN, Vec2(0.0f, 0.0f), 0.0f);
	player2->boundingBox = std::make_shared<CBoundingBox>(PLAYER_W, PLAYER_H);
	player2->sprite      = std::make_shared<CSprite>(PLAYER_W, PLAYER_H, sf::Color(200, 50, 50));
	player2->input       = std::make_shared<CInput>();
	player2->health      = std::make_shared<CHealth>(P2_SPAWN);
}

// ============================================================================
// Main Loop
// ============================================================================

void Game::Run() {
	while (Running) {
		entityManager.Update();
		sUserInput();

		if (state == GameState::PLAYING || state == GameState::RESET_FLASH) {
			sGravity();
			sMovement();
			sPatrol();
			sSight();
			sInteract();
			sMovingPlatform();
			sCheckpoint();
			sCollision();
			sWinCondition();
			sParticle();
			sCamera();
		}

		sRender();
		currentFrame++;
	}
}

// ============================================================================
// Systems
// ============================================================================

void Game::sCamera() {
	if (!player1->transform || !player2->transform) { return; }

	Vec2 p1 = player1->transform->position;
	Vec2 p2 = player2->transform->position;

	float cx = (p1.x + p2.x) * 0.5f;
	float cy = (p1.y + p2.y) * 0.5f;

	float dist   = p1.Dist(p2);
	float needed = std::max(static_cast<float>(WINDOW_WIDTH), dist * 1.6f);
	float viewW  = needed;
	float viewH  = needed * (static_cast<float>(WINDOW_HEIGHT) / static_cast<float>(WINDOW_WIDTH));

	sf::Vector2f current    = gameView.getCenter();
	sf::Vector2f target     = { cx, cy };
	gameView.setCenter(current + (target - current) * 0.1f);

	sf::Vector2f currentSize = gameView.getSize();
	sf::Vector2f targetSize  = { viewW, viewH };
	gameView.setSize(currentSize + (targetSize - currentSize) * 0.1f);

	window.setView(gameView);
}

void Game::sUserInput() {
	sf::Event event;
	while (window.pollEvent(event)) {
		if (event.type == sf::Event::Closed) {
			Running = false;
		}

		if (event.type == sf::Event::KeyPressed) {
			switch (event.key.code) {
			case sf::Keyboard::W:     player1->input->jump  = true; break;
			case sf::Keyboard::A:     player1->input->left  = true; break;
			case sf::Keyboard::D:     player1->input->right = true; break;

			case sf::Keyboard::Up:    player2->input->jump  = true; break;
			case sf::Keyboard::Left:  player2->input->left  = true; break;
			case sf::Keyboard::Right: player2->input->right = true; break;

			default: break;
			}
		}

		if (event.type == sf::Event::KeyReleased) {
			switch (event.key.code) {
			case sf::Keyboard::A:     player1->input->left  = false; break;
			case sf::Keyboard::D:     player1->input->right = false; break;
			case sf::Keyboard::W:     player1->input->jump  = false; break;

			case sf::Keyboard::Left:  player2->input->left  = false; break;
			case sf::Keyboard::Right: player2->input->right = false; break;
			case sf::Keyboard::Up:    player2->input->jump  = false; break;

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
			t->coyoteFrames = 8;
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

		auto& t  = e->transform;
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
			t->onGround   = false;
			t->coyoteFrames      = 0;
			t->JumpBufferFrames  = 0;
			spawnDustParticles(
				Vec2(t->position.x, t->position.y + e->boundingBox->halfSize.y), 8
			);
		}

		// --- Horizontal acceleration ---
		bool pushingLeft  = in->left  && !in->right;
		bool pushingRight = in->right && !in->left;

		bool turningLeft  = pushingLeft  && t->velocity.x >  0.5f;
		bool turningRight = pushingRight && t->velocity.x < -0.5f;
		bool turning = turningLeft || turningRight;

		if (turning) {
			t->velocity.x *= TURN_FRICTION;
			if (std::abs(t->velocity.x) > 1.5f) {
				Vec2 dustPos = Vec2(t->position.x, t->position.y + e->boundingBox->halfSize.y);
				float skidDir = (t->velocity.x > 0.0f) ? 1.0f : -1.0f;
				spawnDustParticles(dustPos, 5, skidDir);
			}
		} else if (pushingLeft) {
			t->velocity.x -= ACCELERATION;
		} else if (pushingRight) {
			t->velocity.x += ACCELERATION;
		} else {
			t->velocity.x *= FRICTION;
			if (std::abs(t->velocity.x) < 0.15f) { t->velocity.x = 0.0f; }
		}

		t->velocity.x = std::clamp(t->velocity.x, -MAX_MOVE_SPEED, MAX_MOVE_SPEED);

		t->position.x += t->velocity.x;
		t->position.y += t->velocity.y;
	}
}

void Game::sPatrol() {
	for (auto& e : entityManager.GetEntities("Enemy")) {
		if (!e->patrol || !e->transform) { continue; }

		auto& patrol = e->patrol;
		auto& t      = e->transform;

		if (patrol->waypoints.empty()) { continue; }

		Vec2  target  = patrol->waypoints[patrol->currentTarget];
		float dist    = t->position.Dist(target);

		if (dist < 4.0f) {
			// Advance to next waypoint (loop)
			patrol->currentTarget = (patrol->currentTarget + 1) % static_cast<int>(patrol->waypoints.size());
		} else {
			Vec2 dir = (target - t->position).Normalize();
			t->position += dir * patrol->speed;
			patrol->facingRight = (dir.x > 0.0f);
		}
	}
}

void Game::sSight() {
	static constexpr float PI = 3.14159265f;

	for (auto& enemy : entityManager.GetEntities("Enemy")) {
		if (!enemy->sight || !enemy->transform) { continue; }

		auto& sight = enemy->sight;
		Vec2  enemyPos = enemy->transform->position;

		// Determine facing direction (use patrol if available)
		bool facingRight = true;
		if (enemy->patrol) { facingRight = enemy->patrol->facingRight; }
		Vec2 facing = facingRight ? Vec2(1.0f, 0.0f) : Vec2(-1.0f, 0.0f);

		float cosHalf = std::cos(sight->halfAngleDeg * PI / 180.0f);

		for (auto& player : entityManager.GetEntities("Player")) {
			if (!player->transform) { continue; }

			Vec2  toPlayer = player->transform->position - enemyPos;
			float dist     = enemyPos.Dist(player->transform->position);

			if (dist < sight->range && dist > 0.0f) {
				Vec2  dir = toPlayer.Normalize();
				float dot = facing.x * dir.x + facing.y * dir.y;
				if (dot > cosHalf) {
					ResetToCheckpoint();
					return; // reset already applied — stop processing
				}
			}
		}
	}
}

void Game::sInteract() {
	for (auto& button : entityManager.GetEntities("Button")) {
		if (!button->interactable || !button->boundingBox || !button->transform) { continue; }

		auto& inter = button->interactable;

		// Check if either player overlaps the button
		bool anyOverlap = false;
		for (auto& player : entityManager.GetEntities("Player")) {
			if (!player->transform || !player->boundingBox) { continue; }
			float dx = std::abs(player->transform->position.x - button->transform->position.x);
			float dy = std::abs(player->transform->position.y - button->transform->position.y);
			if (dx < player->boundingBox->halfSize.x + button->boundingBox->halfSize.x &&
				dy < player->boundingBox->halfSize.y + button->boundingBox->halfSize.y) {
				anyOverlap = true;
				break;
			}
		}

		bool wasPressed = inter->isPressed;

		if (anyOverlap && !inter->isPressed) {
			inter->isPressed = true;
		} else if (!anyOverlap && inter->requiresStay && inter->isPressed) {
			inter->isPressed = false;
		}

		// Only trigger/untrigger linked entities when state changes
		if (inter->isPressed != wasPressed) {
			for (auto& ent : entityManager.GetEntities()) {
				if (ent->GetTag() == inter->linkedTag) {
					if (ent->door)           { ent->door->isOpen           = inter->isPressed; }
					if (ent->movingPlatform) { ent->movingPlatform->triggered = inter->isPressed; }
				}
			}
		}

		// Update button colour to reflect state
		if (button->sprite) {
			button->sprite->getShape().setFillColor(
				inter->isPressed ? sf::Color(0, 200, 100) : sf::Color(200, 100, 0)
			);
		}
	}
}

void Game::sMovingPlatform() {
	// --- Moving platforms ---
	for (auto& ent : entityManager.GetEntities()) {
		if (!ent->movingPlatform || !ent->transform) { continue; }

		auto& mp = ent->movingPlatform;
		auto& t  = ent->transform;

		Vec2 target = mp->triggered ? mp->posB : mp->posA;
		Vec2 prevPos = t->position;
		Vec2 delta   = target - t->position;
		float distSq = delta.x * delta.x + delta.y * delta.y;

		if (distSq > mp->speed * mp->speed) {
			t->position += delta.Normalize() * mp->speed;
		} else {
			t->position = target;
		}

		Vec2 moved = t->position - prevPos;

		// Carry players standing on top of this platform
		if (!ent->boundingBox) { continue; }
		for (auto& player : entityManager.GetEntities("Player")) {
			if (!player->transform || !player->boundingBox) { continue; }

			float playerBottom = player->transform->position.y + player->boundingBox->halfSize.y;
			float platTop      = prevPos.y - ent->boundingBox->halfSize.y;
			float playerX      = player->transform->position.x;
			float platLeft     = prevPos.x - ent->boundingBox->halfSize.x;
			float platRight    = prevPos.x + ent->boundingBox->halfSize.x;

			if (std::abs(playerBottom - platTop) < 4.0f &&
				playerX > platLeft && playerX < platRight) {
				player->transform->position += moved;
			}
		}
	}

	// --- Doors: lerp between open/closed positions ---
	for (auto& ent : entityManager.GetEntities()) {
		if (!ent->door || !ent->transform) { continue; }

		auto& d = ent->door;
		auto& t = ent->transform;

		Vec2 target  = d->isOpen ? d->openPos : d->closedPos;
		Vec2 delta   = target - t->position;
		float distSq = delta.x * delta.x + delta.y * delta.y;

		if (distSq > d->speed * d->speed) {
			t->position += delta.Normalize() * d->speed;
		} else {
			t->position = target;
		}

		// Disable/restore bounding box based on whether the door is fully open
		bool fullyOpen = (t->position.Dist(d->openPos) < 1.0f && d->isOpen);
		if (fullyOpen) {
			ent->boundingBox = nullptr;
		} else if (!ent->boundingBox) {
			ent->boundingBox = std::make_shared<CBoundingBox>(d->savedHalfSize);
		}
	}
}

void Game::sCheckpoint() {
	for (auto& cp : entityManager.GetEntities("Checkpoint")) {
		if (!cp->checkpoint || !cp->boundingBox || !cp->transform) { continue; }

		auto& c = cp->checkpoint;
		if (c->activated) { continue; } // already active, skip

		bool anyOverlap = false;
		for (auto& player : entityManager.GetEntities("Player")) {
			if (!player->transform || !player->boundingBox) { continue; }
			float dx = std::abs(player->transform->position.x - cp->transform->position.x);
			float dy = std::abs(player->transform->position.y - cp->transform->position.y);
			if (dx < player->boundingBox->halfSize.x + cp->boundingBox->halfSize.x &&
				dy < player->boundingBox->halfSize.y + cp->boundingBox->halfSize.y) {
				anyOverlap = true;
				break;
			}
		}

		if (anyOverlap) {
			// Deactivate all other checkpoints
			for (auto& other : entityManager.GetEntities("Checkpoint")) {
				if (other->checkpoint) {
					other->checkpoint->activated = false;
					if (other->sprite) {
						other->sprite->getShape().setFillColor(sf::Color(0, 180, 255));
					}
				}
			}
			c->activated = true;
			c->p1Spawn   = player1->transform->position;
			c->p2Spawn   = player2->transform->position;
			if (cp->sprite) {
				cp->sprite->getShape().setFillColor(sf::Color(0, 255, 120));
			}
		}
	}
}

void Game::sCollision() {
	const float wf = static_cast<float>(WINDOW_WIDTH);

	for (auto& e : entityManager.GetEntities("Player")) {
		if (!e->transform || !e->boundingBox) { continue; }

		const float hw = e->boundingBox->halfSize.x;
		const float hh = e->boundingBox->halfSize.y;

		// Fell into a pit — reset both players
		if (e->transform->position.y > static_cast<float>(WINDOW_HEIGHT) + 100.0f) {
			ResetToCheckpoint();
			return;
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

		// Tiles
		for (auto& tile : entityManager.GetEntities("Tile")) {
			if (!tile->transform || !tile->boundingBox) { continue; }
			const float tileHW = tile->boundingBox->halfSize.x;
			const float tileHH = tile->boundingBox->halfSize.y;
			const float tileX  = tile->transform->position.x;
			const float tileY  = tile->transform->position.y;

			if (std::abs(e->transform->position.x - tileX) < hw + tileHW &&
				std::abs(e->transform->position.y - tileY) < hh + tileHH) {

				float overlapX = (hw + tileHW) - std::abs(e->transform->position.x - tileX);
				float overlapY = (hh + tileHH) - std::abs(e->transform->position.y - tileY);

				if (overlapX < overlapY) {
					if (e->transform->position.x < tileX) {
						e->transform->position.x -= overlapX;
					} else {
						e->transform->position.x += overlapX;
					}
					e->transform->velocity.x = 0.0f;
				} else {
					if (e->transform->position.y < tileY) {
						e->transform->position.y -= overlapY;
						if (!e->transform->onGround && e->transform->velocity.y > 1.0f) {
							spawnDustParticles(Vec2(e->transform->position.x,
								e->transform->position.y + e->boundingBox->halfSize.y), 6);
						}
						e->transform->onGround = true;
					} else {
						e->transform->position.y += overlapY;
					}
					e->transform->velocity.y = 0.0f;
				}
			}
		}

		// Moving platform collision (entities with CMovingPlatform act as solid tiles)
		for (auto& plat : entityManager.GetEntities()) {
			if (!plat->movingPlatform || !plat->transform || !plat->boundingBox) { continue; }
			const float platHW = plat->boundingBox->halfSize.x;
			const float platHH = plat->boundingBox->halfSize.y;
			const float platX  = plat->transform->position.x;
			const float platY  = plat->transform->position.y;

			if (std::abs(e->transform->position.x - platX) < hw + platHW &&
				std::abs(e->transform->position.y - platY) < hh + platHH) {

				float overlapX = (hw + platHW) - std::abs(e->transform->position.x - platX);
				float overlapY = (hh + platHH) - std::abs(e->transform->position.y - platY);

				if (overlapX < overlapY) {
					if (e->transform->position.x < platX) {
						e->transform->position.x -= overlapX;
					} else {
						e->transform->position.x += overlapX;
					}
					e->transform->velocity.x = 0.0f;
				} else {
					if (e->transform->position.y < platY) {
						e->transform->position.y -= overlapY;
						e->transform->onGround    = true;
					} else {
						e->transform->position.y += overlapY;
					}
					e->transform->velocity.y = 0.0f;
				}
			}
		}

		// Door collision (entities with CDoor and active bounding box act as solid tiles)
		for (auto& door : entityManager.GetEntities()) {
			if (!door->door || !door->transform || !door->boundingBox) { continue; }
			const float dHW = door->boundingBox->halfSize.x;
			const float dHH = door->boundingBox->halfSize.y;
			const float dX  = door->transform->position.x;
			const float dY  = door->transform->position.y;

			if (std::abs(e->transform->position.x - dX) < hw + dHW &&
				std::abs(e->transform->position.y - dY) < hh + dHH) {

				float overlapX = (hw + dHW) - std::abs(e->transform->position.x - dX);
				float overlapY = (hh + dHH) - std::abs(e->transform->position.y - dY);

				if (overlapX < overlapY) {
					if (e->transform->position.x < dX) {
						e->transform->position.x -= overlapX;
					} else {
						e->transform->position.x += overlapX;
					}
					e->transform->velocity.x = 0.0f;
				} else {
					if (e->transform->position.y < dY) {
						e->transform->position.y -= overlapY;
						e->transform->onGround    = true;
					} else {
						e->transform->position.y += overlapY;
					}
					e->transform->velocity.y = 0.0f;
				}
			}
		}

		// Player-player collision — only resolve when e has lower ID to avoid double application
		for (auto& other : entityManager.GetEntities("Player")) {
			if (other->GetID() <= e->GetID()) { continue; } // process each pair once
			if (!other->transform || !other->boundingBox) { continue; }

			const float otherHW = other->boundingBox->halfSize.x;
			const float otherHH = other->boundingBox->halfSize.y;
			const float otherX  = other->transform->position.x;
			const float otherY  = other->transform->position.y;

			if (std::abs(e->transform->position.x - otherX) < hw + otherHW &&
				std::abs(e->transform->position.y - otherY) < hh + otherHH) {

				float overlapX = (hw + otherHW) - std::abs(e->transform->position.x - otherX);
				float overlapY = (hh + otherHH) - std::abs(e->transform->position.y - otherY);

				if (overlapX < overlapY) {
					if (e->transform->position.x < otherX) {
						e->transform->position.x -= overlapX * 0.5f;
						other->transform->position.x += overlapX * 0.5f;
					} else {
						e->transform->position.x += overlapX * 0.5f;
						other->transform->position.x -= overlapX * 0.5f;
					}
					e->transform->velocity.x     = 0.0f;
					other->transform->velocity.x = 0.0f;
				} else {
					if (e->transform->position.y < otherY) {
						e->transform->position.y -= overlapY;
						e->transform->onGround = true;
						e->transform->velocity -= other->transform->velocity * 0.5f;
					} else {
						other->transform->position.y -= overlapY;
						other->transform->onGround = true;
						other->transform->velocity -= e->transform->velocity * 0.5f;
					}
				}
			}
		}
	}
}

void Game::sWinCondition() {
	if (state == GameState::WIN) { return; }

	for (auto& exit : entityManager.GetEntities("Exit")) {
		if (!exit->exit_ || !exit->boundingBox || !exit->transform) { continue; }

		exit->exit_->playersInside = 0;
		for (auto& player : entityManager.GetEntities("Player")) {
			if (!player->transform || !player->boundingBox) { continue; }
			float dx = std::abs(player->transform->position.x - exit->transform->position.x);
			float dy = std::abs(player->transform->position.y - exit->transform->position.y);
			if (dx < player->boundingBox->halfSize.x + exit->boundingBox->halfSize.x &&
				dy < player->boundingBox->halfSize.y + exit->boundingBox->halfSize.y) {
				exit->exit_->playersInside++;
			}
		}

		if (exit->exit_->playersInside >= 2) {
			state = GameState::WIN;
		}
	}
}

void Game::ResetToCheckpoint() {
	if (state == GameState::RESET_FLASH) { return; } // already in reset, ignore

	Vec2 p1Target = P1_SPAWN;
	Vec2 p2Target = P2_SPAWN;

	for (auto& cp : entityManager.GetEntities("Checkpoint")) {
		if (cp->checkpoint && cp->checkpoint->activated) {
			p1Target = cp->checkpoint->p1Spawn;
			p2Target = cp->checkpoint->p2Spawn;
			break;
		}
	}

	player1->transform->position = p1Target;
	player1->transform->velocity = { 0.0f, 0.0f };
	player2->transform->position = p2Target;
	player2->transform->velocity = { 0.0f, 0.0f };

	// Re-enable bounding boxes in case they were cleared
	if (!player1->boundingBox) {
		player1->boundingBox = std::make_shared<CBoundingBox>(PLAYER_W, PLAYER_H);
	}
	if (!player2->boundingBox) {
		player2->boundingBox = std::make_shared<CBoundingBox>(PLAYER_W, PLAYER_H);
	}

	resetFlashTimer = 12;
	state = GameState::RESET_FLASH;
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

		t->velocity.y += 0.08f;
		t->position.x += t->velocity.x;
		t->position.y += t->velocity.y;

		sf::Color c = p->color;
		c.a = static_cast<std::uint8_t>(p->alpha() * 255);
		e->sprite->getShape().setFillColor(c);
	}
}

void Game::RenderHud() {
	// Win screen
	if (state == GameState::WIN) {
		sf::Text winText;
		winText.setFont(font);
		winText.setCharacterSize(48);
		winText.setFillColor(sf::Color::Yellow);
		winText.setString("YOU WIN!  Press Escape to quit.");
		winText.setPosition({ 20.f, static_cast<float>(WINDOW_HEIGHT) * 0.5f - 24.f });
		window.draw(winText);
		return;
	}

	// Checkpoint indicator
	bool cpActive = false;
	for (auto& cp : entityManager.GetEntities("Checkpoint")) {
		if (cp->checkpoint && cp->checkpoint->activated) { cpActive = true; break; }
	}

	sf::Text p1Text, p2Text, cpText;
	p1Text.setFont(font);
	p2Text.setFont(font);
	cpText.setFont(font);
	p1Text.setCharacterSize(22);
	p2Text.setCharacterSize(22);
	cpText.setCharacterSize(18);

	p1Text.setFillColor(sf::Color::Cyan);
	p2Text.setFillColor(sf::Color(255, 100, 100));
	cpText.setFillColor(sf::Color(0, 255, 120));

	p1Text.setString("P1: WASD / W to jump");
	p2Text.setString("P2: Arrows / Up to jump");
	cpText.setString(cpActive ? "[ Checkpoint Active ]" : "[ No Checkpoint ]");

	p1Text.setPosition({ 20.f,  20.f });
	p2Text.setPosition({ 20.f,  48.f });
	cpText.setPosition({ 20.f,  76.f });

	window.draw(p1Text);
	window.draw(p2Text);
	window.draw(cpText);
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

	// Red flash overlay after detection/pit reset
	if (resetFlashTimer > 0) {
		sf::RectangleShape overlay(
			{ static_cast<float>(WINDOW_WIDTH) * 4.0f, static_cast<float>(WINDOW_HEIGHT) * 4.0f }
		);
		// Position relative to camera centre so it covers the whole viewport
		sf::Vector2f cam = gameView.getCenter();
		sf::Vector2f sz  = gameView.getSize();
		overlay.setPosition({ cam.x - sz.x * 0.5f, cam.y - sz.y * 0.5f });
		overlay.setFillColor(sf::Color(255, 0, 0, 120));
		window.draw(overlay);
		resetFlashTimer--;
		if (resetFlashTimer <= 0) {
			state = GameState::PLAYING;
		}
	}

	window.setView(window.getDefaultView());
	RenderHud();
	window.display();
}