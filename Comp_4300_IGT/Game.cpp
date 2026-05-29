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

		if (type == "Tile") {
			float x, y, w, h;
			std::string texKey;

			iss >> x >> y >> w >> h >> texKey;

			auto tile = entityManager.AddEntity("Tile");
			tile->transform = std::make_shared<CTransform>(Vec2(x, y), Vec2(0, 0), 0.0f);
			tile->boundingBox = std::make_shared<CBoundingBox>(w, h);
			tile->sprite = std::make_shared<CSprite>(w, h, sf::Color::White);

			sf::Texture& tex = getTexture(texKey);
			tile->sprite->getShape().setTexture(&tex);

			if (w > tex.getSize().x || h > tex.getSize().y) {
				tex.setRepeated(true);
				tile->sprite->getShape().setTextureRect(sf::IntRect({ 0, 0 }, { (int)w, (int)h }));
			}
		}
		else if (type == "Spawn") {
			float p1x, p1y, p2x, p2y;
			iss >> p1x >> p1y >> p2x >> p2y;
			P1_SPAWN = Vec2(p1x, p1y);
			P2_SPAWN = Vec2(p2x, p2y);
		}
		else if (type == "Enemy") {
			float x, y, wp1x, wp1y, wp2x, wp2y, speed, sightRange, sightAngle;
			iss >> x >> y >> wp1x >> wp1y >> wp2x >> wp2y >> speed >> sightRange >> sightAngle;

	Text = std::make_unique<sf::Text>(font, "Default", 24);
	Text->setFillColor(sf::Color::White);

	spawnPlayer();

			auto sight = std::make_shared<CSight>();
			sight->range = sightRange;
			sight->halfAngleDeg = sightAngle;
			enemy->sight = sight;
		}
		else if (type == "Button") {
			float x, y, w, h;
			std::string linkedTag;
			int requiresStay = 0, requiresInput = 0;

			iss >> x >> y >> w >> h >> linkedTag >> requiresStay >> requiresInput;

			auto button = entityManager.AddEntity("Button");
			button->transform = std::make_shared<CTransform>(Vec2(x, y), Vec2(0, 0), 0.0f);
			button->boundingBox = std::make_shared<CBoundingBox>(w, h);
			button->sprite = std::make_shared<CSprite>(w, h, sf::Color(0, 0, 255));

			auto inter = std::make_shared<CInteractable>();
			inter->linkedTag = linkedTag;
			inter->requiresStay = (requiresStay != 0);
			inter->requiresInput = (requiresInput != 0);
			button->interactable = inter;
		}
		else if (type == "Door") {
			std::string tag;
			float x, y, w, h, openX, openY;
			iss >> tag >> x >> y >> w >> h >> openX >> openY;

			auto door = entityManager.AddEntity("Door");
			door->transform = std::make_shared<CTransform>(Vec2(x, y), Vec2(0, 0), 0.0f);
			door->boundingBox = std::make_shared<CBoundingBox>(w, h);
			door->sprite = std::make_shared<CSprite>(w, h, sf::Color(80, 60, 40));

			auto d = std::make_shared<CDoor>();
			d->linkTag = tag;
			door->door = d;
		}
		else if (type == "Platform") {
			std::string tag;
			float x, y, w, h, targetX, targetY, speed;
			std::string linkedTag;
			int reqTriggers = 1;

			iss >> tag >> x >> y >> w >> h >> targetX >> targetY >> speed >> linkedTag >> reqTriggers;

			auto plat = entityManager.AddEntity(tag);
			plat->transform = std::make_shared<CTransform>(Vec2(x, y), Vec2(0, 0), 0.0f);
			plat->boundingBox = std::make_shared<CBoundingBox>(w, h);
			plat->sprite = std::make_shared<CSprite>(w, h, sf::Color(60, 100, 160));

			auto mp = std::make_shared<CMovingPlatform>();
			mp->posA = Vec2(x, y);
			mp->posB = Vec2(targetX, targetY);
			mp->speed = speed;
			mp->requiredTriggers = reqTriggers;
			plat->movingPlatform = mp;
		}
		else if (type == "Checkpoint") {
			float x, y, p1sx, p1sy, p2sx, p2sy;
			iss >> x >> y >> p1sx >> p1sy >> p2sx >> p2sy;

			auto cp = entityManager.AddEntity("Checkpoint");
			cp->transform = std::make_shared<CTransform>(Vec2(x, y), Vec2(0, 0), 0.0f);
			cp->boundingBox = std::make_shared<CBoundingBox>(32.0f, 32.0f);
			cp->sprite = std::make_shared<CSprite>(32.0f, 32.0f, sf::Color(0, 180, 255));

			auto c = std::make_shared<CCheckpoint>();
			c->p1Spawn = Vec2(p1sx, p1sy);
			c->p2Spawn = Vec2(p2sx, p2sy);
			cp->checkpoint = c;
		}
		else if (type == "Exit") {
			float x, y, w, h;
			iss >> x >> y >> w >> h;

			auto exit = entityManager.AddEntity("Exit");
			exit->transform = std::make_shared<CTransform>(Vec2(x, y), Vec2(0, 0), 0.0f);
			exit->boundingBox = std::make_shared<CBoundingBox>(w, h);
			exit->sprite = std::make_shared<CSprite>(w, h, sf::Color(255, 220, 0));
			exit->exit_ = std::make_shared<CExit>();
		}
		else if (type == "Music") {
			std::string trackName;
			iss >> trackName;
			std::cout << "Music Loaded " << trackName << std::endl;
			PushMusic(trackName);
		}
		else if (type == "Background") {
			std::string bgName;
			iss >> bgName;

			if (!bgName.empty() && bgName.back() == '\r') {
				bgName.pop_back();
			}

			std::cout << "Attempting to load Background: " << bgName << std::endl;

			sf::Texture& bgTex = getTexture(bgName);


			backgroundSprite.setTexture(bgTex, true);

			if (bgTex.getSize().x > 0 && bgTex.getSize().y > 0) {
				float scaleX = static_cast<float>(WINDOW_WIDTH) / bgTex.getSize().x;
				float scaleY = static_cast<float>(WINDOW_HEIGHT) / bgTex.getSize().y;
				backgroundSprite.setScale({ scaleX, scaleY });
			}

			hasBackground = true;
		}
		else {
			std::cerr << "Warning: Unknown entity type in config: " << type << std::endl;
		}
	}
}


