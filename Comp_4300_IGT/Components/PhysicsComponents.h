#pragma once
#include "../Vec2.h"

class CTransform {
public:
    Vec2  position = { 0.0f, 0.0f };
    Vec2  velocity = { 0.0f, 0.0f };
    float rotation = 0.0f;
    bool  onGround = false;

    int coyoteFrames = 0;
    int JumpBufferFrames = 0;

    CTransform() : position(0, 0), rotation(0), velocity(0.0f, 0.0f) {}
    CTransform(const Vec2& pos, const Vec2& vel, float rot)
        : position(pos), velocity(vel), rotation(rot) {}
};

class CBoundingBox {
public:
    Vec2 halfSize;
    CBoundingBox(float w, float h) : halfSize(w * 0.5f, h * 0.5f) {}
    float width()  const { return halfSize.x * 2.0f; }
    float height() const { return halfSize.y * 2.0f; }

    CBoundingBox() : halfSize(0, 0) {}
    CBoundingBox(const Vec2& halfSize) : halfSize(halfSize) {}
};
