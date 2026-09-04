//========= Copyright (c) 2026 Mohamed Ashraf, All rights reserved. ============//
//
// This software is proprietary and confidential.
//
// You are granted a limited, non-exclusive, non-transferable license to use and
// modify this software for personal or internal purposes only. You may distribute
// compiled binary builds of the software provided that such distribution does not
// expose, include, or facilitate access to the source code.
//
// You may not distribute, publish, disclose, or otherwise make available the
// source code, in whole or in part, whether original or modified. All rights
// not expressly granted herein are reserved by the copyright holder.
//
// This software is provided "as is", without warranty of any kind, express or
// implied.
//
//=============================================================================//
#ifndef VECTOR_H
#define VECTOR_H
#pragma once

#include <cmath>

struct Vector2
{
    float x{}, y{};

    Vector2() = default;

    Vector2(float x, float y)
        : x(x), y(y)
    {
	}
};

struct Vector4
{
    float x{}, y{}, z{}, w{};

    Vector4() = default;

    Vector4(float x, float y, float z, float w)
        : x(x), y(y), z(z), w(w)
    {
    }
};

struct QAngle
{
    float x;
    float y;
    float z;
};

class CTransformUnaligned {
public:
    CTransformUnaligned()
    {
        m_position = Vector4{ 0,0,0,0 };
        m_scale = Vector4{ 1,1,1,1 };
        m_rotation = Vector4{ 0,0,1,0 };
    }
    Vector4 m_position;
    Vector4 m_scale;
    Vector4 m_rotation;
};

// FIXME
struct Quaternion
{
	float x;
	float y;
	float z;
	float w;

    Quaternion() = default;

    Quaternion(float x, float y, float z, float w)
        : x(x), y(y), z(z), w(w)
    {
    }
};

struct Vector3
{
    float x{}, y{}, z{};

    Vector3() = default;

    Vector3(float x, float y, float z)
        : x(x), y(y), z(z)
    {
    }
};

inline float VectorDot(const Vector3& a, const Vector3& b)
{
    return
        a.x * b.x +
        a.y * b.y +
        a.z * b.z;
}

inline float VectorLengthSqr(const Vector3& v)
{
    return VectorDot(v, v);
}

inline float VectorLength(const Vector3& v)
{
    return std::sqrt(VectorLengthSqr(v));
}

inline Vector3 VectorNormalize(const Vector3& v)
{
    const float length = VectorLength(v);

    if (length <= 0.00001f)
        return Vector3{};

    const float inverseLength = 1.0f / length;

    return Vector3{
        v.x * inverseLength,
        v.y * inverseLength,
        v.z * inverseLength
    };
}

inline Vector3 VectorAdd(
    const Vector3& a,
    const Vector3& b)
{
    return Vector3{
        a.x + b.x,
        a.y + b.y,
        a.z + b.z
    };
}

inline Vector3 VectorSubtract(
    const Vector3& a,
    const Vector3& b)
{
    return Vector3{
        a.x - b.x,
        a.y - b.y,
        a.z - b.z
    };
}

inline Vector3 VectorMultiply(
    const Vector3& v,
    float scalar)
{
    return Vector3{
        v.x * scalar,
        v.y * scalar,
        v.z * scalar
    };
}

typedef Vector3 Vertex;

#endif // VECTOR_H