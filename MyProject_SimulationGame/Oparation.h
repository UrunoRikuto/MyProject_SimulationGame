#pragma once
#include <DirectXMath.h>

inline int GetRandOfRange(const int inMin, const int inMax)
{
    int out = rand() % (inMax - inMin);
    out += inMin;

    return out;
}

// FLOAT2同士の演算子オーバーロード

inline DirectX::XMFLOAT2 operator+(const DirectX::XMFLOAT2& lhs, const DirectX::XMFLOAT2& rhs)
{
    return { lhs.x + rhs.x, lhs.y + rhs.y };
}
inline DirectX::XMFLOAT2 operator-(const DirectX::XMFLOAT2& lhs, const DirectX::XMFLOAT2& rhs)
{
    return { lhs.x - rhs.x, lhs.y - rhs.y };
}
inline DirectX::XMFLOAT2 operator*(const DirectX::XMFLOAT2& lhs, const DirectX::XMFLOAT2& rhs)
{
    return { lhs.x * rhs.x, lhs.y * rhs.y };
}
inline DirectX::XMFLOAT2 operator/(const DirectX::XMFLOAT2& lhs, const DirectX::XMFLOAT2& rhs)
{
    return { lhs.x / rhs.x, lhs.y / rhs.y };
}
inline DirectX::XMFLOAT2 operator+=(DirectX::XMFLOAT2& lhs, const DirectX::XMFLOAT2& rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    return lhs;
}
inline DirectX::XMFLOAT2 operator-=(DirectX::XMFLOAT2& lhs, const DirectX::XMFLOAT2& rhs)
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    return lhs;
}
inline DirectX::XMFLOAT2 operator*=(DirectX::XMFLOAT2& lhs, const DirectX::XMFLOAT2& rhs)
{
    lhs.x *= rhs.x;
    lhs.y *= rhs.y;
    return lhs;
}
inline DirectX::XMFLOAT2 operator/=(DirectX::XMFLOAT2& lhs, const DirectX::XMFLOAT2& rhs)
{
    lhs.x /= rhs.x;
    lhs.y /= rhs.y;
    return lhs;
}

// FLOAT2とfloatの演算子オーバーロード

inline DirectX::XMFLOAT2 operator*(const DirectX::XMFLOAT2& lhs, float rhs)
{
    return { lhs.x * rhs, lhs.y * rhs };
}
inline DirectX::XMFLOAT2 operator/(const DirectX::XMFLOAT2& lhs, float rhs)
{
    return { lhs.x / rhs, lhs.y / rhs };
}
inline DirectX::XMFLOAT2 operator*(float lhs, const DirectX::XMFLOAT2& rhs)
{
    return { lhs * rhs.x, lhs * rhs.y };
}
inline DirectX::XMFLOAT2 operator/(float lhs, const DirectX::XMFLOAT2& rhs)
{
    return { lhs / rhs.x, lhs / rhs.y };
}
inline DirectX::XMFLOAT2 operator*=(DirectX::XMFLOAT2& lhs, float rhs)
{
    lhs.x *= rhs;
    lhs.y *= rhs;
    return lhs;
}
inline DirectX::XMFLOAT2 operator/=(DirectX::XMFLOAT2& lhs, float rhs)
{
    lhs.x /= rhs;
    lhs.y /= rhs;
    return lhs;
}

// FLOAT2とINT2の演算子オーバーロード

