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
// 2DCVector - used for many pathfinding and many other
// operations that are treated as planar rather than 3d.
//=========================================================
class CVector2D
{
public:
	constexpr CVector2D() = default;
	constexpr CVector2D(const CVector2D&) = default;
	constexpr CVector2D& operator=(const CVector2D&) = default;

	constexpr CVector2D(float X, float Y)
		: x(X), y(Y)
	{
	}

	[[nodiscard]] inline constexpr CVector2D operator+(const CVector2D& v) const { return CVector2D(x + v.x, y + v.y); }
	[[nodiscard]] inline constexpr CVector2D operator-(const CVector2D& v) const { return CVector2D(x - v.x, y - v.y); }
	[[nodiscard]] inline constexpr CVector2D operator*(float fl) const { return CVector2D(x * fl, y * fl); }
	[[nodiscard]] inline constexpr CVector2D operator/(float fl) const { return CVector2D(x / fl, y / fl); }

	[[nodiscard]] inline float Length() const { return static_cast<float>(sqrt(x * x + y * y)); }

	[[nodiscard]] inline CVector2D Normalize() const
	{
		float flLen = Length();
		if (flLen == 0)
		{
			return CVector2D(0, 0);
		}
		else
		{
			flLen = 1 / flLen;
			return CVector2D(x * flLen, y * flLen);
		}
	}

	vec_t x = 0, y = 0;
};

[[nodiscard]] constexpr float DotProduct(const CVector2D& a, const CVector2D& b)
{
	return (a.x * b.x + a.y * b.y);
}

[[nodiscard]] constexpr CVector2D operator*(float fl, const CVector2D& v)
{
	return v * fl;
}

//=========================================================
// 3D CVector
//=========================================================
class CVector // same data-layout as engine's vec3_t,
{			 //		which is a vec_t[3]
public:
	// Construction/destruction
	constexpr CVector() = default;
	constexpr CVector(const CVector&) = default;
	constexpr CVector& operator=(const CVector&) = default;

	constexpr CVector(float X, float Y, float Z)
		: x(X), y(Y), z(Z)
	{
	}

	constexpr CVector(float rgfl[3])
		: x(rgfl[0]), y(rgfl[1]), z(rgfl[2])
	{
	}

	// Operators
	[[nodiscard]] inline constexpr CVector operator-() const { return CVector(-x, -y, -z); }
	[[nodiscard]] inline constexpr bool operator==(const CVector& v) const { return x == v.x && y == v.y && z == v.z; }
	[[nodiscard]] inline constexpr bool operator!=(const CVector& v) const { return !(*this == v); }
	[[nodiscard]] inline constexpr CVector operator+(const CVector& v) const { return CVector(x + v.x, y + v.y, z + v.z); }
	[[nodiscard]] inline constexpr CVector operator-(const CVector& v) const { return CVector(x - v.x, y - v.y, z - v.z); }
	[[nodiscard]] inline constexpr CVector operator*(float fl) const { return CVector(x * fl, y * fl, z * fl); }
	[[nodiscard]] inline constexpr CVector operator/(float fl) const { return CVector(x / fl, y / fl, z / fl); }
	[[nodiscard]] inline constexpr bool operator==(const float* v) const { return x == v[0] && y == v[1] && z == v[2]; }
	[[nodiscard]] inline constexpr bool operator!=(const float* v) const { return !(*this == v); }
	[[nodiscard]] inline constexpr CVector operator+(const float* v) const { return CVector(x + v[0], y + v[1], z + v[2]); }
	[[nodiscard]] inline constexpr CVector operator-(const float* v) const { return CVector(x - v[0], y - v[1], z - v[2]); }
	inline constexpr void operator+=(const CVector& v) { this->x += v.x; this->y += v.y; this->z += v.z; }
	inline constexpr void operator+=(const float* v) { this->x += v[0]; this->y += v[1]; this->z += v[2]; }
	inline constexpr void operator*=(const float v) { this->x *= v; this->y *= v; this->z *= v; }
	inline constexpr void operator-=(const CVector& v) { this->x -= v.x; this->y -= v.y; this->z -= v.z; }
	inline constexpr void operator-=(const float* v) { this->x -= v[0]; this->y -= v[1]; this->z -= v[2]; }
	// Methods
	inline constexpr void CopyToArray(float* rgfl) const { rgfl[0] = x, rgfl[1] = y, rgfl[2] = z; }

	[[nodiscard]] inline constexpr float LengthSquared() const { return x * x + y * y + z * z; }
	[[nodiscard]] inline float Length() const { return static_cast<float>(sqrt(LengthSquared())); }
	[[nodiscard]] inline float FLength() const { return static_cast<float>(mathlib::fsqrt(LengthSquared())); }
	[[nodiscard]] inline constexpr operator float* () { return &x; }			 // CVectors will now automatically convert to float * when needed
	[[nodiscard]] inline constexpr operator const float* () const { return &x; } // CVectors will now automatically convert to float * when needed

	[[nodiscard]] inline CVector Normalize() const
	{
		float flLen = Length();
		if (flLen == 0)
			return CVector(0, 0, 1); // ????
		flLen = 1 / flLen;
		return CVector(x * flLen, y * flLen, z * flLen);
	}

	[[nodiscard]] inline constexpr CVector2D Make2D() const
	{
		return { x, y };
	}

	[[nodiscard]] inline float Length2D() const { return static_cast<float>(sqrt(x * x + y * y)); }

	// Members
	vec_t x = 0, y = 0, z = 0;
};

[[nodiscard]] constexpr CVector operator*(float fl, const CVector& v)
{
	return v * fl;
}

[[nodiscard]] constexpr float DotProduct(const CVector& a, const CVector& b)
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}

[[nodiscard]] constexpr CVector CrossProduct(const CVector& a, const CVector& b)
{
	return CVector(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}