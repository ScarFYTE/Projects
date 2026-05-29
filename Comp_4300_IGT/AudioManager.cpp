#include "AudioManager.h"
#include <iostream>

bool AudioManager::init(const std::string& jumpPath,
const std::string& buttonPath,
float jumpVol, float buttonVol) {
bool jumpLoaded = jumpBuffer.loadFromFile(jumpPath);
if (jumpLoaded) {
jumpSound.setVolume(jumpVol);
}
else {
std::cerr << "Failed to load " << jumpPath << std::endl;
}

bool buttonLoaded = buttonBuffer.loadFromFile(buttonPath);
if (buttonLoaded) {
buttonSound.setVolume(buttonVol);
}
else {
std::cerr << "Failed to load " << buttonPath << std::endl;
}

return jumpLoaded && buttonLoaded;
}

void AudioManager::pushMusic(const std::string& path, float volume) {
if (!musicStack.empty() && musicStack.top() == path) { return; }

musicStack.push(path);

bgMusic.stop();
if (!bgMusic.openFromFile(path)) {
std::cerr << "Warning: Could not load music track: " << path << std::endl;
return;
}
bgMusic.setLooping(true);
bgMusic.setVolume(volume);
bgMusic.play();
}

void AudioManager::popMusic() {
float previousVolume = bgMusic.getVolume();

if (!musicStack.empty()) {
musicStack.pop();
}

bgMusic.stop();

if (!musicStack.empty()) {
if (!bgMusic.openFromFile(musicStack.top())) {
std::cerr << "Warning: Could not load music track: " << musicStack.top() << std::endl;
return;
}
bgMusic.setLooping(true);
bgMusic.setVolume(previousVolume);
bgMusic.play();
}
}

std::string AudioManager::currentTrack() const {
return musicStack.empty() ? "" : musicStack.top();
}

bool AudioManager::isMusicEmpty() const {
return musicStack.empty();
}
