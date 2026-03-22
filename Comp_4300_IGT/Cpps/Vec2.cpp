#include "Vec2.h"
#include<cmath>

//Operators Overloading
Vec2 Vec2::operator+(const Vec2& other) const {
	return Vec2(x + other.x, y + other.y);
}

Vec2 Vec2::operator-(const Vec2& other) const {
	return Vec2(x - other.x, y - other.y);
}

Vec2 Vec2::operator*(float scalar) const {
	return Vec2(x * scalar, y * scalar);
}

Vec2 Vec2::operator/(float scalar) const {
	return Vec2(x / scalar, y / scalar);
}

//Booleans
bool Vec2::operator==(const Vec2& other) const {
	return x == other.x && y == other.y;
}

bool Vec2::operator!=(const Vec2& other) const {
	return !(*this == other);
}

//Compound Assignment Operators
void Vec2::operator+=(const Vec2& other) {
	x += other.x;
	y += other.y;
}

void Vec2::operator-=(const Vec2& other) {
	x -= other.x;
	y -= other.y;
}

void Vec2::operator*=(float scalar) {
	x *= scalar;
	y *= scalar;
}

void Vec2::operator/=(float scalar) {
	x /= scalar;
	y /= scalar;
}

float Vec2::Dist(const Vec2& other) const {
	return sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y));
}

Vec2 Vec2::Normalize() const {
	float length = sqrt(x * x + y * y);
	if (length != 0) {
		return Vec2(x / length, y / length);
	}
	return Vec2(0, 0); // Return zero vector if length is zero to avoid division by zero
}