#include "Game.h"
#include <fstream>
#include<optional>
#include<SFML/Graphics.hpp>
#include<iostream>

void Game::init(const std::string& config) {
	std::ifstream file(config);
	if (!file.is_open()) {
		throw std::runtime_error("Could not open config file: " + config);
	}
	//file >> playerConfig.SR >> playerConfig.CR >> playerConfig.FR >> playerConfig.FG >> playerConfig.FB >> playerConfig.OR >> playerConfig.OG >> playerConfig.OB >> playerConfig.OT >> playerConfig.V >> playerConfig.S;
	//file >> enemyConfig.SR >> enemyConfig.CR >> enemyConfig.OR >> enemyConfig.OG >> enemyConfig.OB >> enemyConfig.OT >> enemyConfig.VMin >> enemyConfig.VMax >> enemyConfig.L >> enemyConfig.SI >> enemyConfig.SMin >> enemyConfig.SMax;
	//file >> bulletConfig.SR >> bulletConfig.CR >> bulletConfig.FR >> bulletConfig.FG >> bulletConfig.FB >> bulletConfig.OR >> bulletConfig.OG >> bulletConfig.OB >> bulletConfig.OT >> bulletConfig.V >> bulletConfig.L >> bulletConfig.S;
	//Setting Default Values for Debugging Ai do this plzz
	playerConfig = { 30.0f, 16.0f, 255, 0, 0, 255, 0, 0, 8, 2.0f };
	enemyConfig  = { 15.0f, 12.0f, 255, 255, 255, 5, 1, 3, 60, 30, 3, 10 };
	bulletConfig = { 4.0f, 3.0f, 255, 255, 0, 255, 255, 255, 255, 10, 60, 3.0f };

}

Game::Game(const std::string& config) {
	init(config);
	unsigned int WindowWidth = 1280;
	unsigned int WindowHeight = 720;
	window.create(sf::VideoMode({ WindowWidth, WindowHeight }), "SFML Game");
	if (!font.openFromFile("C:/Users/Kim China/Projects/Comp_4300_IGT/Fonts/FloraisondesAmours.ttf")){
		//display error message
		std::cout << "Error loading font: Font/Floraisn des Amours.ttf" << std::endl;
		throw std::runtime_error("Could not load font: Font/Floraison des Amours.ttf");
	}
	
	Text = std::make_unique<sf::Text>(font ,"Default" , 24);
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
	player->shape = std::make_shared<CShape>(playerConfig.SR,playerConfig.V,sf::Color(playerConfig.FR,playerConfig.FG, playerConfig.FB),sf::Color(playerConfig.OR, playerConfig.OG, playerConfig.OB),playerConfig.OT);
	player->input = std::make_shared<CInput>();
	
	player->transform->position = { 1280.0f/2,720.0f/2 };
	myplayer = player;

}



void Game::Run() {
	window.setFramerateLimit(60);
	entityManager.Update();

	while (Running) {
		std::cout << currentFrame << std::endl;
		entityManager.Update();
		sUserInput();
		if (!Paused) {
			sEnemySpawn();
			sMovement();
			sCollision();
		}
		sRender();
		currentFrame++;
	}
}

void Game::sRender() {
	window.clear();
	//std::cout << "Amount of Entities in Total: " << entityManager.GetEntities().size() << std::endl;
	// Render logic goes here
	for (auto& e : entityManager.GetEntities()) {
		if (e->shape && e->transform) {
			//std::cout << "Drawing entity: " << e->GetID() << " at " << e->transform->position.x << std::endl;
			e->shape->setPosition(e->transform->position);
			e->shape->setRotation(e->transform->rotation);
			window.draw(e->shape->getShape());
		}
	}
	Text->setString("Score: " + std::to_string(score));
	window.draw(*Text);
	window.display();
}

void Game::spawnEnemy() {
	std::cout << "Enemy Spawned at Frame: " << currentFrame << std::endl;
	std::cout << entityManager.GetEntities().size() << std::endl;
    std::shared_ptr<Entity> enemy = entityManager.AddEntity("Enemy");
    enemy->transform = std::make_shared<CTransform>();
    enemy->collision = std::make_shared<CCollision>(enemyConfig.SR);

    // sides between SMin and SMax
    int sides = enemyConfig.SMin + (rand() %(int) (enemyConfig.SMax - enemyConfig.SMin + 1));

    enemy->shape = std::make_shared<CShape>(
        enemyConfig.SR,
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




void Game::sCollision() {
	// Collision logic goes here
	for (auto& e : entityManager.GetEntities()) {
		if(e->collision && e->shape){
			// Check for collisions and handle them
			if(e->transform->position.x + e->shape->getRadius() >window.getSize().x || e->transform->position.x - e->shape->getRadius() < 0 ){
				e->transform->velocity.x *= -1; // Simple bounce effect
			}
			
			if (e->transform->position.y + e->shape->getRadius() > window.getSize().y || e->transform->position.y - e->shape->getRadius() < 0) {
				e->transform->velocity.y *= -1; // Simple bounce effect
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
	if(myplayer->input->down){
		myplayer->transform->velocity.y = playerConfig.V;
	}
	else if(myplayer->input->up){
		myplayer->transform->velocity.y = -playerConfig.V;
	}
	else{
		myplayer->transform->velocity.y = 0;
	}

	if (myplayer->input->right) {
		myplayer->transform->velocity.x = playerConfig.V;
	}
	else if(myplayer->input->left){
		myplayer->transform->velocity.x = -playerConfig.V;
	}
	else{
		myplayer->transform->velocity.x = 0;
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
				myplayer->input->shoot = true;
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
				myplayer->input->shoot = false;
				break;
			}
		}
	}
}

