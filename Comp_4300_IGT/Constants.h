#pragma once
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <cstdint>
#include <string>

namespace GameConstants {
// Window & frame
inline constexpr const char* kLevel1Path = "level1.txt";
inline constexpr const char* kLevel2Path = "Level2.txt";
inline constexpr const char* kLevel3Path = "Level3.txt";
inline constexpr const char* kFontPath = "Fonts/Coolvetica Rg.otf";
inline constexpr const char* kWindowTitle = "2 bhai 2no Tabahi";
inline constexpr const char* kTexturesDirectory = "Textures/";
inline constexpr const char* kTextureExtension = ".png";
inline constexpr unsigned int kFallbackTextureSize = 2;
inline constexpr unsigned int kFrameLimit = 60;

// Physics
inline constexpr float kPlayerOverlapBias = 16.0f;
inline constexpr float kPlayerPushFactor = 0.5f;
inline constexpr float kPlatformSnapThreshold = 3.0f;

// Camera
inline constexpr float kCameraPadding = 120.0f;
inline constexpr float kMinViewWidth = 800.0f;
inline constexpr float kMaxViewWidth = 2200.0f;
inline constexpr float kCameraLerp = 0.1f;

// Dust particles
inline constexpr float kDustBiasMultiplier = 1.5f;
inline constexpr int kDustRandRangeX = 200;
inline constexpr int kDustRandOffsetX = 100;
inline constexpr float kDustRandDivisor = 100.0f;
inline constexpr int kDustRandRangeY = 150;
inline constexpr int kDustRandOffsetY = 30;
inline constexpr float kDustSize = 4.0f;
inline constexpr float kDustLifetime = 18.0f;
inline const sf::Color kDustColor(200, 170, 120);
inline constexpr float kParticleAgeStep = 1.0f;
inline constexpr float kParticleGravity = 0.08f;
inline constexpr std::uint8_t kParticleAlphaScale = 255;

// Entity sizes & colors
inline constexpr float kEnemyWidth = 40.0f;
inline constexpr float kEnemyHeight = 48.0f;
inline const sf::Color kEnemyColor(220, 120, 0);
inline const sf::Color kButtonColor(0, 0, 255);
inline const sf::Color kButtonPressedColor(0, 200, 100);
inline const sf::Color kButtonReleasedColor(200, 100, 0);
inline const sf::Color kDoorColor(80, 60, 40);
inline const sf::Color kDoorOpenColor(20, 20, 20);
inline const sf::Color kPlatformColor(60, 100, 160);
inline constexpr float kCheckpointSize = 32.0f;
inline const sf::Color kCheckpointColor(0, 180, 255);
inline const sf::Color kExitColor(255, 220, 0);
inline const sf::Color kGroundColor(100, 80, 60);
inline const sf::Color kPlayer1Color(50, 100, 200);
inline const sf::Color kPlayer2Color(200, 50, 50);

// HUD
inline constexpr const char* kPlayer1HeartsPath = "Player1hearts.png";
inline constexpr const char* kPlayer2HeartsPath = "Player2hearts.png";
inline constexpr unsigned int kHudTextSize = 24;
inline constexpr float kHudTextOffsetX = 20.0f;
inline constexpr float kHudP1OffsetY = 20.0f;
inline constexpr float kHudP2OffsetY = 60.0f;
inline constexpr float kHeartScale = 0.05f;
inline constexpr float kHeartSpacing = 40.0f;
inline constexpr float kHeartStartOffsetX = 60.0f;

// Menu
inline constexpr int kMenuOptionCount = 2;
inline constexpr unsigned int kMenuTitleSize = 52;
inline constexpr unsigned int kMenuOptionSize = 30;
inline constexpr unsigned int kMenuArrowSize = 30;
inline constexpr float kMenuTitleOffset = 140.0f;
inline constexpr float kMenuOptionOffset = 60.0f;
inline constexpr float kMenuArrowOffset = 120.0f;
inline constexpr const char* kMenuTitleText = "Do bhai dono Tabahi";
inline constexpr const char* kMenuPlayText = "Play";
inline constexpr const char* kMenuQuitText = "Quit";
inline constexpr const char* kMenuArrowText = ">";
inline const sf::Color kMenuInactiveColor(160, 160, 160);
inline constexpr const char* kMenuMusicPath = "menu.ogg";

// Game Over / Game Won screens
inline constexpr unsigned int kGameOverTitleSize = 56;
inline constexpr unsigned int kGameOverSubtitleSize = 28;
inline constexpr unsigned int kGameOverHintSize = 22;
inline constexpr float kGameOverTitleOffset = 120.0f;
inline constexpr float kGameOverHintOffset = 70.0f;
inline constexpr const char* kGameOverTitleText = "Game Over";
inline constexpr const char* kGameOverSubtitleText = "Both players ran out of lives";
inline constexpr const char* kGameOverHintText = "R  — Restart      Escape — Main Menu";
inline const sf::Color kGameOverTitleColor(220, 60, 60);
inline const sf::Color kHudHintColor(160, 160, 160);
inline constexpr unsigned int kGameWonTitleSize = 56;
inline constexpr unsigned int kGameWonSubtitleSize = 28;
inline constexpr unsigned int kGameWonHintSize = 22;
inline constexpr float kGameWonTitleOffset = 120.0f;
inline constexpr float kGameWonHintOffset = 70.0f;
inline constexpr const char* kGameWonTitleText = "VICTORY!";
inline constexpr const char* kGameWonSubtitleText = "DONO BHAI GAYE BILLO DE GHAR";
inline constexpr const char* kGameWonHintText = "Press Enter or Space to return to Main Menu";
inline const sf::Color kGameWonTitleColor(255, 215, 0);
inline constexpr const char* kGameOverMusicPath = "gameover.ogg";

// Audio volumes
inline constexpr const char* kJumpSoundPath = "jump.wav";
inline constexpr const char* kButtonSoundPath = "button.wav";
inline constexpr float kJumpVolume = 50.0f;
inline constexpr float kButtonVolume = 80.0f;
inline constexpr float kMusicVolume = 30.0f;

// Gameplay tuning (jump buffer, coyote frames, etc.)
inline constexpr int kJumpBufferFrames = 8;
inline constexpr int kCoyoteFrames = 8;
inline constexpr float kTurnThreshold = 0.5f;
inline constexpr float kSkidSpeedThreshold = 1.5f;
inline constexpr int kSkidParticleCount = 5;
inline constexpr float kStopVelocityThreshold = 0.15f;
inline constexpr int kJumpDustCount = 8;
inline constexpr int kLandingDustCount = 6;
inline constexpr float kLandingVelocityThreshold = 1.0f;
inline constexpr float kFallDeathOffset = 100.0f;
inline constexpr int kRequiredPlayersAtExit = 2;
inline constexpr float kPatrolTargetThreshold = 4.0f;

// Transitions & rendering
inline const sf::Color kBackgroundClearColor(30, 30, 50);
inline constexpr float kWipeOutlineThickness = 4000.0f;
inline constexpr float kTransitionRadiusStart = 3000.0f;
inline constexpr float kTransitionFadeSpeed = 70.0f;
inline constexpr float kPi = 3.14159265f;

inline void AdvanceMenuOption(int& option) {
option = (option + 1) % kMenuOptionCount;
}

inline bool PlayBackgroundTrack(sf::Music& music, const std::string& path) {
if (!music.openFromFile(path)) {
return false;
}
music.setLooping(true);
music.setVolume(kMusicVolume);
music.play();
return true;
}
} // namespace GameConstants
