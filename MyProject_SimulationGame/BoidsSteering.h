/**************************************************//*
	@file	| BoidsSteering.h
	@brief	| Boidsのステアリングクラス定義
    @note	| Boidsのステアリング定義
*//**************************************************/
#pragma once
#include <vector>
#include <cmath>
#include "StructMath.h"

// Boidsのパラメータ構造体
struct BoidsParams
{
	// 視野半径
    float fViewRadius = 8.0f;
	// 分離半径
    float fSeparationRadius = 2.0f;

	// 分離行動の重み係数
    float fWeightSeparation = 1.0f;
	// 整列行動の重み係数
    float fWeightAlignment  = 1.0f;
	// 凝集行動の重み係数
    float fWeightCohesion   = 1.0f;

    // 最大速度
    float fMaxSpeed = 3.0f;
	// 最大力
    float fMaxForce = 1.0f;

    // 分離行動の最大力
	float fMaxSeparationForce = 1.50;
	// 整列行動の最大力
	float fMaxAlignmentForce = 1.0f;
	// 凝集行動の最大力
	float fMaxCohesionForce = 1.0f;
};

// @brief Boidsの近隣情報構造体
struct BoidsNeighbor
{
	// 位置
	DirectX::XMFLOAT3 v3Position = { 0.0f, 0.0f, 0.0f };
	// 速度
	DirectX::XMFLOAT3 v3Velocity = { 0.0f, 0.0f, 0.0f };
	// 標的が設定されているか
	bool bSetTarget = false;
	// 標的位置ポインタ（存在する場合）
	DirectX::XMFLOAT3 pTargetPos = { 0.0f, 0.0f, 0.0f };
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
    static DirectX::XMFLOAT3 Compute(
        const DirectX::XMFLOAT3& selfPos,
        const DirectX::XMFLOAT3& selfVel,
        const std::vector<BoidsNeighbor>& neighbors,
        const BoidsParams& params
    );

private:
	// @brief 分離ステアリング力を計算
	// @param selfPos 自身の位置
	// @param neighbors 近隣のBoids情報リスト
	// @param params Boidsのパラメータ
	// @return 分離ステアリング力ベクトル
    static DirectX::XMFLOAT3 Separation(
        const DirectX::XMFLOAT3& selfPos,
        const std::vector<BoidsNeighbor>& neighbors,
        const BoidsParams& params
    );

	// @brief 整列ステアリング力を計算
	// @param selfPos 自身の位置
	// @param selfVel 自身の速度
	// @param neighbors 近隣のBoids情報リスト
	// @param params Boidsのパラメータ
	// @return 整列ステアリング力ベクトル
    static DirectX::XMFLOAT3 Alignment(
        const DirectX::XMFLOAT3& selfPos,
        const DirectX::XMFLOAT3& selfVel,
        const std::vector<BoidsNeighbor>& neighbors,
        const BoidsParams& params
    );

	// @brief 凝集ステアリング力を計算
	// @param selfPos 自身の位置
	// @param selfVel 自身の速度
	// @param neighbors 近隣のBoids情報リスト
	// @param params Boidsのパラメータ
	// @return 凝集ステアリング力ベクトル
    static DirectX::XMFLOAT3 Cohesion(
        const DirectX::XMFLOAT3& selfPos,
        const DirectX::XMFLOAT3& selfVel,
        const std::vector<BoidsNeighbor>& neighbors,
        const BoidsParams& params
    );
};
