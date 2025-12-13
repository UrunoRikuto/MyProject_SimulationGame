/**************************************************//*
	@file	| GroupAI.h
	@brief	| 群れAI基底クラスの定義
	@note	| 群れ行動の共通処理を定義
*//**************************************************/
#pragma once
#include "AnimalAI.h"

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
	Vec3 ComputeGroupSteering(
		const Vec3& pos,
		const Vec3& vel,
		const std::vector<BoidsNeighbor>& neighbors)
	{
		// Boidsステアリング計算を呼び出し
		return BoidsSteering::Compute(pos, vel, neighbors, m_BoidsParams);
	}

protected:
	// @brief Boidsパラメータ
	BoidsParams m_BoidsParams;
};

