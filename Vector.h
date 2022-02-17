/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/

#pragma once

//=========================================================
// 2DVector - used for many pathfinding and many other
// operations that are treated as planar rather than 3d.
//=========================================================
class Vector2D
{
public:
	constexpr Vector2D() = default;
	constexpr Vector2D(const Vector2D&) = default;
	constexpr Vector2D& operator=(const Vector2D&) = default;

	constexpr Vector2D(float X, float Y)
		: x(X), y(Y)
	{
	}

	[[nodiscard]] inline constexpr Vector2D operator+(const Vector2D& v) const { return Vector2D(x + v.x, y + v.y); }
	[[nodiscard]] inline constexpr Vector2D operator-(const Vector2D& v) const { return Vector2D(x - v.x, y - v.y); }
	[[nodiscard]] inline constexpr Vector2D operator*(float fl) const { return Vector2D(x * fl, y * fl); }
	[[nodiscard]] inline constexpr Vector2D operator/(float fl) const { return Vector2D(x / fl, y / fl); }

	[[nodiscard]] inline float Length() const { return static_cast<float>(sqrt(x * x + y * y)); }

	[[nodiscard]] inline Vector2D Normalize() const
	{
		float flLen = Length();
		if (flLen == 0)
		{
			return Vector2D(0, 0);
		}
		else
		{
			flLen = 1 / flLen;
			return Vector2D(x * flLen, y * flLen);
		}
	}

	vec_t x = 0, y = 0;
};

[[nodiscard]] constexpr float DotProduct(const Vector2D& a, const Vector2D& b)
{
	return (a.x * b.x + a.y * b.y);
}

[[nodiscard]] constexpr Vector2D operator*(float fl, const Vector2D& v)
{
	return v * fl;
}

//=========================================================
// 3D Vector
//=========================================================
class Vector // same data-layout as engine's vec3_t,
{			 //		which is a vec_t[3]
public:
	// Construction/destruction
	constexpr Vector() = default;
	constexpr Vector(const Vector&) = default;
	constexpr Vector& operator=(const Vector&) = default;

	constexpr Vector(float X, float Y, float Z)
		: x(X), y(Y), z(Z)
	{
	}

	constexpr Vector(float rgfl[3])
		: x(rgfl[0]), y(rgfl[1]), z(rgfl[2])
	{
	}

	// Operators
	[[nodiscard]] inline constexpr Vector operator-() const { return Vector(-x, -y, -z); }
	[[nodiscard]] inline constexpr bool operator==(const Vector& v) const { return x == v.x && y == v.y && z == v.z; }
	[[nodiscard]] inline constexpr bool operator!=(const Vector& v) const { return !(*this == v); }
	[[nodiscard]] inline constexpr Vector operator+(const Vector& v) const { return Vector(x + v.x, y + v.y, z + v.z); }
	[[nodiscard]] inline constexpr Vector operator-(const Vector& v) const { return Vector(x - v.x, y - v.y, z - v.z); }
	[[nodiscard]] inline constexpr Vector operator*(float fl) const { return Vector(x * fl, y * fl, z * fl); }
	[[nodiscard]] inline constexpr Vector operator/(float fl) const { return Vector(x / fl, y / fl, z / fl); }
	[[nodiscard]] inline constexpr bool operator==(const float* v) const { return x == v[0] && y == v[1] && z == v[2]; }
	[[nodiscard]] inline constexpr bool operator!=(const float* v) const { return !(*this == v); }
	[[nodiscard]] inline constexpr Vector operator+(const float* v) const { return Vector(x + v[0], y + v[1], z + v[2]); }
	[[nodiscard]] inline constexpr Vector operator-(const float* v) const { return Vector(x - v[0], y - v[1], z - v[2]); }
	inline constexpr void operator+=(const Vector& v) { this->x += v.x; this->y += v.y; this->z += v.z; }
	inline constexpr void operator+=(const float* v) { this->x += v[0]; this->y += v[1]; this->z += v[2]; }
	inline constexpr void operator*=(const float v) { this->x *= v; this->y *= v; this->z *= v; }
	inline constexpr void operator-=(const Vector& v) { this->x -= v.x; this->y -= v.y; this->z -= v.z; }
	inline constexpr void operator-=(const float* v) { this->x -= v[0]; this->y -= v[1]; this->z -= v[2]; }
	// Methods
	inline constexpr void CopyToArray(float* rgfl) const { rgfl[0] = x, rgfl[1] = y, rgfl[2] = z; }

	[[nodiscard]] inline constexpr float LengthSquared() const { return x * x + y * y + z * z; }
	[[nodiscard]] inline float Length() const { return static_cast<float>(sqrt(LengthSquared())); }
	[[nodiscard]] inline float FLength() const { return static_cast<float>(mathlib::fsqrt(LengthSquared())); }
	[[nodiscard]] inline constexpr operator float* () { return &x; }			 // Vectors will now automatically convert to float * when needed
	[[nodiscard]] inline constexpr operator const float* () const { return &x; } // Vectors will now automatically convert to float * when needed

	[[nodiscard]] inline Vector Normalize() const
	{
		float flLen = Length();
		if (flLen == 0)
			return Vector(0, 0, 1); // ????
		flLen = 1 / flLen;
		return Vector(x * flLen, y * flLen, z * flLen);
	}

	[[nodiscard]] inline constexpr Vector2D Make2D() const
	{
		return { x, y };
	}

	[[nodiscard]] inline float Length2D() const { return static_cast<float>(sqrt(x * x + y * y)); }

	// Members
	vec_t x = 0, y = 0, z = 0;
};

[[nodiscard]] constexpr Vector operator*(float fl, const Vector& v)
{
	return v * fl;
}

[[nodiscard]] constexpr float DotProduct(const Vector& a, const Vector& b)
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}

[[nodiscard]] constexpr Vector CrossProduct(const Vector& a, const Vector& b)
{
	return Vector(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}