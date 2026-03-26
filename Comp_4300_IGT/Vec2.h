#pragma once

class Vec2
{
public:

	float x;
	float y;

	Vec2() : x(0), y(0) {}
	Vec2(float x, float y) : x(x), y(y) {}

	bool operator==(const Vec2& other) const;
	bool operator!=(const Vec2& other) const;

	Vec2 operator+(const Vec2& other) const;
	Vec2 operator-(const Vec2& other) const;
	Vec2 operator*(float scalar) const;
	Vec2 operator/(float scalar) const;

	void operator+=(const Vec2& other);
	void operator-=(const Vec2& other);
	void operator*=(float scalar);
	void operator/=(float scalar);

	Vec2 Normalize() const;

	float Dist(const Vec2& other) const;

};