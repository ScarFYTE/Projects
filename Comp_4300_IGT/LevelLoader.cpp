#include "LevelLoader.h"
#include "Components.h"
#include "Constants.h"

#include <fstream>
#include <iostream>
#include <sstream>

LevelLoader::LevelResult LevelLoader::load(
    const std::string& path,
    EntityManager& em,
    std::map<std::string, sf::Texture>& textureCache
) {
    LevelResult result;
    result.p1Spawn = Vec2(GameConstants::kWindowWidth * 0.25f,
                          GameConstants::kWindowHeight - GameConstants::kGroundH - GameConstants::kPlayerH * 0.5f);
    result.p2Spawn = Vec2(GameConstants::kWindowWidth * 0.5f,
                          GameConstants::kWindowHeight - GameConstants::kGroundH - GameConstants::kPlayerH * 0.5f);

    auto getTexture = [&](const std::string& name) -> sf::Texture& {
        auto it = textureCache.find(name);
        if (it != textureCache.end()) {
            return it->second;
        }

        std::string texPath = std::string(GameConstants::kTexturesDirectory) + name + GameConstants::kTextureExtension;
        if (!textureCache[name].loadFromFile(texPath)) {
            std::cerr << "!!! ERROR: Could not find " << texPath << " !!!" << std::endl;

            sf::Image pinkImage;
            pinkImage.resize({ GameConstants::kFallbackTextureSize, GameConstants::kFallbackTextureSize }, sf::Color::Magenta);
            textureCache[name].loadFromImage(pinkImage);
        }

        textureCache[name].setSmooth(false);
        return textureCache[name];
    };

    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Could not open config: " << path << std::endl;
        return result;
    }

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

            auto tile = em.AddEntity("Tile");
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
            result.p1Spawn = Vec2(p1x, p1y);
            result.p2Spawn = Vec2(p2x, p2y);
        }
        else if (type == "Enemy") {
            float x, y, wp1x, wp1y, wp2x, wp2y, speed, sightRange, sightAngle;
            iss >> x >> y >> wp1x >> wp1y >> wp2x >> wp2y >> speed >> sightRange >> sightAngle;

            auto enemy = em.AddEntity("Enemy");
            enemy->transform = std::make_shared<CTransform>(Vec2(x, y), Vec2(0, 0), 0.0f);
            enemy->boundingBox = std::make_shared<CBoundingBox>(GameConstants::kEnemyWidth, GameConstants::kEnemyHeight);
            enemy->sprite = std::make_shared<CSprite>(GameConstants::kEnemyWidth, GameConstants::kEnemyHeight, GameConstants::kEnemyColor);

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

            auto button = em.AddEntity("Button");
            button->transform = std::make_shared<CTransform>(Vec2(x, y), Vec2(0, 0), 0.0f);
            button->boundingBox = std::make_shared<CBoundingBox>(w, h);
            button->sprite = std::make_shared<CSprite>(w, h, GameConstants::kButtonColor);

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

            auto door = em.AddEntity("Door");
            door->transform = std::make_shared<CTransform>(Vec2(x, y), Vec2(0, 0), 0.0f);
            door->boundingBox = std::make_shared<CBoundingBox>(w, h);
            door->sprite = std::make_shared<CSprite>(w, h, GameConstants::kDoorColor);

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

            auto plat = em.AddEntity(tag);
            plat->transform = std::make_shared<CTransform>(Vec2(x, y), Vec2(0, 0), 0.0f);
            plat->boundingBox = std::make_shared<CBoundingBox>(w, h);
            plat->sprite = std::make_shared<CSprite>(w, h, GameConstants::kPlatformColor);

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

            auto cp = em.AddEntity("Checkpoint");
            cp->transform = std::make_shared<CTransform>(Vec2(x, y), Vec2(0, 0), 0.0f);
            cp->boundingBox = std::make_shared<CBoundingBox>(GameConstants::kCheckpointSize, GameConstants::kCheckpointSize);
            cp->sprite = std::make_shared<CSprite>(GameConstants::kCheckpointSize, GameConstants::kCheckpointSize, GameConstants::kCheckpointColor);

            auto c = std::make_shared<CCheckpoint>();
            c->p1Spawn = Vec2(p1sx, p1sy);
            c->p2Spawn = Vec2(p2sx, p2sy);
            cp->checkpoint = c;
        }
        else if (type == "Exit") {
            float x, y, w, h;
            iss >> x >> y >> w >> h;

            auto exit = em.AddEntity("Exit");
            exit->transform = std::make_shared<CTransform>(Vec2(x, y), Vec2(0, 0), 0.0f);
            exit->boundingBox = std::make_shared<CBoundingBox>(w, h);
            exit->sprite = std::make_shared<CSprite>(w, h, GameConstants::kExitColor);
            exit->exit_ = std::make_shared<CExit>();
        }
        else if (type == "Music") {
            std::string trackName;
            iss >> trackName;
            std::cout << "Music Loaded " << trackName << std::endl;
            result.musicTrack = trackName;
        }
        else if (type == "Background") {
            std::string bgName;
            iss >> bgName;

            if (!bgName.empty() && bgName.back() == '\r') {
                bgName.pop_back();
            }

            std::cout << "Attempting to load Background: " << bgName << std::endl;
            getTexture(bgName);
            result.hasBackground = true;
            result.backgroundKey = bgName;
        }
        else {
            std::cerr << "Warning: Unknown entity type in config: " << type << std::endl;
        }
    }

    return result;
}
