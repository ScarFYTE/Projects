#pragma once
#include "../Vec2.h"

class CHealth {
public:
    int maxLives;
    int lives;
    bool isDead = false;
    int respawnTimer = 0;
    Vec2 spawnPoint;

    CHealth(int lives, Vec2 spawnPoint)
        : maxLives(lives), lives(lives), spawnPoint(spawnPoint) {}
    CHealth() : maxLives(5), lives(5), spawnPoint(250, 250) {}

    bool isRespawning() const { return isDead && respawnTimer > 0; }
};
