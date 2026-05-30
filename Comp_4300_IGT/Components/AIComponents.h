#pragma once
#include "../Vec2.h"
#include <vector>

class CPatrol {
public:
    std::vector<Vec2> waypoints;
    int currentTarget = 0;
    float speed = 1.5f;
    bool facingRight = true;
};

class CSight {
public:
    float range = 100.0f;
    float halfAngleDeg = 35.0f;
};
