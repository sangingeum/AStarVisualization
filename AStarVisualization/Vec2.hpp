#pragma once
#include <SFML/Graphics.hpp>
class Vec2 {
public:
	float x = 0;
	float y = 0;
	Vec2();
	Vec2(float xin, float yin);
	Vec2(const sf::Vector2f& vec);
	Vec2(const sf::Vector2i& vec);
	Vec2 operator+(const Vec2& rhs) const;
	Vec2 operator-(const Vec2& rhs) const;
	Vec2 operator*(const Vec2& rhs) const;
	Vec2 operator*(float rhs) const;
	Vec2 operator/(const Vec2& rhs) const;
	Vec2 operator/(float rhs) const;
	Vec2& operator+=(const Vec2& rhs);
	Vec2& operator-=(const Vec2& rhs);
	Vec2& operator-=(float rhs);
	Vec2& add(const Vec2& rhs);
	Vec2& scale(float s);
	Vec2& rotate(float degree);
	Vec2 rotate_(float degree) const;
	Vec2& rotate(const Vec2& origin, float degree);
	Vec2 rotate_(const Vec2& origin, float degree) const;
	Vec2& normalize();
	Vec2& resize(float length);
	Vec2& decreaseUntilZero(float diff);
	float dist(const Vec2& v) const;
	float cross(const Vec2& v) const;
	float dot(const Vec2& v) const;
	Vec2 delta(const Vec2& v) const;
	float length() const;
	void set(float xin, float yin);
	void negate();
	sf::Vector2f toVec2f() const;
};

struct intersectionOutput {
	Vec2 intersection; // intersection point 
	float t, u;     // a + t(b-a) = c + u(d-c)
	bool valid;
};

intersectionOutput checkIntersection(
	const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d);



