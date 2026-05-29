#include "Game.h"
#include <optional>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <cstdint>

namespace {
	constexpr const char* kLevel1Path = "level1.txt";
	constexpr const char* kLevel2Path = "Level2.txt";
	constexpr const char* kLevel3Path = "Level3.txt";
	constexpr const char* kFontPath = "Fonts/Coolvetica Rg.otf";
	constexpr const char* kPlayer1HeartsPath = "Player1hearts.png";
	constexpr const char* kPlayer2HeartsPath = "Player2hearts.png";
	constexpr const char* kJumpSoundPath = "jump.wav";
	constexpr const char* kButtonSoundPath = "button.wav";
	constexpr const char* kWindowTitle = "2 bhai 2no Tabahi";
	constexpr const char* kMenuMusicPath = "menu.ogg";
	constexpr const char* kGameOverMusicPath = "gameover.ogg";
	constexpr const char* kTexturesDirectory = "Textures/";
	constexpr const char* kTextureExtension = ".png";
	constexpr unsigned int kFallbackTextureSize = 2;

	constexpr unsigned int kFrameLimit = 60;
	constexpr float kJumpVolume = 50.0f;
	constexpr float kButtonVolume = 80.0f;
	constexpr float kMusicVolume = 30.0f;

	constexpr float kDustBiasMultiplier = 1.5f;
	constexpr int kDustRandRangeX = 200;
	constexpr int kDustRandOffsetX = 100;
	constexpr float kDustRandDivisor = 100.0f;
	constexpr int kDustRandRangeY = 150;
	constexpr int kDustRandOffsetY = 30;
	constexpr float kDustSize = 4.0f;
	constexpr float kDustLifetime = 18.0f;
	const sf::Color kDustColor(200, 170, 120);

	constexpr float kEnemyWidth = 40.0f;
	constexpr float kEnemyHeight = 48.0f;
	const sf::Color kEnemyColor(220, 120, 0);
	const sf::Color kButtonColor(0, 0, 255);
 const sf::Color kButtonPressedColor(0, 200, 100);
	const sf::Color kButtonReleasedColor(200, 100, 0);
	const sf::Color kDoorColor(80, 60, 40);
	const sf::Color kDoorOpenColor(20, 20, 20);
	const sf::Color kPlatformColor(60, 100, 160);
	constexpr float kCheckpointSize = 32.0f;
	const sf::Color kCheckpointColor(0, 180, 255);
	const sf::Color kExitColor(255, 220, 0);
	const sf::Color kGroundColor(100, 80, 60);
	const sf::Color kPlayer1Color(50, 100, 200);
	const sf::Color kPlayer2Color(200, 50, 50);
	const sf::Color kBackgroundClearColor(30, 30, 50);

	constexpr float kCameraPadding = 120.0f;
	constexpr float kMinViewWidth = 800.0f;
	constexpr float kMaxViewWidth = 2200.0f;
	constexpr float kCameraLerp = 0.1f;

	constexpr int kMenuOptionCount = 2;
	constexpr unsigned int kMenuTitleSize = 52;
	constexpr unsigned int kMenuOptionSize = 30;
	constexpr unsigned int kMenuArrowSize = 30;
	constexpr float kMenuTitleOffset = 140.0f;
	constexpr float kMenuOptionOffset = 60.0f;
	constexpr float kMenuArrowOffset = 120.0f;
	constexpr const char* kMenuTitleText = "Do bhai dono Tabahi";
	constexpr const char* kMenuPlayText = "Play";
	constexpr const char* kMenuQuitText = "Quit";
	constexpr const char* kMenuArrowText = ">";
	const sf::Color kMenuInactiveColor(160, 160, 160);

	constexpr unsigned int kGameOverTitleSize = 56;
	constexpr unsigned int kGameOverSubtitleSize = 28;
	constexpr unsigned int kGameOverHintSize = 22;
	constexpr float kGameOverTitleOffset = 120.0f;
	constexpr float kGameOverHintOffset = 70.0f;
	constexpr const char* kGameOverTitleText = "Game Over";
	constexpr const char* kGameOverSubtitleText = "Both players ran out of lives";
	constexpr const char* kGameOverHintText = "R  — Restart      Escape — Main Menu";
	const sf::Color kGameOverTitleColor(220, 60, 60);
	const sf::Color kHudHintColor(160, 160, 160);

