/**************************************************//*
	@file	| CGatherer_Strategy.h
	@brief	| 採集職業クラスのhファイル
	@note	| 探して採取する職の処理を実装
			| CCollect_Strategyを継承
*//**************************************************/
#pragma once
#include "Collect_Strategy.h"
#include "CollectTarget.h"

// @brief 探して採取する採取職業クラス
class CGatherer_Strategy : public CCollect_Strategy
{
private:
	// @brief 仕事状態
	enum class WorkState
	{
		// 探して移動
		SearchAndMove,
		// 採取中
		Gathering,
		// 休憩中
		Resting
	};

public:
	// @brief 仕事処理
	virtual void DoWork() override;

protected:

	// @brief 標的を探す処理
	virtual CCollectTarget* SearchTarget(std::vector<ObjectID> vNotTargetIDs) = 0;

private:

	// @brief 現在の仕事状態
	WorkState m_eCurrentState = WorkState::SearchAndMove;

	// @brief 標的にしている採取対象オブジェクトのポインタ
	CCollectTarget* m_pTarget = nullptr;

};