void Game::spawnDustParticles(Vec2 position, int count, float directionX) {
	for (int i = 0; i < count; i++) {
		auto p = entityManager.AddEntity("Particle");

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

	player1 = entityManager.AddEntity("Player");
	player1->transform = std::make_shared<CTransform>(P1_SPAWN, Vec2(0.0f, 0.0f), 0.0f);
	player1->boundingBox = std::make_shared<CBoundingBox>(PLAYER_W, PLAYER_H);
	player1->sprite = std::make_shared<CSprite>(PLAYER_W, PLAYER_H, sf::Color(50, 100, 200));
	player1->input = std::make_shared<CInput>();
	player1->health = std::make_shared<CHealth>();

	player2 = entityManager.AddEntity("Player");
	player2->transform = std::make_shared<CTransform>(P2_SPAWN, Vec2(0.0f, 0.0f), 0.0f);
	player2->boundingBox = std::make_shared<CBoundingBox>(PLAYER_W, PLAYER_H);
	player2->sprite = std::make_shared<CSprite>(PLAYER_W, PLAYER_H, sf::Color(200, 50, 50));
	player2->input = std::make_shared<CInput>();
	player2->health = std::make_shared<CHealth>();
}

// Main loop

	while (Running) {
		entityManager.Update();
		sUserInput();

		sTransition();
		if (State == GameState::Playing) {
			sGravity();
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

						PopMusic();
					}
					else {
						Running = false;
					}
				}
				return;
			}

			if (State == GameState::GameOver) {
				if (kp->code == sf::Keyboard::Key::R) {
					PopMusic();
					entityManager = EntityManager();

					loadConfig(currentLevelPath);


					spawnPlayers();
					entityManager.Update();
					gameView = window.getDefaultView();
					State = GameState::Playing;
				}
				if (kp->code == sf::Keyboard::Key::Escape) {
					State = GameState::StartMenu;
					SelectedOption = 0;

					PushMusic("menu.ogg");
				}
				return;
			}
			if (State == GameState::GameWon) {
				if (kp->code == sf::Keyboard::Key::Enter ||
					kp->code == sf::Keyboard::Key::Space ||
					kp->code == sf::Keyboard::Key::Escape) {

		// Bullet and enemy collision
		for (auto& Bullets : entityManager.GetEntities("Bullet")) {
			if (Bullets->collision && Enemies->collision) {
				float dx = Bullets->transform->position.x - Enemies->transform->position.x;
				float dy = Bullets->transform->position.y - Enemies->transform->position.y;
				float distanceSquared = dx * dx + dy * dy;

					levelQueue.enqueue("Level3.txt");
				}
				return;
			}

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

							player->transform->position = targetDoor->transform->position;
							player->input->interact = false;
							break;
						}
					}
					break;
				}
			}
		}
	}
	for (auto& button : entityManager.GetEntities("Button")) {
		if (!button->interactable || !button->boundingBox || !button->transform) { continue; }

		auto& inter = button->interactable;
		bool wasPressed = inter->isPressed;

		bool anyOverlap = false;
		bool interactPressed = false;
		bool isHoldingInteract = false;

		for (auto& player : entityManager.GetEntities("Player")) {
			if (!player->transform || !player->boundingBox) { continue; }

			float dx = std::abs(player->transform->position.x - button->transform->position.x);
			float dy = std::abs(player->transform->position.y - button->transform->position.y);

			if (dx < player->boundingBox->halfSize.x + button->boundingBox->halfSize.x &&
				dy < player->boundingBox->halfSize.y + button->boundingBox->halfSize.y) {

				anyOverlap = true;

				if (player == player1 && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E)) {
					isHoldingInteract = true;
				}
				if (player == player2 && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift)) {
					isHoldingInteract = true;
				}

				if (player->input->interact) {
					interactPressed = true;
					player->input->interact = false;
				}
			}
		}

		if (inter->requiresInput && inter->requiresStay) {
			inter->isPressed = (anyOverlap && isHoldingInteract);
		}
		else if (inter->requiresInput && !inter->requiresStay) {
			if (anyOverlap && interactPressed) {
				inter->isPressed = !inter->isPressed;
			}
		}
		else if (!inter->requiresInput && inter->requiresStay) {
			inter->isPressed = anyOverlap;
		}
		else {
			if (anyOverlap) {
				inter->isPressed = true;
			}
		}

		if (inter->isPressed != wasPressed) {
			buttonSound.play();

			for (auto& ent : entityManager.GetEntities()) {

				if (ent->movingPlatform && ent->GetTag() == inter->linkedTag) {

					if (inter->isPressed) {
						ent->movingPlatform->currentTriggers++;
					}
					else {
						ent->movingPlatform->currentTriggers--;
					}

					ent->movingPlatform->triggered =
						(ent->movingPlatform->currentTriggers >= ent->movingPlatform->requiredTriggers);
				}

				if (ent->door && ent->door->linkTag == inter->linkedTag) {
					ent->door->isOpen = inter->isPressed;

					ent->sprite->getShape().setFillColor(
						ent->door->isOpen ? sf::Color(20, 20, 20) : sf::Color(80, 60, 40)
					);
				}
			}
		}

		if (button->sprite) {
			button->sprite->getShape().setFillColor(
				inter->isPressed ? sf::Color(0, 200, 100) : sf::Color(200, 100, 0)
			);
		}
	}
}


