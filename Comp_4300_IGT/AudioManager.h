#pragma once
#include <SFML/Audio.hpp>
#include <stack>
#include <string>

class AudioManager {
std::stack<std::string> musicStack;
sf::Music bgMusic;
sf::SoundBuffer jumpBuffer;
sf::SoundBuffer buttonBuffer;
public:
sf::Sound jumpSound{ jumpBuffer };
sf::Sound buttonSound{ buttonBuffer };

bool init(const std::string& jumpPath,
  const std::string& buttonPath,
  float jumpVol, float buttonVol);

void pushMusic(const std::string& path, float volume);
void popMusic();
std::string currentTrack() const;
bool isMusicEmpty() const;
};
