#include "RenderSystem.h"

#include "../Constants.h"
#include "../Entity.h"

#include <algorithm>

namespace {
void sParticle(EntityManager& entityManager) {
    for (auto& e : entityManager.GetEntities("Particle")) {
        if (!e->particle || !e->transform || !e->sprite) { continue; }

        auto& p = e->particle;
        auto& t = e->transform;

        p->age += GameConstants::kParticleAgeStep;

        if (p->age >= p->lifetime) {
            e->Destroy();
            continue;
        }

        t->velocity.y += GameConstants::kParticleGravity;
        t->position.x += t->velocity.x;
        t->position.y += t->velocity.y;

        sf::Color c = p->color;
        c.a = static_cast<std::uint8_t>(p->alpha() * GameConstants::kParticleAlphaScale);
        e->sprite->getShape().setFillColor(c);
    }
}

void sRenderMenu(sf::RenderWindow& window, sf::Font& font, int m_selectedOption) {
    const float cx = GameConstants::kWindowWidth * 0.5f;
    const float cy = GameConstants::kWindowHeight * 0.5f;

    sf::Text title(font), opt0(font), opt1(font);

    title.setCharacterSize(GameConstants::kMenuTitleSize);
    title.setFillColor(sf::Color::White);
    title.setString(GameConstants::kMenuTitleText);
    title.setPosition({ cx - title.getLocalBounds().size.x * 0.5f, cy - GameConstants::kMenuTitleOffset });

    opt0.setCharacterSize(GameConstants::kMenuOptionSize);
    opt0.setString(GameConstants::kMenuPlayText);
    opt0.setFillColor(m_selectedOption == 0 ? sf::Color::Yellow : GameConstants::kMenuInactiveColor);
    opt0.setPosition({ cx - opt0.getLocalBounds().size.x * 0.5f, cy });

    opt1.setCharacterSize(GameConstants::kMenuOptionSize);
    opt1.setString(GameConstants::kMenuQuitText);
    opt1.setFillColor(m_selectedOption == 1 ? sf::Color::Yellow : GameConstants::kMenuInactiveColor);
    opt1.setPosition({ cx - opt1.getLocalBounds().size.x * 0.5f, cy + GameConstants::kMenuOptionOffset });

    sf::Text arrow(font);
    arrow.setCharacterSize(GameConstants::kMenuArrowSize);
    arrow.setFillColor(sf::Color::Yellow);
    arrow.setString(GameConstants::kMenuArrowText);
    float arrowY = (m_selectedOption == 0) ? cy : cy + GameConstants::kMenuOptionOffset;
    arrow.setPosition({ cx - GameConstants::kMenuArrowOffset, arrowY });

    window.draw(title);
    window.draw(opt0);
    window.draw(opt1);
    window.draw(arrow);
}

void sRenderGameOver(sf::RenderWindow& window, sf::Font& font) {
    const float cx = GameConstants::kWindowWidth * 0.5f;
    const float cy = GameConstants::kWindowHeight * 0.5f;

    sf::Text over(font), sub(font), hint(font);

    over.setCharacterSize(GameConstants::kGameOverTitleSize);
    over.setFillColor(GameConstants::kGameOverTitleColor);
    over.setString(GameConstants::kGameOverTitleText);
    over.setPosition({ cx - over.getLocalBounds().size.x * 0.5f, cy - GameConstants::kGameOverTitleOffset });

    sub.setCharacterSize(GameConstants::kGameOverSubtitleSize);
    sub.setFillColor(sf::Color::White);
    sub.setString(GameConstants::kGameOverSubtitleText);
    sub.setPosition({ cx - sub.getLocalBounds().size.x * 0.5f, cy });

    hint.setCharacterSize(GameConstants::kGameOverHintSize);
    hint.setFillColor(GameConstants::kHudHintColor);
    hint.setString(GameConstants::kGameOverHintText);
    hint.setPosition({ cx - hint.getLocalBounds().size.x * 0.5f, cy + GameConstants::kGameOverHintOffset });

    window.draw(over);
    window.draw(sub);
    window.draw(hint);
}

void sRenderGameWon(sf::RenderWindow& window, sf::Font& font) {
    const float cx = GameConstants::kWindowWidth * 0.5f;
    const float cy = GameConstants::kWindowHeight * 0.5f;

    sf::Text over(font), sub(font), hint(font);

    over.setCharacterSize(GameConstants::kGameWonTitleSize);
    over.setFillColor(GameConstants::kGameWonTitleColor);
    over.setString(GameConstants::kGameWonTitleText);
    over.setPosition({ cx - over.getLocalBounds().size.x * 0.5f, cy - GameConstants::kGameWonTitleOffset });

    sub.setCharacterSize(GameConstants::kGameWonSubtitleSize);
    sub.setFillColor(sf::Color::White);
    sub.setString(GameConstants::kGameWonSubtitleText);
    sub.setPosition({ cx - sub.getLocalBounds().size.x * 0.5f, cy });

    hint.setCharacterSize(GameConstants::kGameWonHintSize);
    hint.setFillColor(GameConstants::kHudHintColor);
    hint.setString(GameConstants::kGameWonHintText);
    hint.setPosition({ cx - hint.getLocalBounds().size.x * 0.5f, cy + GameConstants::kGameWonHintOffset });

    window.draw(over);
    window.draw(sub);
    window.draw(hint);
}

void sRenderHud(sf::RenderWindow& window,
                sf::Font& font,
                sf::Texture& p1HeartTex,
                sf::Texture& p2HeartTex,
                const std::shared_ptr<Entity>& player1,
                const std::shared_ptr<Entity>& player2) {
    sf::Text p1Text(font), p2Text(font);
    p1Text.setCharacterSize(GameConstants::kHudTextSize);
    p2Text.setCharacterSize(GameConstants::kHudTextSize);
    p1Text.setFillColor(sf::Color::Cyan);
    p2Text.setFillColor(sf::Color::White);

    p1Text.setString("P1:");
    p2Text.setString("P2:");

    p1Text.setPosition({ GameConstants::kHudTextOffsetX, GameConstants::kHudP1OffsetY });
    p2Text.setPosition({ GameConstants::kHudTextOffsetX, GameConstants::kHudP2OffsetY });

    window.draw(p1Text);
    window.draw(p2Text);

    sf::Sprite p1Heart(p1HeartTex);
    sf::Sprite p2Heart(p2HeartTex);

    p1Heart.setScale({ GameConstants::kHeartScale, GameConstants::kHeartScale });
    p2Heart.setScale({ GameConstants::kHeartScale, GameConstants::kHeartScale });

    for (int i = 0; i < player1->health->lives; i++) {
        p1Heart.setPosition({ p1Text.getPosition().x + GameConstants::kHeartStartOffsetX + (i * GameConstants::kHeartSpacing),
                              GameConstants::kHudP1OffsetY });
        window.draw(p1Heart);
    }

    for (int i = 0; i < player2->health->lives; i++) {
        p2Heart.setPosition({ p2Text.getPosition().x + GameConstants::kHeartStartOffsetX + (i * GameConstants::kHeartSpacing),
                              GameConstants::kHudP2OffsetY });
        window.draw(p2Heart);
    }
}
}

