#include "AudioManager.h"
#include "Constants.h"

bool AudioManager::init(const std::string& jumpPath,
                        const std::string& buttonPath,
                        float jumpVol, float buttonVol) {
    bool ok = true;
    if (jumpBuffer.loadFromFile(jumpPath)) {
        jumpSound.setVolume(jumpVol);
    } else {
        ok = false;
    }

    if (buttonBuffer.loadFromFile(buttonPath)) {
        buttonSound.setVolume(buttonVol);
    } else {
        ok = false;
    }

    return ok;
}

void AudioManager::pushMusic(const std::string& path, float volume) {
    if (!musicStack.empty() && musicStack.top() == path) { return; }

    musicStack.push(path);
    bgMusic.stop();
    GameConstants::PlayBackgroundTrack(bgMusic, path, volume);
}

void AudioManager::popMusic() {
    if (!musicStack.empty()) {
        musicStack.pop();
    }

    bgMusic.stop();

    if (!musicStack.empty()) {
        GameConstants::PlayBackgroundTrack(bgMusic, musicStack.top());
    }
}

std::string AudioManager::currentTrack() const {
    return musicStack.empty() ? "" : musicStack.top();
}

bool AudioManager::isMusicEmpty() const {
    return musicStack.empty();
}