	constexpr unsigned int kGameWonTitleSize = 56;
	constexpr unsigned int kGameWonSubtitleSize = 28;
	constexpr unsigned int kGameWonHintSize = 22;
	constexpr float kGameWonTitleOffset = 120.0f;
	constexpr float kGameWonHintOffset = 70.0f;
	constexpr const char* kGameWonTitleText = "VICTORY!";
	constexpr const char* kGameWonSubtitleText = "DONO BHAI GAYE BILLO DE GHAR";
	constexpr const char* kGameWonHintText = "Press Enter or Space to return to Main Menu";
	const sf::Color kGameWonTitleColor(255, 215, 0);

	constexpr int kJumpBufferFrames = 8;
	constexpr int kCoyoteFrames = 8;
	constexpr float kTurnThreshold = 0.5f;
	constexpr float kSkidSpeedThreshold = 1.5f;
	constexpr int kSkidParticleCount = 5;
	constexpr float kStopVelocityThreshold = 0.15f;
	constexpr int kJumpDustCount = 8;
	constexpr int kLandingDustCount = 6;
	constexpr float kLandingVelocityThreshold = 1.0f;

	constexpr float kPlayerOverlapBias = 16.0f;
	constexpr float kPlayerPushFactor = 0.5f;

	constexpr float kParticleAgeStep = 1.0f;
	constexpr float kParticleGravity = 0.08f;
	constexpr std::uint8_t kParticleAlphaScale = 255;

	constexpr float kFallDeathOffset = 100.0f;
	constexpr int kRequiredPlayersAtExit = 2;

	constexpr unsigned int kHudTextSize = 24;
	constexpr float kHudTextOffsetX = 20.0f;
	constexpr float kHudP1OffsetY = 20.0f;
	constexpr float kHudP2OffsetY = 60.0f;
	constexpr float kHeartScale = 0.05f;
	constexpr float kHeartSpacing = 40.0f;
	constexpr float kHeartStartOffsetX = 60.0f;

	constexpr float kWipeOutlineThickness = 4000.0f;
	constexpr float kPlatformSnapThreshold = 3.0f;
	constexpr float kPatrolTargetThreshold = 4.0f;

	constexpr float kTransitionRadiusStart = 3000.0f;
	constexpr float kTransitionFadeSpeed = 70.0f;

	constexpr float kPi = 3.14159265f;

	inline void AdvanceMenuOption(int& option) {
		option = (option + 1) % kMenuOptionCount;
	}

    inline bool PlayBackgroundTrack(sf::Music& music, const std::string& path) {
		if (!music.openFromFile(path)) {
			return false;
		}
		music.setLooping(true);
		music.setVolume(kMusicVolume);
		music.play();
		return true;
	}
}

Game::Game() {
	init();
}

void Game::init() {

	levelQueue.enqueue("level1.txt");
   levelQueue.enqueue(kLevel1Path);
	levelQueue.enqueue(kLevel2Path);
	levelQueue.enqueue(kLevel3Path);

	if (!levelQueue.isEmpty()) {
		currentLevelPath = levelQueue.front();
		levelQueue.dequeue();
		loadConfig(currentLevelPath);
	}
 if (font.openFromFile(kFontPath)) {
		std::cout << "Font loed successfully." << std::endl;
	}
	else {
		std::cerr << "Failed to load font ##########." << std::endl;
	}

    if (!p1HeartTex.loadFromFile(kPlayer1HeartsPath)) {
		std::cerr << "Failed to load Player1hearts.png" << std::endl;
	}
    if (!p2HeartTex.loadFromFile(kPlayer2HeartsPath)) {
		std::cerr << "Failed to load Player2hearts.png" << std::endl;
	}

  if (jumpBuffer.loadFromFile(kJumpSoundPath)) {
		jumpSound.setVolume(kJumpVolume);
	}
	else {
		std::cerr << "Failed to load jump.wav" << std::endl;
	}

  if (buttonBuffer.loadFromFile(kButtonSoundPath)) {
		buttonSound.setVolume(kButtonVolume);
	}
	else {
		std::cerr << "Failed to load button.wav" << std::endl;
	}

 window.create(sf::VideoMode({ WINDOW_WIDTH, WINDOW_HEIGHT }), kWindowTitle);
	window.setFramerateLimit(kFrameLimit);
	spawnPlayers();
	entityManager.Update();
	gameView = window.getDefaultView();


  PushMusic(kMenuMusicPath);

	musicStack.print();
}


