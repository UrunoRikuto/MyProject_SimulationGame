/**************************************************//*
	@file	| GroupAI.h
	@brief	| 群れAI基底クラスの定義
	@note	| 群れ行動の共通処理を定義
*//**************************************************/
#pragma once
#include "AnimalAI.h"
#include "Oparation.h"

// @brief 群れAI基底クラス
class CGroupAI : public CAnimalAI
{
public:
	// @brief 引数付きコンストラクタ
	// @param p：Boidsパラメータ
	CGroupAI(const BoidsParams& params) : m_BoidsParams(params) {}

	// @brief ステアリング計算
	// @param pos：自身の位置
	// @param vel：自身の速度
	// @param neighbors：近隣情報リスト
	// @return ステアリングベクトル
	DirectX::XMFLOAT3 ComputeGroupSteering(
		const DirectX::XMFLOAT3& pos,
		const DirectX::XMFLOAT3& vel,
		const std::vector<BoidsNeighbor>& neighbors)
	{
		// Boidsステアリング計算を呼び出し
		return BoidsSteering::Compute(pos, vel, neighbors, m_BoidsParams);
	}
protected:

	// @brief ベクトルの長さを制限
	// @param v：ベクトル
	// @param maxLen：最大長さ
	// @return 制限後ベクトル
	static DirectX::XMFLOAT3 Limit(const DirectX::XMFLOAT3& v, float maxLen)
	{
		float len = StructMath::Length(v);
		if (len > maxLen && len > 0.0001f) return v * (maxLen / len);
		return v;
	}

protected:
	// @brief Boidsパラメータ
	BoidsParams m_BoidsParams;
};