void Game::RenderStartMenu() {
	const float cx = WINDOW_WIDTH * 0.5f;
	const float cy = WINDOW_HEIGHT * 0.5f;

	sf::Text title(font), opt0(font), opt1(font);

	title.setCharacterSize(52);
	title.setFillColor(sf::Color::White);
	title.setString("Do bhai dono Tabahi");
	title.setPosition({ cx - title.getLocalBounds().size.x * 0.5f, cy - 140.f });

	opt0.setCharacterSize(30);
	opt0.setString("Play");
	opt0.setFillColor(SelectedOption == 0 ? sf::Color::Yellow : sf::Color(160, 160, 160));
	opt0.setPosition({ cx - opt0.getLocalBounds().size.x * 0.5f, cy });

	opt1.setCharacterSize(30);
	opt1.setString("Quit");
	opt1.setFillColor(SelectedOption == 1 ? sf::Color::Yellow : sf::Color(160, 160, 160));
	opt1.setPosition({ cx - opt1.getLocalBounds().size.x * 0.5f, cy + 60.f });

	sf::Text arrow(font);
	arrow.setCharacterSize(30);
	arrow.setFillColor(sf::Color::Yellow);
	arrow.setString(">");
	float arrowY = (SelectedOption == 0) ? cy : cy + 60.f;
	arrow.setPosition({ cx - 120.f, arrowY });

	window.draw(title);
	window.draw(opt0);
	window.draw(opt1);
	window.draw(arrow);
}