inline DirectX::XMFLOAT2 operator+(const DirectX::XMFLOAT2& lhs, const DirectX::XMINT2& rhs)
{
    return { lhs.x + rhs.x, lhs.y + rhs.y };
}
inline DirectX::XMFLOAT2 operator-(const DirectX::XMFLOAT2& lhs, const DirectX::XMINT2& rhs)
{
    return { lhs.x - rhs.x, lhs.y - rhs.y };
}
inline DirectX::XMFLOAT2 operator*(const DirectX::XMFLOAT2& lhs, const DirectX::XMINT2& rhs)
{
    return { lhs.x * rhs.x, lhs.y * rhs.y };
}
inline DirectX::XMFLOAT2 operator/(const DirectX::XMFLOAT2& lhs, const DirectX::XMINT2& rhs)
{
    return { lhs.x / rhs.x, lhs.y / rhs.y };
}
inline DirectX::XMFLOAT2 operator+=(DirectX::XMFLOAT2& lhs, const DirectX::XMINT2& rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    return lhs;
}
inline DirectX::XMFLOAT2 operator-=(DirectX::XMFLOAT2& lhs, const DirectX::XMINT2& rhs)
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    return lhs;
}
inline DirectX::XMFLOAT2 operator*=(DirectX::XMFLOAT2& lhs, const DirectX::XMINT2& rhs)
{
    lhs.x *= rhs.x;
    lhs.y *= rhs.y;
    return lhs;
}
inline DirectX::XMFLOAT2 operator/=(DirectX::XMFLOAT2& lhs, const DirectX::XMINT2& rhs)
{
    lhs.x /= rhs.x;
    lhs.y /= rhs.y;
    return lhs;
}

// FLOAT3同士の演算子オーバーロード

