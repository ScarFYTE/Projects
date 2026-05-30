#include "InputSystem.h"

#include "../Constants.h"
#include "../Entity.h"
#include "../LevelLoader.h"

#include <optional>

namespace InputSystem {
void sUserInput(sf::RenderWindow& window,
                bool& m_isRunning,
                GameState& m_state,
                int& m_selectedOption,
                EntityManager& entityManager,
                std::shared_ptr<Entity>& player1,
                std::shared_ptr<Entity>& player2,
                LevelQueue& levelQueue,
                std::string& currentLevelPath,
                Vec2& p1Spawn,
                Vec2& p2Spawn,
                bool& m_hasBackground,
                std::string& currentBackgroundKey,
                std::map<std::string, sf::Texture>& textureCache,
                sf::View& gameView,
                AudioManager& audioManager,
                void (*spawnPlayers)(EntityManager&, std::shared_ptr<Entity>&, std::shared_ptr<Entity>&, const Vec2&, const Vec2&)) {
    while (const std::optional<sf::Event> event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            m_isRunning = false;
        }

        if (const auto* kp = event->getIf<sf::Event::KeyPressed>()) {
            if (m_state == GameState::StartMenu) {
                if (kp->code == sf::Keyboard::Key::Up || kp->code == sf::Keyboard::Key::W) {
                    GameConstants::AdvanceMenuOption(m_selectedOption);
                }
                if (kp->code == sf::Keyboard::Key::Down || kp->code == sf::Keyboard::Key::S) {
                    GameConstants::AdvanceMenuOption(m_selectedOption);
                }
                if (kp->code == sf::Keyboard::Key::Enter || kp->code == sf::Keyboard::Key::Space) {
                    if (m_selectedOption == 0) {
                        m_state = GameState::Playing;
                        audioManager.popMusic();
                    }
                    else {
                        m_isRunning = false;
                    }
                }
                return;
            }

            if (m_state == GameState::GameOver) {
                if (kp->code == sf::Keyboard::Key::R) {
                    audioManager.popMusic();
                    entityManager = EntityManager();

                    auto result = LevelLoader::load(currentLevelPath, entityManager, textureCache);
                    p1Spawn = result.p1Spawn;
                    p2Spawn = result.p2Spawn;
                    m_hasBackground = result.hasBackground;
                    currentBackgroundKey = result.backgroundKey;
                    if (!result.musicTrack.empty()) {
                        audioManager.pushMusic(result.musicTrack, GameConstants::kMusicVolume);
                    }

                    spawnPlayers(entityManager, player1, player2, p1Spawn, p2Spawn);
                    entityManager.Update();
                    gameView = window.getDefaultView();
                    m_state = GameState::Playing;
                }
                if (kp->code == sf::Keyboard::Key::Escape) {
                    m_state = GameState::StartMenu;
                    m_selectedOption = 0;
                    audioManager.pushMusic(GameConstants::kMenuMusicPath, GameConstants::kMusicVolume);
                }
                return;
            }

            if (m_state == GameState::GameWon) {
                if (kp->code == sf::Keyboard::Key::Enter ||
                    kp->code == sf::Keyboard::Key::Space ||
                    kp->code == sf::Keyboard::Key::Escape) {
                    m_state = GameState::StartMenu;
                    m_selectedOption = 0;
                    levelQueue.enqueue(GameConstants::kLevel3Path);
                }
                return;
            }

            switch (kp->code) {
            case sf::Keyboard::Key::W: player1->input->jump = true; break;
            case sf::Keyboard::Key::A: player1->input->left = true; break;
            case sf::Keyboard::Key::D: player1->input->right = true; break;
            case sf::Keyboard::Key::E: player1->input->interact = true; break;

            case sf::Keyboard::Key::Up: player2->input->jump = true; break;
            case sf::Keyboard::Key::Left: player2->input->left = true; break;
            case sf::Keyboard::Key::Right: player2->input->right = true; break;
            case sf::Keyboard::Key::RShift: player2->input->interact = true; break;

            default: break;
            }
        }

        if (const auto* kr = event->getIf<sf::Event::KeyReleased>()) {
            switch (kr->code) {
            case sf::Keyboard::Key::A: player1->input->left = false; break;
            case sf::Keyboard::Key::D: player1->input->right = false; break;
            case sf::Keyboard::Key::W: player1->input->jump = false; break;
            case sf::Keyboard::Key::E: player1->input->interact = false; break;

            case sf::Keyboard::Key::Left: player2->input->left = false; break;
            case sf::Keyboard::Key::Right: player2->input->right = false; break;
            case sf::Keyboard::Key::Up: player2->input->jump = false; break;
            case sf::Keyboard::Key::RShift: player2->input->interact = false; break;

            default: break;
            }
        }
    }
}
} // namespace InputSystem