// Load Config
#include <sstream>

// Parses a level configuration file and spawns entities for the current scene.
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
          enemy->boundingBox = std::make_shared<CBoundingBox>(kEnemyWidth, kEnemyHeight);
			enemy->sprite = std::make_shared<CSprite>(kEnemyWidth, kEnemyHeight, kEnemyColor);

			auto patrol = std::make_shared<CPatrol>();
			patrol->waypoints.push_back(Vec2(wp1x, wp1y));
			patrol->waypoints.push_back(Vec2(wp2x, wp2y));
			patrol->speed = speed;
			enemy->patrol = patrol;

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
         button->sprite = std::make_shared<CSprite>(w, h, kButtonColor);

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
          door->sprite = std::make_shared<CSprite>(w, h, kDoorColor);

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
            plat->sprite = std::make_shared<CSprite>(w, h, kPlatformColor);

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
         cp->boundingBox = std::make_shared<CBoundingBox>(kCheckpointSize, kCheckpointSize);
			cp->sprite = std::make_shared<CSprite>(kCheckpointSize, kCheckpointSize, kCheckpointColor);

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
         exit->sprite = std::make_shared<CSprite>(w, h, kExitColor);
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

      float biasX = (directionX != 0.0f) ? directionX * kDustBiasMultiplier : 0.0f;
		float vx = biasX + ((rand() % kDustRandRangeX) - kDustRandOffsetX) / kDustRandDivisor;
		float vy = -((rand() % kDustRandRangeY) + kDustRandOffsetY) / kDustRandDivisor;

		p->transform = std::make_shared<CTransform>(position, Vec2(vx, vy), 0.0f);
        p->sprite = std::make_shared<CSprite>(kDustSize, kDustSize, kDustColor);
		p->particle = std::make_shared<CParticle>(kDustLifetime, kDustColor);
	}
}

void Game::spawnGround() {
	auto ground = entityManager.AddEntity("Ground");

	const float w = static_cast<float>(WINDOW_WIDTH);
	const float cx = w * 0.5f;
	const float cy = static_cast<float>(WINDOW_HEIGHT) - GROUND_H * 0.5f;

	ground->transform = std::make_shared<CTransform>(Vec2(cx, cy), Vec2(0.0f, 0.0f), 0.0f);
	ground->boundingBox = std::make_shared<CBoundingBox>(w, GROUND_H);
    ground->sprite = std::make_shared<CSprite>(w, GROUND_H, kGroundColor);
}

void Game::spawnPlayers() {
	player1 = entityManager.AddEntity("Player");
	player1->transform = std::make_shared<CTransform>(P1_SPAWN, Vec2(0.0f, 0.0f), 0.0f);
	player1->boundingBox = std::make_shared<CBoundingBox>(PLAYER_W, PLAYER_H);
   player1->sprite = std::make_shared<CSprite>(PLAYER_W, PLAYER_H, kPlayer1Color);
	player1->input = std::make_shared<CInput>();
	player1->health = std::make_shared<CHealth>();

	player2 = entityManager.AddEntity("Player");
	player2->transform = std::make_shared<CTransform>(P2_SPAWN, Vec2(0.0f, 0.0f), 0.0f);
	player2->boundingBox = std::make_shared<CBoundingBox>(PLAYER_W, PLAYER_H);
    player2->sprite = std::make_shared<CSprite>(PLAYER_W, PLAYER_H, kPlayer2Color);
	player2->input = std::make_shared<CInput>();
	player2->health = std::make_shared<CHealth>();
}

