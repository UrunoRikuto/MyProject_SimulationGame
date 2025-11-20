/**************************************************//*
	@file	| WoodCollect_Job.h
	@brief	| 木材収集職業クラスのhファイル
	@note	| 木材収集職業の処理を定義
			| CCollect_Strategyを継承
*//**************************************************/
#pragma once
#include "Collect_Strategy.h"
#include "JobOperator.h"
#include "Wood.h"

// @brief 木材収集職業クラス
class CWoodCollect_Job final : public CCollect_Strategy
{
private:
	enum class WorkState
	{
		// 木を探して移動する状態
		SearchAndMove,
		// 木を収集する状態
		Collecting,
		// 休憩する状態
		Resting
	};

public:
	// @brief 職業ごとの仕事処理のオーバーライド関数
	void DoWork() override;

	// @brief 職業名を取得するオーバーライド関数
	// @return 職業名の文字列
	std::string GetJobName() const override { return JobName::WoodCollect; }

private:
	// @brief 現在の仕事状態
	WorkState m_eCurrentState = WorkState::SearchAndMove;

	// @brief 木オブジェクトへのポインタ
	CWood* m_pTargetWood = nullptr;
};

