#include "Game.h"
#include <optional>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <cstdint>

Game::Game() {
	init();
}

void Game::init() {

	levelQueue.enqueue("level1.txt");
	levelQueue.enqueue("Level2.txt");
	levelQueue.enqueue("Level3.txt");

	if (!levelQueue.isEmpty()) {
		currentLevelPath = levelQueue.front();
		levelQueue.dequeue();
		loadConfig(currentLevelPath);
	}
	if (font.openFromFile("Fonts/Coolvetica Rg.otf")) {
		std::cout << "Font loed successfully." << std::endl;
	}
	else {
		std::cerr << "Failed to load font ##########." << std::endl;
	}

	if (!p1HeartTex.loadFromFile("Player1hearts.png")) {
		std::cerr << "Failed to load Player1hearts.png" << std::endl;
	}
	if (!p2HeartTex.loadFromFile("Player2hearts.png")) {
		std::cerr << "Failed to load Player2hearts.png" << std::endl;
	}

	if (jumpBuffer.loadFromFile("jump.wav")) {
		jumpSound.setVolume(50.f);
	}
	else {
		std::cerr << "Failed to load jump.wav" << std::endl;
	}

	if (buttonBuffer.loadFromFile("button.wav")) {
		buttonSound.setVolume(80.f);
	}
	else {
		std::cerr << "Failed to load button.wav" << std::endl;
	}

	window.create(sf::VideoMode({ WINDOW_WIDTH, WINDOW_HEIGHT }), "2 bhai 2no Tabahi");
	window.setFramerateLimit(60);
	spawnPlayers();
	entityManager.Update();
	gameView = window.getDefaultView();


	PushMusic("menu.ogg");

	musicStack.print();
}


// Load Config
#include <sstream>

void Game::loadConfig(const std::string& path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		std::cerr << "Could not open config: " << path << std::endl;
		return;
	}

	hasBackground = false;

	std::string line;
	while (std::getline(file, line)) {
		if (line.empty() || line[0] == '#' || line[0] == '\r') {
			continue;
		}

		std::istringstream iss(line);
		std::string type;
		iss >> type;

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

			auto enemy = entityManager.AddEntity("Enemy");
			enemy->transform = std::make_shared<CTransform>(Vec2(x, y), Vec2(0, 0), 0.0f);
			enemy->boundingBox = std::make_shared<CBoundingBox>(40.0f, 48.0f);
			enemy->sprite = std::make_shared<CSprite>(40.0f, 48.0f, sf::Color(220, 120, 0));

			auto patrol = std::make_shared<CPatrol>();
			patrol->waypoints.push_back(Vec2(wp1x, wp1y));
			patrol->waypoints.push_back(Vec2(wp2x, wp2y));
			patrol->speed = speed;
			enemy->patrol = patrol;

	Running = true;
	Paused = false;
	LastEnemySpawnTime = 0;

}