// Main loop

void Game::Run() {
	while (Running) {
		entityManager.Update();
		sUserInput();

		sTransition();
		if (State == GameState::Playing) {
			sGravity();
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

       float dx = std::abs(p1.x - p2.x) + kCameraPadding;
		float dy = std::abs(p1.y - p2.y) + kCameraPadding;

		float aspect = static_cast<float>(WINDOW_HEIGHT) / WINDOW_WIDTH;

     float minViewW = kMinViewWidth;
		float minViewH = minViewW * aspect;

		if (dy > dx * aspect) {
			viewH = std::max(minViewH, dy);
			viewW = viewH / aspect;
		}
		else {
			viewW = std::max(minViewW, dx);
			viewH = viewW * aspect;
		}

       if (viewW > kMaxViewWidth) {
			viewW = kMaxViewWidth;
            viewH = kMaxViewWidth * aspect;
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
    gameView.setCenter(current + (target - current) * kCameraLerp);

	sf::Vector2f currentSize = gameView.getSize();
	sf::Vector2f targetSize = { viewW, viewH };
  gameView.setSize(currentSize + (targetSize - currentSize) * kCameraLerp);

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
                  AdvanceMenuOption(SelectedOption);
				}
				if (kp->code == sf::Keyboard::Key::Down ||
					kp->code == sf::Keyboard::Key::S) {
                  AdvanceMenuOption(SelectedOption);
				}
				if (kp->code == sf::Keyboard::Key::Enter ||
					kp->code == sf::Keyboard::Key::Space) {
					if (SelectedOption == 0) {
						State = GameState::Playing;

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

                  PushMusic(kMenuMusicPath);
				}
				return;
			}
			if (State == GameState::GameWon) {
				if (kp->code == sf::Keyboard::Key::Enter ||
					kp->code == sf::Keyboard::Key::Space ||
					kp->code == sf::Keyboard::Key::Escape) {

					State = GameState::StartMenu;
					SelectedOption = 0;

                   levelQueue.enqueue(kLevel3Path);
				}
				return;
			}

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
						ent->door->isOpen ? kDoorOpenColor : kDoorColor
					);
				}
			}
		}

		if (button->sprite) {
        button->sprite->getShape().setFillColor(
			inter->isPressed ? kButtonPressedColor : kButtonReleasedColor
		);
		}
	}
}


void Game::RenderStartMenu() {
	const float cx = WINDOW_WIDTH * 0.5f;
	const float cy = WINDOW_HEIGHT * 0.5f;

	sf::Text title(font), opt0(font), opt1(font);

 title.setCharacterSize(kMenuTitleSize);
	title.setFillColor(sf::Color::White);
 title.setString(kMenuTitleText);
	title.setPosition({ cx - title.getLocalBounds().size.x * 0.5f, cy - kMenuTitleOffset });

  opt0.setCharacterSize(kMenuOptionSize);
	opt0.setString(kMenuPlayText);
	opt0.setFillColor(SelectedOption == 0 ? sf::Color::Yellow : kMenuInactiveColor);
	opt0.setPosition({ cx - opt0.getLocalBounds().size.x * 0.5f, cy });

  opt1.setCharacterSize(kMenuOptionSize);
	opt1.setString(kMenuQuitText);
	opt1.setFillColor(SelectedOption == 1 ? sf::Color::Yellow : kMenuInactiveColor);
	opt1.setPosition({ cx - opt1.getLocalBounds().size.x * 0.5f, cy + kMenuOptionOffset });

	sf::Text arrow(font);
 arrow.setCharacterSize(kMenuArrowSize);
	arrow.setFillColor(sf::Color::Yellow);
   arrow.setString(kMenuArrowText);
	float arrowY = (SelectedOption == 0) ? cy : cy + kMenuOptionOffset;
	arrow.setPosition({ cx - kMenuArrowOffset, arrowY });

	window.draw(title);
	window.draw(opt0);
	window.draw(opt1);
	window.draw(arrow);
}

