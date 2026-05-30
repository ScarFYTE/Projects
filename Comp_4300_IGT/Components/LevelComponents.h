#pragma once
#include "../Vec2.h"
#include <string>

class CInteractable {
public:
    std::string linkedTag;
    bool isPressed = false;
    bool requiresStay = true;
    bool requiresInput = false;
};

class CDoor {
public:
    std::string linkTag;
    bool isOpen = false;

    CDoor() {}
};

class CMovingPlatform {
public:
    Vec2 posA;
    Vec2 posB;
    bool triggered = false;
    float speed = 2.0f;

    int requiredTriggers = 1;
    int currentTriggers = 0;
};

class CCheckpoint {
public:
    bool activated = false;
    Vec2 p1Spawn;
    Vec2 p2Spawn;
};

class CExit {
public:
    int playersInside = 0;
};
