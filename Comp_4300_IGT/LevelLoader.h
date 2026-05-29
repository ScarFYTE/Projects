#pragma once
#include "EntityManager.h"
#include "Vec2.h"
#include <SFML/Graphics.hpp>
#include <map>
#include <string>

class LevelLoader {
public:
struct LevelResult {
Vec2 p1Spawn;
Vec2 p2Spawn;
bool hasBackground = false;
std::string backgroundKey;
std::string musicTrack;
};

static LevelResult load(
const std::string& path,
EntityManager& em,
std::map<std::string, sf::Texture>& textureCache
);
};
