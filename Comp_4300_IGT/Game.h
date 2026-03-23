#pragma once
#include"EntityManager.h"
#include"Entity.h"

#include<SFML/Graphics.hpp>

struct PlayerConfig {};
struct EnemyConfig {};
struct BulletConfig {};

class Game {
	sf::RenderWindow window;
	EntityManager entityManager;
	sf::Font font;
	sf::Text Text;
	PlayerConfig playerConfig;
	EnemyConfig enemyConfig;
	BulletConfig bulletConfig;

	int score = 0;
	int currentFrame = 0;
	int LastEnemySpawnTime = 0;
	bool Paused = false;
	bool Running = true;

	std::shared_ptr<Entity> player;

	void init(const std::string& config);
	void SetPaused(bool Paused);

	void sMovement();
	void sUserInput();
	void sRender();
	void sEnemySpawn();
	void sCollision();

	void spawnPlayer();
	void spawnEnemy();
	void spawnSmallEnemy(std::shared_ptr<Entity> entity);
	void spawnBullet(std::shared_ptr<Entity> entity,const Vec2 & mousepos);
	void spawnSpecialWeapon(std::shared_ptr<Entity> entity);

public:
	Game(const std::string& config);
	
	void Run();

};