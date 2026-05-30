#include "InteractSystem.h"

#include "../AudioManager.h"
#include "../Constants.h"
#include "../Entity.h"

#include <cmath>

namespace InteractSystem {
void sInteract(EntityManager& entityManager,
               const std::shared_ptr<Entity>& player1,
               const std::shared_ptr<Entity>& player2,
               AudioManager& audioManager) {
    for (auto& player : entityManager.GetEntities("Player")) {
        if (!player->transform || !player->boundingBox) { continue; }

        if (player->input->interact) {
            for (auto& door : entityManager.GetEntities("Door")) {
                if (!door->door || !door->door->isOpen || !door->transform || !door->boundingBox) { continue; }

                float dx = std::abs(player->transform->position.x - door->transform->position.x);
                float dy = std::abs(player->transform->position.y - door->transform->position.y);

                if (dx < player->boundingBox->halfSize.x + door->boundingBox->halfSize.x &&
                    dy < player->boundingBox->halfSize.y + door->boundingBox->halfSize.y) {

                    for (auto& targetDoor : entityManager.GetEntities("Door")) {
                        if (targetDoor != door && targetDoor->door->linkTag == door->door->linkTag) {
                            player->transform->position = targetDoor->transform->position;
                            player->input->interact = false;
                            break;
                        }
                    }
                    break;
                }
            }
        }
    }

    for (auto& button : entityManager.GetEntities("Button")) {
        if (!button->interactable || !button->boundingBox || !button->transform) { continue; }

        auto& inter = button->interactable;
        bool wasPressed = inter->isPressed;

        bool anyOverlap = false;
        bool interactPressed = false;
        bool isHoldingInteract = false;

        for (auto& player : entityManager.GetEntities("Player")) {
            if (!player->transform || !player->boundingBox) { continue; }

            float dx = std::abs(player->transform->position.x - button->transform->position.x);
            float dy = std::abs(player->transform->position.y - button->transform->position.y);

            if (dx < player->boundingBox->halfSize.x + button->boundingBox->halfSize.x &&
                dy < player->boundingBox->halfSize.y + button->boundingBox->halfSize.y) {

                anyOverlap = true;

                if (player == player1 && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E)) {
                    isHoldingInteract = true;
                }
                if (player == player2 && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift)) {
                    isHoldingInteract = true;
                }

                if (player->input->interact) {
                    interactPressed = true;
                    player->input->interact = false;
                }
            }
        }

        if (inter->requiresInput && inter->requiresStay) {
            inter->isPressed = (anyOverlap && isHoldingInteract);
        }
        else if (inter->requiresInput && !inter->requiresStay) {
            if (anyOverlap && interactPressed) {
                inter->isPressed = !inter->isPressed;
            }
        }
        else if (!inter->requiresInput && inter->requiresStay) {
            inter->isPressed = anyOverlap;
        }
        else {
            if (anyOverlap) {
                inter->isPressed = true;
            }
        }

        if (inter->isPressed != wasPressed) {
            audioManager.buttonSound.play();

            for (auto& ent : entityManager.GetEntities()) {
                if (ent->movingPlatform && ent->GetTag() == inter->linkedTag) {
                    if (inter->isPressed) {
                        ent->movingPlatform->currentTriggers++;
                    }
                    else {
                        ent->movingPlatform->currentTriggers--;
                    }

                    ent->movingPlatform->triggered =
                        (ent->movingPlatform->currentTriggers >= ent->movingPlatform->requiredTriggers);
                }

                if (ent->door && ent->door->linkTag == inter->linkedTag) {
                    ent->door->isOpen = inter->isPressed;

                    ent->sprite->getShape().setFillColor(
                        ent->door->isOpen ? GameConstants::kDoorOpenColor : GameConstants::kDoorColor
                    );
                }
            }
        }

        if (button->sprite) {
            button->sprite->getShape().setFillColor(
                inter->isPressed ? GameConstants::kButtonPressedColor : GameConstants::kButtonReleasedColor
            );
        }
    }
}

void sWinCondition(EntityManager& entityManager,
                   GameState& m_state,
                   Vec2& transitionCenter,
                   float& transitionRadius,
                   bool& m_isLoadingNextLevel) {
    for (auto& exit : entityManager.GetEntities("Exit")) {
        if (!exit->transform || !exit->boundingBox) { continue; }

        int playersAtExit = 0;

        for (auto& player : entityManager.GetEntities("Player")) {
            if (!player->transform || !player->boundingBox || player->health->isDead) { continue; }

            float dx = std::abs(player->transform->position.x - exit->transform->position.x);
            float dy = std::abs(player->transform->position.y - exit->transform->position.y);

            if (dx < player->boundingBox->halfSize.x + exit->boundingBox->halfSize.x &&
                dy < player->boundingBox->halfSize.y + exit->boundingBox->halfSize.y) {
                playersAtExit++;
            }
        }

        if (playersAtExit >= GameConstants::kRequiredPlayersAtExit) {
            if (m_state == GameState::RespawnFadeOut || m_state == GameState::RespawnFadeIn) { return; }
            m_state = GameState::RespawnFadeOut;
            transitionCenter = exit->transform->position;
            transitionRadius = GameConstants::kTransitionRadiusStart;
            m_isLoadingNextLevel = true;
        }
    }
}
} // namespace InteractSystem