inline DirectX::XMFLOAT3 operator+(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
{
    return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
}
inline DirectX::XMFLOAT3 operator-(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
{
    return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
}
inline DirectX::XMFLOAT3 operator*(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
{
    return { lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z };
}
inline DirectX::XMFLOAT3 operator/(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
{
    return { lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z };
}
inline DirectX::XMFLOAT3 operator+=(DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    lhs.z += rhs.z;
    return lhs;
}
inline DirectX::XMFLOAT3 operator-=(DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    lhs.z -= rhs.z;
    return lhs;
}
inline DirectX::XMFLOAT3 operator*=(DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
{
    lhs.x *= rhs.x;
    lhs.y *= rhs.y;
    lhs.z *= rhs.z;
    return lhs;
}
inline DirectX::XMFLOAT3 operator/=(DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
{
    lhs.x /= rhs.x;
    lhs.y /= rhs.y;
    lhs.z /= rhs.z;
    return lhs;
}

// FLOAT3とfloatの演算子オーバーロード

inline DirectX::XMFLOAT3 operator*(const DirectX::XMFLOAT3& lhs, float rhs)
{
    return { lhs.x * rhs, lhs.y * rhs, lhs.z * rhs };
}
inline DirectX::XMFLOAT3 operator/(const DirectX::XMFLOAT3& lhs, float rhs)
{
    return { lhs.x / rhs, lhs.y / rhs, lhs.z / rhs };
}
inline DirectX::XMFLOAT3 operator*(float lhs, const DirectX::XMFLOAT3& rhs)
{
    return { lhs * rhs.x, lhs * rhs.y, lhs * rhs.z };
}
inline DirectX::XMFLOAT3 operator/(float lhs, const DirectX::XMFLOAT3& rhs)
{
    return { lhs / rhs.x, lhs / rhs.y, lhs / rhs.z };
}
inline DirectX::XMFLOAT3 operator*=(DirectX::XMFLOAT3& lhs, float rhs)
{
    lhs.x *= rhs;
    lhs.y *= rhs;
    lhs.z *= rhs;
    return lhs;
}
inline DirectX::XMFLOAT3 operator/=(DirectX::XMFLOAT3& lhs, float rhs)
{
    lhs.x /= rhs;
    lhs.y /= rhs;
    lhs.z /= rhs;
    return lhs;
}

// FLOAT4同士の演算子オーバーロード

inline DirectX::XMFLOAT4 operator+(const DirectX::XMFLOAT4& lhs, const DirectX::XMFLOAT4& rhs)
{
    return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w };
}
inline DirectX::XMFLOAT4 operator-(const DirectX::XMFLOAT4& lhs, const DirectX::XMFLOAT4& rhs)
{
    return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w };
}
inline DirectX::XMFLOAT4 operator*(const DirectX::XMFLOAT4& lhs, const DirectX::XMFLOAT4& rhs)
{
    return { lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w };
}
inline DirectX::XMFLOAT4 operator/(const DirectX::XMFLOAT4& lhs, const DirectX::XMFLOAT4& rhs)
{
    return { lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w };
}
inline DirectX::XMFLOAT4 operator+=(DirectX::XMFLOAT4& lhs, const DirectX::XMFLOAT4& rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    lhs.z += rhs.z;
    lhs.w += rhs.w;
    return lhs;
}
inline DirectX::XMFLOAT4 operator-=(DirectX::XMFLOAT4& lhs, const DirectX::XMFLOAT4& rhs)
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    lhs.z -= rhs.z;
    lhs.w -= rhs.w;
    return lhs;
}
inline DirectX::XMFLOAT4 operator*=(DirectX::XMFLOAT4& lhs, const DirectX::XMFLOAT4& rhs)
{
    lhs.x *= rhs.x;
    lhs.y *= rhs.y;
    lhs.z *= rhs.z;
    lhs.w *= rhs.w;
    return lhs;
}
inline DirectX::XMFLOAT4 operator/=(DirectX::XMFLOAT4& lhs, const DirectX::XMFLOAT4& rhs)
{
    lhs.x /= rhs.x;
    lhs.y /= rhs.y;
    lhs.z /= rhs.z;
    lhs.w /= rhs.w;
    return lhs;
}

// FLOAT4とfloatの演算子オーバーロード

inline DirectX::XMFLOAT4 operator*(const DirectX::XMFLOAT4& lhs, float rhs)
{
    return { lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs };
}
inline DirectX::XMFLOAT4 operator/(const DirectX::XMFLOAT4& lhs, float rhs)
{
    return { lhs.x / rhs, lhs.y / rhs, lhs.z / rhs, lhs.w / rhs };
}
inline DirectX::XMFLOAT4 operator*(float lhs, const DirectX::XMFLOAT4& rhs)
{
    return { lhs * rhs.x, lhs * rhs.y, lhs * rhs.z, lhs * rhs.w };
}
inline DirectX::XMFLOAT4 operator/(float lhs, const DirectX::XMFLOAT4& rhs)
{
    return { lhs / rhs.x, lhs / rhs.y, lhs / rhs.z, lhs / rhs.w };
}
inline DirectX::XMFLOAT4 operator*=(DirectX::XMFLOAT4& lhs, float rhs)
{
    lhs.x *= rhs;
    lhs.y *= rhs;
    lhs.z *= rhs;
    lhs.w *= rhs;
    return lhs;
}
inline DirectX::XMFLOAT4 operator/=(DirectX::XMFLOAT4& lhs, float rhs)
{
    lhs.x /= rhs;
    lhs.y /= rhs;
    lhs.z /= rhs;
    lhs.w /= rhs;
    return lhs;
}

// INT2同士の演算子オーバーロード

inline DirectX::XMINT2 operator+(const DirectX::XMINT2& lhs, const DirectX::XMINT2& rhs)
{
    return { lhs.x + rhs.x, lhs.y + rhs.y };
}
inline DirectX::XMINT2 operator-(const DirectX::XMINT2& lhs, const DirectX::XMINT2& rhs)
{
    return { lhs.x - rhs.x, lhs.y - rhs.y };
}
inline DirectX::XMINT2 operator*(const DirectX::XMINT2& lhs, const DirectX::XMINT2& rhs)
{
    return { lhs.x * rhs.x, lhs.y * rhs.y };
}
inline DirectX::XMINT2 operator/(const DirectX::XMINT2& lhs, const DirectX::XMINT2& rhs)
{
    return { lhs.x / rhs.x, lhs.y / rhs.y };
}
inline DirectX::XMINT2 operator+=(DirectX::XMINT2& lhs, const DirectX::XMINT2& rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    return lhs;
}
inline DirectX::XMINT2 operator-=(DirectX::XMINT2& lhs, const DirectX::XMINT2& rhs)
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    return lhs;
}
inline DirectX::XMINT2 operator*=(DirectX::XMINT2& lhs, const DirectX::XMINT2& rhs)
{
    lhs.x *= rhs.x;
    lhs.y *= rhs.y;
    return lhs;
}
inline DirectX::XMINT2 operator/=(DirectX::XMINT2& lhs, const DirectX::XMINT2& rhs)
{
    lhs.x /= rhs.x;
    lhs.y /= rhs.y;
    return lhs;
}

// VECTOR関連の演算子オーバーロード

inline DirectX::XMVECTOR operator+(const DirectX::XMVECTOR& lhs, const DirectX::XMVECTOR& rhs)
{
	return DirectX::XMVectorAdd(lhs, rhs);
}
inline DirectX::XMVECTOR operator-(const DirectX::XMVECTOR& lhs, const DirectX::XMVECTOR& rhs)
{
    return DirectX::XMVectorSubtract(lhs, rhs);
}
inline DirectX::XMVECTOR operator*(const DirectX::XMVECTOR& lhs, const DirectX::XMVECTOR& rhs)
{
    return DirectX::XMVectorMultiply(lhs, rhs);
}
inline DirectX::XMVECTOR operator/(const DirectX::XMVECTOR& lhs, const DirectX::XMVECTOR& rhs)
{
    return DirectX::XMVectorDivide(lhs, rhs);
}
inline DirectX::XMVECTOR operator+=(DirectX::XMVECTOR& lhs, const DirectX::XMVECTOR& rhs)
{
	lhs = DirectX::XMVectorAdd(lhs, rhs);
    return lhs;
}
inline DirectX::XMVECTOR operator-=(DirectX::XMVECTOR& lhs, const DirectX::XMVECTOR& rhs)
{
    lhs = DirectX::XMVectorSubtract(lhs, rhs);
    return lhs;
}
inline DirectX::XMVECTOR operator*=(DirectX::XMVECTOR& lhs, const DirectX::XMVECTOR& rhs)
{
    lhs = DirectX::XMVectorMultiply(lhs, rhs);
    return lhs;
}
inline DirectX::XMVECTOR operator/=(DirectX::XMVECTOR& lhs, const DirectX::XMVECTOR& rhs)
{
    lhs = DirectX::XMVectorDivide(lhs, rhs);
    return lhs;
}

inline DirectX::XMVECTOR operator*(const DirectX::XMVECTOR& lhs, const float rhs)
{
    return DirectX::XMVectorScale(lhs, rhs);
}
inline DirectX::XMVECTOR operator/(const DirectX::XMVECTOR& lhs, const float rhs)
{
    return DirectX::XMVectorScale(lhs, 1.0f / rhs);
}
inline DirectX::XMVECTOR operator*=(DirectX::XMVECTOR& lhs, const float rhs)
{
    lhs = DirectX::XMVectorScale(lhs, rhs);
    return lhs;
}
inline DirectX::XMVECTOR operator/=(DirectX::XMVECTOR& lhs, const float rhs)
{
    lhs = DirectX::XMVectorScale(lhs, 1.0f / rhs);
    return lhs;
}

inline float HelmiteValue(float t, float x0, float x1, float v0, float v1, float maxT = 1.0f)
{
    // エルミート曲線
    // P...関数
    // t...Time
    // x₀...初期位置ベクトル
    // x₁...目標位置ベクトル
    // v₀...初期位置接線ベクトル
    // v₁...目標接線ベクトル
    // P(t) = (t - 1)²(2t + 1)x₀ + t²(3 - 2t)x₁ + (1 - t)²tv₀ + (t - 1)t²v₁

    t = t / maxT;
    float out = powf(t - 1, 2) * (2 * t + 1) * x0 + powf(t, 2) * (3 - 2 * t) * x1 + powf(1 - t, 2) * t * v0 + (t - 1) * powf(t, 2) * v1;

    return out;
}

inline DirectX::XMFLOAT3 HelmiteValue(float t, DirectX::XMFLOAT3 x0, DirectX::XMFLOAT3 x1, DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1, float maxT = 1.0f)
{
    t = t / maxT;
    DirectX::XMFLOAT3 out = powf(t - 1, 2) * (2 * t + 1) * x0 + powf(t, 2) * (3 - 2 * t) * x1 + powf(1 - t, 2) * t * v0 + (t - 1) * powf(t, 2) * v1;

    return out;
}