void Game::Run() {
	window.setFramerateLimit(GameConfig::FRAMERATE_LIMIT);
	entityManager.Update();

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
			std::cout << "Music Loaded "<< trackName<<std::endl;
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

void Game::Run() {
	while (Running) {
		entityManager.Update();
		sUserInput();
		if (!Paused) {
			sEnemySpawn();
			sMovement();
			sPatrol();
			sSight();
			sHealth();
			sInteract();
			sMovingPlatform();
			sWinCondition();
			sCollision();
			sCamera();
		}
		sRender();
		currentFrame++;
	}
}

void Game::sCamera() {

	bool p1Alive = player1->transform && !player1->health->isDead;
	bool p2Alive = player2->transform && !player2->health->isDead;

	if (!p1Alive && !p2Alive) { return; }

	float cx, cy, viewW, viewH;

	if (p1Alive && p2Alive) {
		Vec2 p1 = player1->transform->position;
		Vec2 p2 = player2->transform->position;

		cx = (p1.x + p2.x) / 2.0f;
		cy = (p1.y + p2.y) / 2.0f;

		float dx = std::abs(p1.x - p2.x) + 120.f;
		float dy = std::abs(p1.y - p2.y) + 120.f;

		float aspect = static_cast<float>(WINDOW_HEIGHT) / WINDOW_WIDTH;

		float minViewW = 800.f;
		float minViewH = minViewW * aspect;

		if (dy > dx * aspect) {
			viewH = std::max(minViewH, dy);
			viewW = viewH / aspect;
		}
		else {
			viewW = std::max(minViewW, dx);
			viewH = viewW * aspect;
		}

		if (viewW > 2200.f) {
			viewW = 2200.f;
			viewH = 2200.f * aspect;
		}
	}
	else {
		Vec2 pos = p1Alive ? player1->transform->position
			: player2->transform->position;
		cx = pos.x;
		cy = pos.y;
		viewW = static_cast<float>(WINDOW_WIDTH);
		viewH = static_cast<float>(WINDOW_HEIGHT);
	}

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
			if (State == GameState::StartMenu) {
				if (kp->code == sf::Keyboard::Key::Up ||
					kp->code == sf::Keyboard::Key::W) {
					SelectedOption = (SelectedOption + 1) % 2;
				}
				if (kp->code == sf::Keyboard::Key::Down ||
					kp->code == sf::Keyboard::Key::S) {
					SelectedOption = (SelectedOption + 1) % 2;
				}
				if (kp->code == sf::Keyboard::Key::Enter ||
					kp->code == sf::Keyboard::Key::Space) {
					if (SelectedOption == 0) {
						State = GameState::Playing;

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

					State = GameState::StartMenu;
					SelectedOption = 0;

				int combinedRadius = Bullets->collision->radius + Enemies->collision->radius;

			switch (kp->code) {
			case sf::Keyboard::Key::W: player1->input->jump = true; break;
			case sf::Keyboard::Key::A: player1->input->left = true; break;
			case sf::Keyboard::Key::D: player1->input->right = true; break;
			case sf::Keyboard::Key::E: player1->input->interact = true; break;

			case sf::Keyboard::Key::Up: player2->input->jump = true; break;
			case sf::Keyboard::Key::Left:  player2->input->left = true; break;
			case sf::Keyboard::Key::Right: player2->input->right = true; break;
			case sf::Keyboard::Key::RShift: player2->input->interact = true; break;

			default: break;
			}
		}

		if (const auto* kr = event->getIf<sf::Event::KeyReleased>()) {
			switch (kr->code) {
			case sf::Keyboard::Key::A: player1->input->left = false; break;
			case sf::Keyboard::Key::D: player1->input->right = false; break;
			case sf::Keyboard::Key::W: player1->input->jump = false; break;
			case sf::Keyboard::Key::E: player1->input->interact = false; break;

			case sf::Keyboard::Key::Left:  player2->input->left = false; break;
			case sf::Keyboard::Key::Right: player2->input->right = false; break;
			case sf::Keyboard::Key::Up: player2->input->jump = false; break;
			case sf::Keyboard::Key::RShift: player2->input->interact = false; break;

			default: break;
			}
		}
	}
}

void Game::sInteract() {
	for (auto& player : entityManager.GetEntities("Player")) {
		if (!player->transform || !player->boundingBox) { continue; }

		if (player->input->interact) {
			for (auto& door : entityManager.GetEntities("Door")) {
				if (!door->door || !door->door->isOpen || !door->transform || !door->boundingBox) { continue; }

				float dx = std::abs(player->transform->position.x - door->transform->position.x);
				float dy = std::abs(player->transform->position.y - door->transform->position.y);

				if (dx < player->boundingBox->halfSize.x + door->boundingBox->halfSize.x &&
					dy < player->boundingBox->halfSize.y + door->boundingBox->halfSize.y) {

					for (auto& targetDoor : entityManager.GetEntities("Door")) {
						if (targetDoor != door && targetDoor->door->linkTag == door->door->linkTag) {

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
	for (auto& e : entityManager.GetEntities("Player")) {
		if (!e->transform || !e->input) { continue; }

		auto& t = e->transform;
		auto& in = e->input;

		if (in->jump) {
			t->JumpBufferFrames = 8;
			in->jump = false;

		}
		if (t->JumpBufferFrames > 0) { t->JumpBufferFrames--; }

		bool canJump = t->onGround || t->coyoteFrames > 0;
		if (t->JumpBufferFrames > 0 && canJump) {

			if (t->velocity.y < 0.0f) {
				t->velocity.y += JUMP_VELOCITY;
			}
			else {
				t->velocity.y = JUMP_VELOCITY;
			}
			// ------------------------------

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

		}
		else if (pushingLeft) {
			t->velocity.x -= ACCELERATION;

		}
		else if (pushingRight) {
			t->velocity.x += ACCELERATION;

		}
		else {
			t->velocity.x *= FRICTION;

			if (std::abs(t->velocity.x) < 0.15f) {
				t->velocity.x = 0.0f;
			}
		}

		t->velocity.x = std::clamp(t->velocity.x, -MAX_MOVE_SPEED, MAX_MOVE_SPEED);

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

		std::vector<std::shared_ptr<Entity>> solidEntities;

		for (auto& tile : entityManager.GetEntities("Tile")) {
			solidEntities.push_back(tile);
		}

		for (auto& ent : entityManager.GetEntities()) {
			if (ent->movingPlatform) {
				solidEntities.push_back(ent);
			}
		}

	if (myplayer->input->right && myplayer->transform->position.x + playerConfig.SR < window.getSize().x) {
		myplayer->transform->velocity.x = playerConfig.V;
	}
	else if (myplayer->input->left && myplayer->transform->position.x - playerConfig.SR > 0) {
		myplayer->transform->velocity.x = -playerConfig.V;
	}

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

			if (e.get() >= Other.get()) { continue; }
			if (!Other->transform || !Other->boundingBox) { continue; }

			const float hw = e->boundingBox->halfSize.x;
			const float hh = e->boundingBox->halfSize.y;
			const float otherHW = Other->boundingBox->halfSize.x;
			const float otherHH = Other->boundingBox->halfSize.y;

			float dx = e->transform->position.x - Other->transform->position.x;
			float dy = e->transform->position.y - Other->transform->position.y;

			if (std::abs(dx) < hw + otherHW && std::abs(dy) < hh + otherHH) {
				float overlapX = (hw + otherHW) - std::abs(dx);
				float overlapY = (hh + otherHH) - std::abs(dy);

				if (overlapY < overlapX + 16.0f) {

					if (dy < 0.0f) {
						e->transform->position.y -= overlapY;
						e->transform->onGround = true;
						e->transform->coyoteFrames = 8;

						if (e->transform->velocity.y > 0.0f) {
							e->transform->velocity.y = (Other->transform->velocity.y < 0.0f) ? Other->transform->velocity.y : 0.0f;
						}

						e->transform->position.x += Other->transform->velocity.x;

					}
					else {
						Other->transform->position.y -= overlapY;
						Other->transform->onGround = true;
						Other->transform->coyoteFrames = 8;

						if (Other->transform->velocity.y > 0.0f) {
							Other->transform->velocity.y = (e->transform->velocity.y < 0.0f) ? e->transform->velocity.y : 0.0f;
						}

						Other->transform->position.x += e->transform->velocity.x;
					}

				}
				else {

					if (dx < 0.0f) {
						e->transform->position.x -= overlapX * 0.5f;
						Other->transform->position.x += overlapX * 0.5f;
					}
					else {
						e->transform->position.x += overlapX * 0.5f;
						Other->transform->position.x -= overlapX * 0.5f;
					}

					e->transform->velocity.x *= 0.5f;
					Other->transform->velocity.x *= 0.5f;
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

		t->velocity.y += 0.08f;
		t->position.x += t->velocity.x;
		t->position.y += t->velocity.y;

		sf::Color c = p->color;
		c.a = static_cast<std::uint8_t>(p->alpha() * 255);
		e->sprite->getShape().setFillColor(c);
	}
}

void Game::sHealth() {
	for (auto& e : entityManager.GetEntities("Player")) {
		if (!e->health || !e->transform) { continue; }

		if (e->transform->position.y > WINDOW_HEIGHT + 100.0f) {
			e->health->lives--;

			if (e->health->lives <= 0) {
				State = GameState::GameOver;
				PushMusic("gameover.ogg");
			}
			else {
				StartRespawn(e->transform->position);
			}
			return;
		}
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


	if (State == GameState::StartMenu) {
		window.setView(window.getDefaultView());
		RenderStartMenu();
		window.display();
		return;
	}

	if (State == GameState::GameOver) {
		window.setView(window.getDefaultView());
		RenderGameOver();
		window.display();
		return;
	}

	if (State == GameState::GameWon) {
		window.setView(window.getDefaultView());
		RenderGameWon();
		window.display();
		return;
	}
	else {
		Running = false;
		return;
	}
}

void Game::StartRespawn(Vec2 focusPoint) {
	if (State == GameState::RespawnFadeOut || State == GameState::RespawnFadeIn) { return; }

	State = GameState::RespawnFadeOut;
	transitionCenter = focusPoint;  
	transitionRadius = 3000.0f;     

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

	transitionCenter = Vec2((P1_SPAWN.x + P2_SPAWN.x) * 0.5f, (P1_SPAWN.y + P2_SPAWN.y) * 0.5f);
	State = GameState::RespawnFadeIn;
}



void Game::sWinCondition() {
	for (auto& exit : entityManager.GetEntities("Exit")) {
		if (!exit->transform || !exit->boundingBox) { continue; }

		int playersAtExit = 0;

		for (auto& player : entityManager.GetEntities("Player")) {
			if (!player->transform || !player->boundingBox || player->health->isDead) { continue; }

			float dx = std::abs(player->transform->position.x - exit->transform->position.x);
			float dy = std::abs(player->transform->position.y - exit->transform->position.y);

			if (dx < player->boundingBox->halfSize.x + exit->boundingBox->halfSize.x &&
				dy < player->boundingBox->halfSize.y + exit->boundingBox->halfSize.y) {
				playersAtExit++;
			}
		}

		if (playersAtExit >= 2) {
			StartWipe(exit->transform->position, true);
		}
	}
}
void Game::LoadNextLevel() {
	if (levelQueue.isEmpty()) {
		State = GameState::GameWon;

		PushMusic("menu.ogg");
		return;
	}

	currentLevelPath = levelQueue.front();
	levelQueue.dequeue();

	entityManager = EntityManager();

	loadConfig(currentLevelPath);

	spawnPlayers();

	entityManager.Update();

	transitionCenter = Vec2((P1_SPAWN.x + P2_SPAWN.x) * 0.5f, (P1_SPAWN.y + P2_SPAWN.y) * 0.5f);
	State = GameState::RespawnFadeIn;
}

void Game::StartWipe(Vec2 focusPoint, bool advancingLevel) {
	if (State == GameState::RespawnFadeOut || State == GameState::RespawnFadeIn) { return; }

	State = GameState::RespawnFadeOut;
	transitionCenter = focusPoint;
	transitionRadius = 3000.0f;

	isLoadingNextLevel = advancingLevel;
}

	// Random velocity
	float vx = static_cast<float>(rand() % static_cast<int>(enemyConfig.VMax - enemyConfig.VMin + 1) + static_cast<int>(enemyConfig.VMin));
	float vy = static_cast<float>(rand() % static_cast<int>(enemyConfig.VMax - enemyConfig.VMin + 1) + static_cast<int>(enemyConfig.VMin));
	Vec2 Velocity = { vx, vy };

	enemy->transform->position = Position;
	enemy->transform->velocity = Velocity;
}

void Game::PushMusic(const std::string& path) {
	if (!musicStack.isEmpty() && musicStack.top() == path) { return; }

	musicStack.push(path);

	bgMusic.stop();
	if (bgMusic.openFromFile(path)) {
		bgMusic.setLooping(true);
		bgMusic.setVolume(30.f);
		bgMusic.play();
	}
	else {
		std::cerr << "Warning: Could not load music track: " << path << std::endl;
	}
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

	std::string path = "Textures/" + name + ".png";

	if (!textureCache[name].loadFromFile(path)) {
		std::cerr << "!!! ERROR: Could not find " << path << " !!!" << std::endl;

		sf::Image pinkImage;
		pinkImage.resize({ 2, 2 }, sf::Color::Magenta);

		bool fallbackLoaded = textureCache[name].loadFromImage(pinkImage);
		if (!fallbackLoaded) {
			std::cerr << "Failed to generate fallback texture!" << std::endl;
		}
	}

	textureCache[name].setSmooth(false);
	return textureCache[name];
}

	window.draw(over);
	window.draw(sub);
	window.draw(hint);
}