void Game::RenderGameOver() {
	const float cx = WINDOW_WIDTH * 0.5f;
	const float cy = WINDOW_HEIGHT * 0.5f;

	sf::Text over(font), sub(font), hint(font);

  over.setCharacterSize(kGameOverTitleSize);
	over.setFillColor(kGameOverTitleColor);
	over.setString(kGameOverTitleText);
	over.setPosition({ cx - over.getLocalBounds().size.x * 0.5f, cy - kGameOverTitleOffset });

   sub.setCharacterSize(kGameOverSubtitleSize);
	sub.setFillColor(sf::Color::White);
 sub.setString(kGameOverSubtitleText);
	sub.setPosition({ cx - sub.getLocalBounds().size.x * 0.5f, cy });

  hint.setCharacterSize(kGameOverHintSize);
	hint.setFillColor(kHudHintColor);
	hint.setString(kGameOverHintText);
	hint.setPosition({ cx - hint.getLocalBounds().size.x * 0.5f, cy + kGameOverHintOffset });

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
			t->coyoteFrames = kCoyoteFrames;
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
			t->JumpBufferFrames = kJumpBufferFrames;
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
				Vec2(t->position.x, t->position.y + e->boundingBox->halfSize.y), kJumpDustCount
			);

			jumpSound.play();
		}

		bool pushingLeft = in->left && !in->right;
		bool pushingRight = in->right && !in->left;

     bool turningLeft = pushingLeft && t->velocity.x > kTurnThreshold;
		bool turningRight = pushingRight && t->velocity.x < -kTurnThreshold;
		bool turning = turningLeft || turningRight;

		if (turning) {
			t->velocity.x *= TURN_FRICTION;

           if (std::abs(t->velocity.x) > kSkidSpeedThreshold) {
				Vec2 dustPos = Vec2(t->position.x, t->position.y + e->boundingBox->halfSize.y);
				float skidDir = (t->velocity.x > 0.0f) ? 1.0f : -1.0f;
                spawnDustParticles(dustPos, kSkidParticleCount, skidDir);
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

          if (std::abs(t->velocity.x) < kStopVelocityThreshold) {
				t->velocity.x = 0.0f;
			}
		}

		t->velocity.x = std::clamp(t->velocity.x, -MAX_MOVE_SPEED, MAX_MOVE_SPEED);

		t->position.x += t->velocity.x;
		t->position.y += t->velocity.y;
	}
}
// Handles collisions between players, tiles, and moving platforms.
void Game::sCollision() {

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

		for (auto& geo : solidEntities) {
			if (!geo->transform || !geo->boundingBox) { continue; }
			const float geoHW = geo->boundingBox->halfSize.x;
			const float geoHH = geo->boundingBox->halfSize.y;
			const float geoX = geo->transform->position.x;
			const float geoY = geo->transform->position.y;

			if (std::abs(e->transform->position.x - geoX) < hw + geoHW &&
				std::abs(e->transform->position.y - geoY) < hh + geoHH) {

				float overlapX = (hw + geoHW) - std::abs(e->transform->position.x - geoX);
				float overlapY = (hh + geoHH) - std::abs(e->transform->position.y - geoY);

				if (overlapX < overlapY) {
					if (e->transform->position.x < geoX) {
						e->transform->position.x -= overlapX;
					}
					else {
						e->transform->position.x += overlapX;
					}
					e->transform->velocity.x = 0.0f;
				}
				else {
					if (e->transform->position.y < geoY) {
						e->transform->position.y -= overlapY;

                   if (!e->transform->onGround && e->transform->velocity.y > kLandingVelocityThreshold) {
							spawnDustParticles(Vec2(e->transform->position.x,
                         e->transform->position.y + e->boundingBox->halfSize.y), kLandingDustCount);
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

              if (overlapY < overlapX + kPlayerOverlapBias) {

					if (dy < 0.0f) {
						e->transform->position.y -= overlapY;
						e->transform->onGround = true;
                     e->transform->coyoteFrames = kCoyoteFrames;

						if (e->transform->velocity.y > 0.0f) {
							e->transform->velocity.y = (Other->transform->velocity.y < 0.0f) ? Other->transform->velocity.y : 0.0f;
						}

						e->transform->position.x += Other->transform->velocity.x;

					}
					else {
						Other->transform->position.y -= overlapY;
						Other->transform->onGround = true;
                     Other->transform->coyoteFrames = kCoyoteFrames;

						if (Other->transform->velocity.y > 0.0f) {
							Other->transform->velocity.y = (e->transform->velocity.y < 0.0f) ? e->transform->velocity.y : 0.0f;
						}

						Other->transform->position.x += e->transform->velocity.x;
					}

				}
				else {

					if (dx < 0.0f) {
                        e->transform->position.x -= overlapX * kPlayerPushFactor;
						Other->transform->position.x += overlapX * kPlayerPushFactor;
					}
					else {
                        e->transform->position.x += overlapX * kPlayerPushFactor;
						Other->transform->position.x -= overlapX * kPlayerPushFactor;
					}

                   e->transform->velocity.x *= kPlayerPushFactor;
					Other->transform->velocity.x *= kPlayerPushFactor;
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

     p->age += kParticleAgeStep;

		if (p->age >= p->lifetime) {
			e->Destroy();
			continue;
		}

     t->velocity.y += kParticleGravity;
		t->position.x += t->velocity.x;
		t->position.y += t->velocity.y;

		sf::Color c = p->color;
      c.a = static_cast<std::uint8_t>(p->alpha() * kParticleAlphaScale);
		e->sprite->getShape().setFillColor(c);
	}
}

void Game::sHealth() {
	for (auto& e : entityManager.GetEntities("Player")) {
		if (!e->health || !e->transform) { continue; }

        if (e->transform->position.y > WINDOW_HEIGHT + kFallDeathOffset) {
			e->health->lives--;

			if (e->health->lives <= 0) {
				State = GameState::GameOver;
              PushMusic(kGameOverMusicPath);
			}
			else {
				StartRespawn(e->transform->position);
			}
			return;
		}
	}
}

void Game::RenderHud() {
	sf::Text p1Text(font), p2Text(font);
    p1Text.setCharacterSize(kHudTextSize);
	p2Text.setCharacterSize(kHudTextSize);
	p1Text.setFillColor(sf::Color::Cyan);
	p2Text.setFillColor(sf::Color::White);

	p1Text.setString("P1:");
	p2Text.setString("P2:");

 p1Text.setPosition({ kHudTextOffsetX, kHudP1OffsetY });
	p2Text.setPosition({ kHudTextOffsetX, kHudP2OffsetY });

	window.draw(p1Text);
	window.draw(p2Text);

	sf::Sprite p1Heart(p1HeartTex);
	sf::Sprite p2Heart(p2HeartTex);

 p1Heart.setScale({ kHeartScale, kHeartScale });
	p2Heart.setScale({ kHeartScale, kHeartScale });

	for (int i = 0; i < player1->health->lives; i++) {
      p1Heart.setPosition({ p1Text.getPosition().x + kHeartStartOffsetX + (i * kHeartSpacing), kHudP1OffsetY });
		window.draw(p1Heart);
	}

	for (int i = 0; i < player2->health->lives; i++) {
      p2Heart.setPosition({ p2Text.getPosition().x + kHeartStartOffsetX + (i * kHeartSpacing), kHudP2OffsetY });
		window.draw(p2Heart);
	}
}

void Game::sRender() {
    window.clear(kBackgroundClearColor);

	window.setView(window.getDefaultView());

	if (hasBackground) {
		backgroundSprite.setPosition({ 0.f, 0.f });
		window.draw(backgroundSprite);
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
        wipeCircle.setOutlineThickness(kWipeOutlineThickness);

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

         if (std::abs(playerBottom - platTop) < kPlatformSnapThreshold && playerX > platLeft && playerX < platRight) {
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

      if (dist < kPatrolTargetThreshold) {
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

        float cosHalf = std::cos(sight->halfAngleDeg * kPi / 180.0f);

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
                  PushMusic(kGameOverMusicPath);
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

void Game::StartRespawn(Vec2 focusPoint) {
	if (State == GameState::RespawnFadeOut || State == GameState::RespawnFadeIn) { return; }

	State = GameState::RespawnFadeOut;
	transitionCenter = focusPoint;
 transitionRadius = kTransitionRadiusStart;

	isLoadingNextLevel = false;
}

void Game::ApplyReset() {
	player1->transform->position = P1_SPAWN;
	player1->transform->velocity = { 0.0f, 0.0f };
	player2->transform->position = P2_SPAWN;
	player2->transform->velocity = { 0.0f, 0.0f };

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

       if (playersAtExit >= kRequiredPlayersAtExit) {
			StartWipe(exit->transform->position, true);
		}
	}
}
void Game::LoadNextLevel() {
	if (levelQueue.isEmpty()) {
		State = GameState::GameWon;

      PushMusic(kMenuMusicPath);
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
 transitionRadius = kTransitionRadiusStart;

	isLoadingNextLevel = advancingLevel;
}

void Game::sTransition() {
    float fadeSpeed = kTransitionFadeSpeed;

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
      if (transitionRadius >= kTransitionRadiusStart) {
			State = GameState::Playing;
		}
	}
}

void Game::PushMusic(const std::string& path) {
	if (!musicStack.isEmpty() && musicStack.top() == path) { return; }

	musicStack.push(path);

	bgMusic.stop();
   if (!PlayBackgroundTrack(bgMusic, path)) {
		std::cerr << "Warning: Could not load music track: " << path << std::endl;
	}
}

void Game::PopMusic() {
	if (!musicStack.isEmpty()) {
		musicStack.pop();
	}

	bgMusic.stop();

	if (!musicStack.isEmpty()) {
		std::string previousTrack = musicStack.top();
      PlayBackgroundTrack(bgMusic, previousTrack);
	}
}

sf::Texture& Game::getTexture(const std::string& name) {
	auto it = textureCache.find(name);
	if (it != textureCache.end()) {
		return it->second;
	}

 std::string path = std::string(kTexturesDirectory) + name + kTextureExtension;

	if (!textureCache[name].loadFromFile(path)) {
		std::cerr << "!!! ERROR: Could not find " << path << " !!!" << std::endl;

		sf::Image pinkImage;
     pinkImage.resize({ kFallbackTextureSize, kFallbackTextureSize }, sf::Color::Magenta);

		bool fallbackLoaded = textureCache[name].loadFromImage(pinkImage);
		if (!fallbackLoaded) {
			std::cerr << "Failed to generate fallback texture!" << std::endl;
		}
	}

	textureCache[name].setSmooth(false);
	return textureCache[name];
}
void Game::RenderGameWon() {
	const float cx = WINDOW_WIDTH * 0.5f;
	const float cy = WINDOW_HEIGHT * 0.5f;

	sf::Text over(font), sub(font), hint(font);

  over.setCharacterSize(kGameWonTitleSize);
	over.setFillColor(kGameWonTitleColor);
	over.setString(kGameWonTitleText);
	over.setPosition({ cx - over.getLocalBounds().size.x * 0.5f, cy - kGameWonTitleOffset });

   sub.setCharacterSize(kGameWonSubtitleSize);
	sub.setFillColor(sf::Color::White);
  sub.setString(kGameWonSubtitleText);
	sub.setPosition({ cx - sub.getLocalBounds().size.x * 0.5f, cy });

  hint.setCharacterSize(kGameWonHintSize);
	hint.setFillColor(kHudHintColor);
	hint.setString(kGameWonHintText);
	hint.setPosition({ cx - hint.getLocalBounds().size.x * 0.5f, cy + kGameWonHintOffset });

	window.draw(over);
	window.draw(sub);
	window.draw(hint);
}