void Game::RenderGameOver() {
	const float cx = WINDOW_WIDTH * 0.5f;
	const float cy = WINDOW_HEIGHT * 0.5f;

	sf::Text over(font), sub(font), hint(font);

	over.setCharacterSize(56);
	over.setFillColor(sf::Color(220, 60, 60));
	over.setString("Game Over");
	over.setPosition({ cx - over.getLocalBounds().size.x * 0.5f, cy - 120.f });

	sub.setCharacterSize(28);
	sub.setFillColor(sf::Color::White);
	sub.setString("Both players ran out of lives");
	sub.setPosition({ cx - sub.getLocalBounds().size.x * 0.5f, cy });

	hint.setCharacterSize(22);
	hint.setFillColor(sf::Color(160, 160, 160));
	hint.setString("R  — Restart      Escape — Main Menu");
	hint.setPosition({ cx - hint.getLocalBounds().size.x * 0.5f, cy + 70.f });

	window.draw(over);
	window.draw(sub);
	window.draw(hint);
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
	// Reset player velocity
	myplayer->transform->velocity = { 0.0f, 0.0f };

	// Player movement
	if (myplayer->input->down && myplayer->transform->position.y + playerConfig.SR < window.getSize().y) {
		myplayer->transform->velocity.y = playerConfig.V;
	}
	else if (myplayer->input->up && myplayer->transform->position.y - playerConfig.SR > 0) {
		myplayer->transform->velocity.y = -playerConfig.V;
	}

			t->onGround = false;
			t->coyoteFrames = 0;
			t->JumpBufferFrames = 0;

			spawnDustParticles(
				Vec2(t->position.x, t->position.y + e->boundingBox->halfSize.y), 8
			);

			jumpSound.play();
		}

		bool pushingLeft = in->left && !in->right;
		bool pushingRight = in->right && !in->left;

		bool turningLeft = pushingLeft && t->velocity.x > 0.5f;
		bool turningRight = pushingRight && t->velocity.x < -0.5f;
		bool turning = turningLeft || turningRight;

		if (turning) {
			t->velocity.x *= TURN_FRICTION;

			if (std::abs(t->velocity.x) > 1.5f) {
				Vec2 dustPos = Vec2(t->position.x, t->position.y + e->boundingBox->halfSize.y);
				float skidDir = (t->velocity.x > 0.0f) ? 1.0f : -1.0f;
				spawnDustParticles(dustPos, 5, skidDir);
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
			case sf::Keyboard::Key::P:
				SetPaused(!Paused);
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

void Game::RenderHud() {
	sf::Text p1Text(font), p2Text(font);
	p1Text.setCharacterSize(24);
	p2Text.setCharacterSize(24);
	p1Text.setFillColor(sf::Color::Cyan);
	p2Text.setFillColor(sf::Color::White);

	p1Text.setString("P1:");
	p2Text.setString("P2:");

	p1Text.setPosition({ 20.f, 20.f });
	p2Text.setPosition({ 20.f, 60.f });

	window.draw(p1Text);
	window.draw(p2Text);

	sf::Sprite p1Heart(p1HeartTex);
	sf::Sprite p2Heart(p2HeartTex);

	p1Heart.setScale({ 0.05f, 0.05f });
	p2Heart.setScale({ 0.05f, 0.05f });

	float heartSpacing = 40.0f;
	float startXOffset = 60.0f;

	for (int i = 0; i < player1->health->lives; i++) {
		p1Heart.setPosition({ p1Text.getPosition().x + startXOffset + (i * heartSpacing), 20.f });
		window.draw(p1Heart);
	}

	for (int i = 0; i < player2->health->lives; i++) {
		p2Heart.setPosition({ p2Text.getPosition().x + startXOffset + (i * heartSpacing), 60.f });
		window.draw(p2Heart);
	}
}

void Game::sRender() {
	window.clear(sf::Color(30, 30, 50));

	window.setView(window.getDefaultView());

	if (hasBackground) {
		backgroundSprite.setPosition({ 0.f, 0.f });
		window.draw(backgroundSprite);
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


	window.setView(window.getDefaultView());

	window.setView(gameView);
	for (auto& e : entityManager.GetEntities()) {
		if (e->transform && e->sprite) {
			e->sprite->setPosition(e->transform->position);
			window.draw(e->sprite->getShape());
		}
	}

	sParticle();

	if (State == GameState::RespawnFadeOut || State == GameState::RespawnFadeIn) {
		sf::CircleShape wipeCircle;
		float r = std::max(0.0f, transitionRadius);
		wipeCircle.setRadius(r);
		wipeCircle.setOrigin({ r, r });
		wipeCircle.setPosition({ transitionCenter.x, transitionCenter.y });

		wipeCircle.setFillColor(sf::Color::Transparent);
		wipeCircle.setOutlineColor(sf::Color::Black);
		wipeCircle.setOutlineThickness(4000.0f);

		window.draw(wipeCircle);
	}

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
			moveAmount = target - current;
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
				player->transform->position += moveAmount;
			}
		}
	}
}

