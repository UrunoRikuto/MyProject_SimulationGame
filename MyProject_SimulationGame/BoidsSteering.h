/**************************************************//*
	@file	| BoidsSteering.h
	@brief	| Boidsのステアリングクラス定義
    @note	| Boidsのステアリング定義
*//**************************************************/
#pragma once
#include <vector>
#include <cmath>
#include "StructMath.h"

// @brief 簡易ベクトル構造体
struct Vec3
{
    float x, y, z;

    Vec3 operator+(const Vec3& o) const { return { x + o.x, y + o.y, z + o.z }; }
    Vec3 operator-(const Vec3& o) const { return { x - o.x, y - o.y, z - o.z }; }
    Vec3 operator*(float s) const { return { x * s, y * s, z * s }; }
    Vec3& operator+=(const Vec3& o) { x += o.x; y += o.y; z += o.z; return *this; }
};

// @brief ベクトルの長さを計算
// @param v ベクトル
// @return ベクトルの長さ
inline float Length(const Vec3& v)
{
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

// @brief ベクトルを正規化
// @param v ベクトル
// @return 正規化されたベクトル
inline Vec3 Normalize(const Vec3& v)
{
    float len = Length(v);
    if (len < 0.0001f) return { 0,0,0 };
    return { v.x / len, v.y / len, v.z / len };
}

// Boidsのパラメータ構造体
struct BoidsParams
{
	// 視野半径
    float fViewRadius = 8.0f;
	// 分離半径
    float fSeparationRadius = 2.0f;

	// 分離行動の重み係数
    float fWeightSeparation = 1.2f;
	// 整列行動の重み係数
    float fWeightAlignment  = 1.0f;
	// 凝集行動の重み係数
    float fWeightCohesion   = 1.0f;

    // 最大速度
    float fMaxSpeed = 3.0f;
	// 最大力
    float fMaxForce = 1.0f;
};

// 値保持に変更（ポインタを排除）
struct BoidsNeighbor
{
	// 位置
    Vec3 v3Position;
	// 速度
    Vec3 v3Velocity;
};

// @brief Boidsのステアリングクラス
class BoidsSteering
{
public:
	// @brief Boidsのステアリング力を計算
	// @param selfPos 自身の位置
	// @param selfVel 自身の速度
	// @param neighbors 近隣のBoids情報リスト
	// @param params Boidsのパラメータ
	// @return ステアリング力ベクトル
    static Vec3 Compute(
        const Vec3& selfPos,
        const Vec3& selfVel,
        const std::vector<BoidsNeighbor>& neighbors,
        const BoidsParams& params
    );

private:
	// @brief 分離ステアリング力を計算
	// @param selfPos 自身の位置
	// @param neighbors 近隣のBoids情報リスト
	// @param params Boidsのパラメータ
	// @return 分離ステアリング力ベクトル
    static Vec3 Separation(
        const Vec3& selfPos,
        const std::vector<BoidsNeighbor>& neighbors,
        const BoidsParams& params
    );

	// @brief 整列ステアリング力を計算
	// @param selfPos 自身の位置
	// @param selfVel 自身の速度
	// @param neighbors 近隣のBoids情報リスト
	// @param params Boidsのパラメータ
	// @return 整列ステアリング力ベクトル
    static Vec3 Alignment(
        const Vec3& selfPos,
        const Vec3& selfVel,
        const std::vector<BoidsNeighbor>& neighbors,
        const BoidsParams& params
    );

	// @brief 凝集ステアリング力を計算
	// @param selfPos 自身の位置
	// @param selfVel 自身の速度
	// @param neighbors 近隣のBoids情報リスト
	// @param params Boidsのパラメータ
	// @return 凝集ステアリング力ベクトル
    static Vec3 Cohesion(
        const Vec3& selfPos,
        const Vec3& selfVel,
        const std::vector<BoidsNeighbor>& neighbors,
        const BoidsParams& params
    );
};
