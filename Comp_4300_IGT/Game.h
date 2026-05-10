#pragma once
#include "EntityManager.h"
#include "Entity.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
enum class GameState {
	StartMenu,
	Playing,
	GameOver,
	RespawnFadeOut,
	RespawnFadeIn
};

// --- CUSTOM LINKED LIST QUEUE ---
struct LevelNode {
	std::string levelPath;
	LevelNode* next;
	LevelNode(std::string path) : levelPath(path), next(nullptr) {}
};

class LevelQueue {
private:
	LevelNode* frontNode;
	LevelNode* rearNode;

public:
	LevelQueue() : frontNode(nullptr), rearNode(nullptr) {}

	~LevelQueue() {
		while (!isEmpty()) { dequeue(); }
	}

	void enqueue(const std::string& path) {
		LevelNode* newNode = new LevelNode(path);
		if (rearNode == nullptr) {
			frontNode = rearNode = newNode;
			return;
		}
		rearNode->next = newNode;
		rearNode = newNode;
	}

	void dequeue() {
		if (frontNode == nullptr) return;
		LevelNode* temp = frontNode;
		frontNode = frontNode->next;
		if (frontNode == nullptr) { rearNode = nullptr; }
		delete temp;
	}

	std::string front() const {
		if (frontNode != nullptr) { return frontNode->levelPath; }
		return "";
	}

	bool isEmpty() const {
		return frontNode == nullptr;
	}
};

// --- CUSTOM LINKED LIST STACK (For Audio States) ---
struct MusicNode {
	std::string trackPath;
	MusicNode* next;
	MusicNode(std::string path) : trackPath(path), next(nullptr) {}
};

class MusicStack {
private:
	MusicNode* topNode;

public:
	MusicStack() : topNode(nullptr) {}

	~MusicStack() {
		clear();
	}

	void push(const std::string& path) {
		MusicNode* newNode = new MusicNode(path);
		newNode->next = topNode;
		topNode = newNode;
	}

	void pop() {
		if (topNode == nullptr) return;
		MusicNode* temp = topNode;
		topNode = topNode->next;
		delete temp;
	}

	std::string top() const {
		if (topNode != nullptr) { return topNode->trackPath; }
		return "";
	}

	bool isEmpty() const {
		return topNode == nullptr;
	}

	void clear() {
		while (!isEmpty()) { pop(); }
	}

	void print() {
		if (isEmpty()) {
			std::cout << "Music Stack: [Empty]" << std::endl;
			return;
		}
		std::cout << "Music Stack (top to bottom):" << std::endl;
		MusicNode* current = topNode;
		int index = 0;
		while (current != nullptr) {
			std::cout << "  [" << index << "] " << current->trackPath << std::endl;
			current = current->next;
			index++;
		}
	}
};


class Game {
	// -----------------------------------------------------------------------
	// Constants
	// -----------------------------------------------------------------------
	static constexpr unsigned int WINDOW_WIDTH  = 1280;
	static constexpr unsigned int WINDOW_HEIGHT = 720;
	static constexpr float GRAVITY        =  0.5f;
	static constexpr float MAX_FALL_SPEED =  15.0f;
	static constexpr float JUMP_VELOCITY  = -8.0f;
	static constexpr float MOVE_SPEED     =  1.5f;
	static constexpr float PLAYER_W       =  24.0f;
	static constexpr float PLAYER_H       =  24.0f;
	static constexpr float GROUND_H       =  20.0f;
	static constexpr float ACCELERATION = 0.8f;  // speed gained per frame
	static constexpr float FRICTION = 0.75f; // multiplier when no input (0-1, lower = more friction)
	static constexpr float TURN_FRICTION = 0.55f; // multiplier on direction change (faster stop)
	static constexpr float MAX_MOVE_SPEED = 7.0f;  // replaces MOVE_SPEED for horizontal cap
	Vec2 P1_SPAWN = Vec2(WINDOW_WIDTH * 0.25f, WINDOW_HEIGHT - GROUND_H - PLAYER_H * 0.5f);
	Vec2 P2_SPAWN = Vec2(WINDOW_WIDTH * 0.5f, WINDOW_HEIGHT - GROUND_H - PLAYER_H * 0.5f);

	



	// -----------------------------------------------------------------------
	// Members
	// -----------------------------------------------------------------------

	// --- AUDIO ---
	sf::Music bgMusic;

	sf::SoundBuffer jumpBuffer;
	sf::Sound jumpSound{ jumpBuffer }; // SFML 3 Fix: Attach buffer immediately

	sf::SoundBuffer buttonBuffer;
	sf::Sound buttonSound{ buttonBuffer }; // SFML 3 Fix: Attach buffer immediately

	// --- NEW STACK MANAGER ---
	MusicStack musicStack;
	void PushMusic(const std::string& filepath);
	void PopMusic();

	sf::RenderWindow window;
	sf::Font font;
	sf::Texture p1HeartTex; // NEW
	sf::Texture p2HeartTex; // NEW

	LevelQueue  levelQueue;
	std::string currentLevelPath;    
	bool isLoadingNextLevel = false;

	EntityManager    entityManager;
	bool Running      = true;
	int  currentFrame = 0;

	float transitionRadius = 3000.0f;
	Vec2  transitionCenter = { 0.0f, 0.0f };

	sf::View gameView;
	float baseZoom = 1.0f;

	std::shared_ptr<Entity> player1;  // WASD
	std::shared_ptr<Entity> player2;  // Arrow keys

	GameState State = GameState::StartMenu;
	int SelectedOption = 0; // 0 = play, 1 = exit

	// -----------------------------------------------------------------------
	// Internal helpers
	// -----------------------------------------------------------------------
	void init();
	void loadConfig(const std::string& path);
	void spawnGround();
	void spawnPlayers();
	void spawnDustParticles(Vec2 position, int count = 6, float directionX = 0.0f);

	void RenderStartMenu();
	void RenderHud();
	void RenderGameOver();
	void StartRespawn(Vec2 focusPoint);
	void ApplyReset();
	void sTransition();

	//Levels
	void LoadNextLevel();
	void sWinCondition();
	void StartWipe(Vec2 focusPoint, bool advancingLevel = false);


	// Systems
	void sUserInput();
	void sGravity();
	void sMovement();
	void sCollision();
	void sRender();
	void sCamera();
	void sParticle();
	void sHealth();
	void sMovingPlatform();
	void sInteract();
	void sPatrol();
	void sSight();


public:


	Game();
	void Run();
};