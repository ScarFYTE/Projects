#include "PhysicsSystem.h"

#include "../AudioManager.h"
#include "../Constants.h"
#include "../Entity.h"

#include <algorithm>
#include <cmath>
#include <vector>

namespace PhysicsSystem {
void sGravity(EntityManager& entityManager) {
    for (auto& e : entityManager.GetEntities("Player")) {
        if (!e->transform) { continue; }

        auto& t = e->transform;

        if (t->coyoteFrames > 0) t->coyoteFrames--;

        bool wasOnGround = t->onGround;

        t->onGround = false;
        if (wasOnGround && !t->onGround && t->velocity.y >= 0.0f) {
            t->coyoteFrames = GameConstants::kCoyoteFrames;
        }

        t->velocity.y += GameConstants::kGravity;
        if (t->velocity.y > GameConstants::kMaxFallSpeed) {
            t->velocity.y = GameConstants::kMaxFallSpeed;
        }
    }
}

void sMovement(EntityManager& entityManager, AudioManager& audioManager,
               const std::shared_ptr<Entity>&,
               const std::shared_ptr<Entity>&,
               void (*spawnDustParticles)(EntityManager&, Vec2, int, float)) {
    for (auto& e : entityManager.GetEntities("Player")) {
        if (!e->transform || !e->input) { continue; }

        auto& t = e->transform;
        auto& in = e->input;

        if (in->jump) {
            t->JumpBufferFrames = GameConstants::kJumpBufferFrames;
            in->jump = false;
        }
        if (t->JumpBufferFrames > 0) { t->JumpBufferFrames--; }

        bool canJump = t->onGround || t->coyoteFrames > 0;
        if (t->JumpBufferFrames > 0 && canJump) {
            if (t->velocity.y < 0.0f) {
                t->velocity.y += GameConstants::kJumpVelocity;
            } else {
                t->velocity.y = GameConstants::kJumpVelocity;
            }

            t->onGround = false;
            t->coyoteFrames = 0;
            t->JumpBufferFrames = 0;

            spawnDustParticles(entityManager,
                Vec2(t->position.x, t->position.y + e->boundingBox->halfSize.y),
                GameConstants::kJumpDustCount, 0.0f);

            audioManager.jumpSound.play();
        }

        bool pushingLeft = in->left && !in->right;
        bool pushingRight = in->right && !in->left;

        bool turningLeft = pushingLeft && t->velocity.x > GameConstants::kTurnThreshold;
        bool turningRight = pushingRight && t->velocity.x < -GameConstants::kTurnThreshold;
        bool turning = turningLeft || turningRight;

        if (turning) {
            t->velocity.x *= GameConstants::kTurnFriction;

            if (std::abs(t->velocity.x) > GameConstants::kSkidSpeedThreshold) {
                Vec2 dustPos = Vec2(t->position.x, t->position.y + e->boundingBox->halfSize.y);
                float skidDir = (t->velocity.x > 0.0f) ? 1.0f : -1.0f;
                spawnDustParticles(entityManager, dustPos, GameConstants::kSkidParticleCount, skidDir);
            }
        }
        else if (pushingLeft) {
            t->velocity.x -= GameConstants::kAcceleration;
        }
        else if (pushingRight) {
            t->velocity.x += GameConstants::kAcceleration;
        }
        else {
            t->velocity.x *= GameConstants::kFriction;

            if (std::abs(t->velocity.x) < GameConstants::kStopVelocityThreshold) {
                t->velocity.x = 0.0f;
            }
        }

        t->velocity.x = std::clamp(t->velocity.x, -GameConstants::kMaxMoveSpeed, GameConstants::kMaxMoveSpeed);

        t->position.x += t->velocity.x;
        t->position.y += t->velocity.y;
    }
}

void sCollision(EntityManager& entityManager,
                void (*spawnDustParticles)(EntityManager&, Vec2, int, float)) {
    for (auto& e : entityManager.GetEntities("Player")) {
        if (!e->transform || !e->boundingBox) { continue; }

        const float hw = e->boundingBox->halfSize.x;
        const float hh = e->boundingBox->halfSize.y;

        std::vector<std::shared_ptr<Entity>> solidEntities;

        for (auto& tile : entityManager.GetEntities("Tile")) {
            solidEntities.push_back(tile);
        }

        for (auto& ent : entityManager.GetEntities()) {
            if (ent->movingPlatform) {
                solidEntities.push_back(ent);
            }
        }

        for (auto& geo : solidEntities) {
            if (!geo->transform || !geo->boundingBox) { continue; }
            const float geoHW = geo->boundingBox->halfSize.x;
            const float geoHH = geo->boundingBox->halfSize.y;
            const float geoX = geo->transform->position.x;
            const float geoY = geo->transform->position.y;

            if (std::abs(e->transform->position.x - geoX) < hw + geoHW &&
                std::abs(e->transform->position.y - geoY) < hh + geoHH) {

                float overlapX = (hw + geoHW) - std::abs(e->transform->position.x - geoX);
                float overlapY = (hh + geoHH) - std::abs(e->transform->position.y - geoY);

                if (overlapX < overlapY) {
                    if (e->transform->position.x < geoX) {
                        e->transform->position.x -= overlapX;
                    } else {
                        e->transform->position.x += overlapX;
                    }
                    e->transform->velocity.x = 0.0f;
                }
                else {
                    if (e->transform->position.y < geoY) {
                        e->transform->position.y -= overlapY;

                        if (!e->transform->onGround && e->transform->velocity.y > GameConstants::kLandingVelocityThreshold) {
                            spawnDustParticles(entityManager,
                                Vec2(e->transform->position.x, e->transform->position.y + e->boundingBox->halfSize.y),
                                GameConstants::kLandingDustCount, 0.0f);
                        }
                        e->transform->onGround = true;
                    }
                    else {
                        e->transform->position.y += overlapY;
                    }
                    e->transform->velocity.y = 0.0f;
                }
            }
        }
        for (auto& other : entityManager.GetEntities("Player")) {
            if (e.get() >= other.get()) { continue; }
            if (!other->transform || !other->boundingBox) { continue; }

            const float otherHW = other->boundingBox->halfSize.x;
            const float otherHH = other->boundingBox->halfSize.y;

            float dx = e->transform->position.x - other->transform->position.x;
            float dy = e->transform->position.y - other->transform->position.y;

            if (std::abs(dx) < hw + otherHW && std::abs(dy) < hh + otherHH) {
                float overlapX = (hw + otherHW) - std::abs(dx);
                float overlapY = (hh + otherHH) - std::abs(dy);

                if (overlapY < overlapX + GameConstants::kPlayerOverlapBias) {
                    if (dy < 0.0f) {
                        e->transform->position.y -= overlapY;
                        e->transform->onGround = true;
                        e->transform->coyoteFrames = GameConstants::kCoyoteFrames;

                        if (e->transform->velocity.y > 0.0f) {
                            e->transform->velocity.y = (other->transform->velocity.y < 0.0f) ? other->transform->velocity.y : 0.0f;
                        }

                        e->transform->position.x += other->transform->velocity.x;
                    }
                    else {
                        other->transform->position.y -= overlapY;
                        other->transform->onGround = true;
                        other->transform->coyoteFrames = GameConstants::kCoyoteFrames;

                        if (other->transform->velocity.y > 0.0f) {
                            other->transform->velocity.y = (e->transform->velocity.y < 0.0f) ? e->transform->velocity.y : 0.0f;
                        }

                        other->transform->position.x += e->transform->velocity.x;
                    }
                }
                else {
                    if (dx < 0.0f) {
                        e->transform->position.x -= overlapX * GameConstants::kPlayerPushFactor;
                        other->transform->position.x += overlapX * GameConstants::kPlayerPushFactor;
                    }
                    else {
                        e->transform->position.x += overlapX * GameConstants::kPlayerPushFactor;
                        other->transform->position.x -= overlapX * GameConstants::kPlayerPushFactor;
                    }

                    e->transform->velocity.x *= GameConstants::kPlayerPushFactor;
                    other->transform->velocity.x *= GameConstants::kPlayerPushFactor;
                }
            }
        }
    }
}

void sMovingPlatform(EntityManager& entityManager) {
    for (auto& e : entityManager.GetEntities()) {
        if (!e->movingPlatform || !e->transform || !e->boundingBox) { continue; }

        Vec2 target = e->movingPlatform->triggered ? e->movingPlatform->posB : e->movingPlatform->posA;
        Vec2 current = e->transform->position;

        Vec2 direction = target - current;

        float distSq = (direction.x * direction.x) + (direction.y * direction.y);
        float speedSq = e->movingPlatform->speed * e->movingPlatform->speed;

        Vec2 moveAmount(0.0f, 0.0f);

        if (distSq > speedSq) {
            Vec2 normDir = direction.Normalize();
            moveAmount = normDir * e->movingPlatform->speed;
            e->transform->position += moveAmount;
        }
        else {
            moveAmount = target - current;
            e->transform->position = target;
        }

        for (auto& player : entityManager.GetEntities("Player")) {
            if (!player->transform || !player->boundingBox) { continue; }
            float platTop = current.y - e->boundingBox->halfSize.y;
            float platLeft = current.x - e->boundingBox->halfSize.x;
            float platRight = current.x + e->boundingBox->halfSize.x;

            float playerBottom = player->transform->position.y + player->boundingBox->halfSize.y;
            float playerX = player->transform->position.x;

            if (std::abs(playerBottom - platTop) < GameConstants::kPlatformSnapThreshold && playerX > platLeft && playerX < platRight) {
                player->transform->position += moveAmount;
            }
        }
    }
}
} // namespace PhysicsSystem
