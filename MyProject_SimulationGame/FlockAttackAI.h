/**************************************************//*
	@file	| FlockAttackAI.h
	@brief	| 群れ攻撃AIクラスの定義
	@note	| 群れで攻撃行動を行うAIを定義
*//**************************************************/
#pragma once
#include "GroupAI.h"

// @brief 群れ攻撃AIクラス
class CFlockAttackAI : public CGroupAI
{
public:
	// @brief 引数付きコンストラクタ
	CFlockAttackAI(const BoidsParams& params);

	// @brief 動物の行動を更新
	// @param pos：動物の現在位置
	// @param vel：動物の現在速度
	// @param neighbors：近隣の動物情報リスト
	// @return ステアリングベクトル
	Vec3 UpdateAI(const Vec3& pos,const Vec3& vel,const std::vector<BoidsNeighbor>& neighbors) override;

	// @brief 標的位置設定
	// @param pPos：標的の位置ポインタ
	void SetTargetPosition(Vec3* pPos) { m_TargetPos = pPos; }

	// @brief 標的位置取得
	Vec3* GetTargetPosition() const { return m_TargetPos; }

private:
	// @brief 標的の位置
	Vec3* m_TargetPos;
};