namespace RenderSystem {
void sCamera(sf::RenderWindow& window, sf::View& gameView,
             const std::shared_ptr<Entity>& player1,
             const std::shared_ptr<Entity>& player2) {
    bool p1Alive = player1->transform && !player1->health->isDead;
    bool p2Alive = player2->transform && !player2->health->isDead;

    if (!p1Alive && !p2Alive) { return; }

    float cx, cy, viewW, viewH;

    if (p1Alive && p2Alive) {
        Vec2 p1 = player1->transform->position;
        Vec2 p2 = player2->transform->position;

        cx = (p1.x + p2.x) / 2.0f;
        cy = (p1.y + p2.y) / 2.0f;

        float dx = std::abs(p1.x - p2.x) + GameConstants::kCameraPadding;
        float dy = std::abs(p1.y - p2.y) + GameConstants::kCameraPadding;

        float aspect = static_cast<float>(GameConstants::kWindowHeight) / GameConstants::kWindowWidth;

        float minViewW = GameConstants::kMinViewWidth;
        float minViewH = minViewW * aspect;

        if (dy > dx * aspect) {
            viewH = std::max(minViewH, dy);
            viewW = viewH / aspect;
        }
        else {
            viewW = std::max(minViewW, dx);
            viewH = viewW * aspect;
        }

        if (viewW > GameConstants::kMaxViewWidth) {
            viewW = GameConstants::kMaxViewWidth;
            viewH = GameConstants::kMaxViewWidth * aspect;
        }
    }
    else {
        Vec2 pos = p1Alive ? player1->transform->position : player2->transform->position;
        cx = pos.x;
        cy = pos.y;
        viewW = static_cast<float>(GameConstants::kWindowWidth);
        viewH = static_cast<float>(GameConstants::kWindowHeight);
    }

    sf::Vector2f current = gameView.getCenter();
    sf::Vector2f target = { cx, cy };
    gameView.setCenter(current + (target - current) * GameConstants::kCameraLerp);

    sf::Vector2f currentSize = gameView.getSize();
    sf::Vector2f targetSize = { viewW, viewH };
    gameView.setSize(currentSize + (targetSize - currentSize) * GameConstants::kCameraLerp);

    window.setView(gameView);
}

void sRender(sf::RenderWindow& window,
             sf::View& gameView,
             EntityManager& entityManager,
             sf::Font& font,
             sf::Texture& p1HeartTex,
             sf::Texture& p2HeartTex,
             const std::shared_ptr<Entity>& player1,
             const std::shared_ptr<Entity>& player2,
             GameState m_state,
             int m_selectedOption,
             bool m_hasBackground,
             sf::Sprite& backgroundSprite,
             float transitionRadius,
             const Vec2& transitionCenter) {
    window.clear(GameConstants::kBackgroundClearColor);

    window.setView(window.getDefaultView());

    if (m_hasBackground) {
        backgroundSprite.setPosition({ 0.f, 0.f });
        window.draw(backgroundSprite);
    }

    if (m_state == GameState::StartMenu) {
        window.setView(window.getDefaultView());
        sRenderMenu(window, font, m_selectedOption);
        window.display();
        return;
    }

    if (m_state == GameState::GameOver) {
        window.setView(window.getDefaultView());
        sRenderGameOver(window, font);
        window.display();
        return;
    }

    if (m_state == GameState::GameWon) {
        window.setView(window.getDefaultView());
        sRenderGameWon(window, font);
        window.display();
        return;
    }

    window.setView(gameView);
    for (auto& e : entityManager.GetEntities()) {
        if (e->transform && e->sprite) {
            e->sprite->setPosition(e->transform->position);
            window.draw(e->sprite->getShape());
        }
    }

    sParticle(entityManager);

    if (m_state == GameState::RespawnFadeOut || m_state == GameState::RespawnFadeIn) {
        sf::CircleShape wipeCircle;
        float r = std::max(0.0f, transitionRadius);
        wipeCircle.setRadius(r);
        wipeCircle.setOrigin({ r, r });
        wipeCircle.setPosition({ transitionCenter.x, transitionCenter.y });

        wipeCircle.setFillColor(sf::Color::Transparent);
        wipeCircle.setOutlineColor(sf::Color::Black);
        wipeCircle.setOutlineThickness(GameConstants::kWipeOutlineThickness);

        window.draw(wipeCircle);
    }

    window.setView(window.getDefaultView());
    sRenderHud(window, font, p1HeartTex, p2HeartTex, player1, player2);
    window.display();
}
} // namespace RenderSystem