void Game::sPatrol() {
	for (auto& e : entityManager.GetEntities("Enemy")) {
		if (!e->patrol || !e->transform) { continue; }

		auto& patrol = e->patrol;
		auto& t = e->transform;

		if (patrol->waypoints.empty()) { continue; }

		Vec2  target = patrol->waypoints[patrol->currentTarget];
		Vec2  delta = target - t->position;
		float dist = std::sqrt(delta.x * delta.x + delta.y * delta.y);

		if (dist < 4.0f) {
			patrol->currentTarget =
				(patrol->currentTarget + 1) % static_cast<int>(patrol->waypoints.size());
		}
		else {
			Vec2 dir = delta * (1.0f / dist);
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

		Vec2 facing = { 1.0f, 0.0f };
		if (enemy->patrol) {
			facing = enemy->patrol->facingRight
				? Vec2(1.0f, 0.0f)
				: Vec2(-1.0f, 0.0f);
		}

		float cosHalf = std::cos(sight->halfAngleDeg * PI / 180.0f);

		for (auto& player : entityManager.GetEntities("Player")) {
			if (!player->transform || !player->health) { continue; }
			if (player->health->isDead) { continue; }

			Vec2  toPlayer = player->transform->position - enemyPos;
			float dist = std::sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);

			if (dist < sight->range && dist > 0.0f) {
				Vec2  dir = toPlayer * (1.0f / dist);
				float dot = facing.x * dir.x + facing.y * dir.y;

				if (dot > cosHalf) {
					player->health->lives--;
					if (player->health->lives <= 0) {
						State = GameState::GameOver;
						PushMusic("gameover.ogg");
					}
					else {
						StartRespawn(player->transform->position);
					}
					return;
				}
			}
		}
	}
}


	State = GameState::RespawnFadeOut;
	transitionCenter = focusPoint;
	transitionRadius = 3000.0f;

	isLoadingNextLevel = false;
}

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

void Game::sTransition() {
	float fadeSpeed = 70.0f;

	if (State == GameState::RespawnFadeOut) {
		transitionRadius -= fadeSpeed;
		if (transitionRadius <= 0.0f) {
			transitionRadius = 0.0f;

			if (isLoadingNextLevel) {
				LoadNextLevel();
			}
			else {
				ApplyReset();
			}
			// -----------------------
		}
	}
	else if (State == GameState::RespawnFadeIn) {
		transitionRadius += fadeSpeed;
		if (transitionRadius >= 3000.0f) {
			State = GameState::Playing;
		}
	}
}



void Game::PopMusic() {
	if (!musicStack.isEmpty()) {
		musicStack.pop();
	}

	bgMusic.stop();

	if (!musicStack.isEmpty()) {
		std::string previousTrack = musicStack.top();
		if (bgMusic.openFromFile(previousTrack)) {
			bgMusic.setLooping(true);
			bgMusic.setVolume(30.f);
			bgMusic.play();
		}
	}
}

sf::Texture& Game::getTexture(const std::string& name) {
	auto it = textureCache.find(name);
	if (it != textureCache.end()) {
		return it->second;
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
void Game::RenderGameWon() {
	const float cx = WINDOW_WIDTH * 0.5f;
	const float cy = WINDOW_HEIGHT * 0.5f;

	sf::Text over(font), sub(font), hint(font);

	over.setCharacterSize(56);
	over.setFillColor(sf::Color(255, 215, 0));
	over.setString("VICTORY!");
	over.setPosition({ cx - over.getLocalBounds().size.x * 0.5f, cy - 120.f });

	sub.setCharacterSize(28);
	sub.setFillColor(sf::Color::White);
	sub.setString("DONO BHAI GAYE BILLO DE GHAR");
	sub.setPosition({ cx - sub.getLocalBounds().size.x * 0.5f, cy });

	hint.setCharacterSize(22);
	hint.setFillColor(sf::Color(160, 160, 160));
	hint.setString("Press Enter or Space to return to Main Menu");
	hint.setPosition({ cx - hint.getLocalBounds().size.x * 0.5f, cy + 70.f });

void Game::SetPaused(bool paused) {
	Paused = paused;
}
