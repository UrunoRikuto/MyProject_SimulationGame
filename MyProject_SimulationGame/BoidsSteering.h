/**************************************************//*
	@file	| BoidsSteering.h
	@brief	| Boidsのステアリングクラス定義
    @note	| Boidsのステアリングを定義
*//**************************************************/
#pragma once
#include <vector>
#include <cmath>
#include "StructMath.h"

// @brief 3次元ベクトル構造体(軽量版)
struct Vec3
{
    float x, y, z;

    Vec3 operator+(const Vec3& o) const { return { x + o.x, y + o.y, z + o.z }; }
    Vec3 operator-(const Vec3& o) const { return { x - o.x, y - o.y, z - o.z }; }
    Vec3 operator*(float s) const { return { x * s, y * s, z * s }; }
    Vec3& operator+=(const Vec3& o) { x += o.x; y += o.y; z += o.z; return *this; }
};

// @brief ベクトルの長さを計算
inline float Length(const Vec3& v)
{
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

// @brief ベクトルを正規化
inline Vec3 Normalize(const Vec3& v)
{
    float len = Length(v);
    if (len < 0.0001f) return { 0,0,0 };
    return { v.x / len, v.y / len, v.z / len };
}

// @brief Boidsパラメータ構造体
struct BoidsParams
{
	// @brief 視界距離
    float fViewRadius = 6.0f;
	// @brief 分離距離
    float fSeparationRadius = 2.0f;

	// @brief 分離ルールの重み
    float fWeightSeparation = 1.5f;
	// @brief 整列ルールの重み
    float fWeightAlignment = 1.0f;
	// @brief 結合ルールの重み
    float fWeightCohesion = 1.0f;

	// @brief 最大速度
    float fMaxSpeed = 3.0f;
	// @brief 最大力
    float fMaxForce = 1.0f;
};

// @brief Boids近隣情報構造体
struct BoidsNeighbor
{
	// 位置情報ポインタ
    const Vec3* v3Position;
	// 速度情報ポインタ
    const Vec3* v3Velocity;
};

// @brief Boidsステアリングクラス
class BoidsSteering
{
public:

	// @brief ステアリング計算
	// @param selfPos：自身の位置
	// @param selfVel：自身の速度
	// @param neighbors：近隣情報リスト
	// @param params：Boidsパラメータ
	// @return ステアリングベクトル
    static Vec3 Compute(
        const Vec3& selfPos,
        const Vec3& selfVel,
        const std::vector<BoidsNeighbor>& neighbors,
        const BoidsParams& params
    );

private:
	// @brief 分離ルール計算
	// @param selfPos：自身の位置
	// @param neighbors：近隣情報リスト
	// @param params：Boidsパラメータ
	// @return 分離ベクトル
    static Vec3 Separation(
        const Vec3& selfPos,
        const std::vector<BoidsNeighbor>& neighbors,
        const BoidsParams& params
    );

	// @brief 整列ルール計算
	// @param selfPos：自身の位置
	// @param neighbors：近隣情報リスト
	// @param params：Boidsパラメータ
	// @return 整列ベクトル
    static Vec3 Alignment(
        const Vec3& selfPos,
        const std::vector<BoidsNeighbor>& neighbors,
        const BoidsParams& params
    );

	// @brief 結合ルール計算
	// @param selfPos：自身の位置
	// @param neighbors：近隣情報リスト
	// @param params：Boidsパラメータ
	// @return 結合ベクトル
    static Vec3 Cohesion(
        const Vec3& selfPos,
        const std::vector<BoidsNeighbor>& neighbors,
        const BoidsParams& params
    );
};
