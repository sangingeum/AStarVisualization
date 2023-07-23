#include "Vec2.hpp"
#include <cmath>

Vec2::Vec2() {}
Vec2::Vec2(float xin, float yin) : x(xin), y(yin) {}
Vec2::Vec2(const sf::Vector2f& vec) : x(vec.x), y(vec.y) {}
Vec2::Vec2(const sf::Vector2i& vec) : x(static_cast<float>(vec.x)), y(static_cast<float>(vec.y)) {}

Vec2 Vec2::operator+(const Vec2& rhs) const {
	return Vec2(this->x + rhs.x, this->y + rhs.y);
}
Vec2 Vec2::operator-(const Vec2& rhs) const {
	return Vec2(this->x - rhs.x, this->y - rhs.y);
}
Vec2 Vec2::operator*(const Vec2& rhs) const {
	return Vec2(this->x * rhs.x, this->y * rhs.y);
}
Vec2 Vec2::operator*(float rhs) const {
	return Vec2(this->x * rhs, this->y * rhs);
}
Vec2 Vec2::operator/(const Vec2& rhs) const {
	return Vec2(this->x / rhs.x, this->y / rhs.y);
}
Vec2 Vec2::operator/(float rhs) const {
	return Vec2(this->x / rhs, this->y / rhs);
}
Vec2& Vec2::operator+=(const Vec2& rhs) {
	x += rhs.x;
	y += rhs.y;
	return *this;
}
Vec2& Vec2::operator-=(const Vec2& rhs) {
	x -= rhs.x;
	y -= rhs.y;
	return *this;
}
Vec2& Vec2::operator-=(float rhs) {
	x -= rhs;
	y -= rhs;
	return *this;
}
Vec2& Vec2::add(const Vec2& rhs) {
	x += rhs.x;
	y += rhs.y;
	return *this;
}
Vec2& Vec2::scale(float s) {
	x *= s;
	y *= s;
	return *this;
}
Vec2& Vec2::rotate(float degree) {
	float radian = degree * (3.141592653589793f / 180.0f);
	float cosine = std::cos(radian);
	float sine = std::sin(radian);
	float newX = x * cosine - y * sine;
	float newY = x * sine + y * cosine;
	x = newX;
	y = newY;
	return *this;
}

Vec2 Vec2::rotate_(float degree) const {
	float radian = degree * (3.141592653589793f / 180.0f);
	float cosine = std::cos(radian);
	float sine = std::sin(radian);
	float newX = x * cosine - y * sine;
	float newY = x * sine + y * cosine;
	return Vec2(newX, newY);
}

Vec2& Vec2::rotate(const Vec2& origin, float degree) {
	*this -= origin;
	rotate(degree);
	*this += origin;
	return *this;
}

Vec2 Vec2::rotate_(const Vec2& origin, float degree) const {
	return (*this - origin).rotate_(degree) + origin;
}

Vec2& Vec2::normalize() {
	float len = length();
	x /= len;
	y /= len;
	return *this;
}
Vec2& Vec2::resize(float length) {
	normalize();
	scale(length);
	return *this;
}

Vec2& Vec2::decreaseUntilZero(float diff) {
	x -= (std::signbit(x) ? -1.0f : 1.0f) * std::max(abs(x) - diff, 0.f);
	y -= (std::signbit(y) ? -1.0f : 1.0f) * std::max(abs(y) - diff, 0.f);
	return *this;
}

float Vec2::dist(const Vec2& v) const {
	return sqrtf(powf(x - v.x, 2) + powf(y - v.y, 2));
}

float Vec2::cross(const Vec2& v) const {
	return x * v.y - y * v.x;
}
float Vec2::dot(const Vec2& v) const {
	return x * v.x + y * v.y;
}
Vec2 Vec2::delta(const Vec2& v) const {
	return Vec2({ abs(this->x - v.x), abs(this->y - v.y) });
}

float Vec2::length() const {
	return sqrtf(powf(x, 2) + powf(y, 2));
}
void Vec2::set(float xin, float yin) {
	x = xin;
	y = yin;
}

void Vec2::negate() {
	x = -x;
	y = -y;
}

sf::Vector2f Vec2::toVec2f() const {
	return sf::Vector2f(x, y);
}


intersectionOutput checkIntersection(
	const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d) {
	// Check if a linesegment from a to b and a linesegment from c to d intersects
	// Find t and u such that "a + t(b-a) = c + u(d-c)" and "both t and u are between 0 and 1"
	// The order of inputs matter
	// r = b-a, s = d-c
	Vec2 r = b - a, s = d - c;
	float rxs = r.cross(s);
	Vec2 cma = (c - a);
	float t = cma.cross(s) / rxs;
	float u = cma.cross(r) / rxs;
	if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
		return { a + r * t, t, u, true };
	}
	return { Vec2(), 0, 0, false };
}