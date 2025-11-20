/**************************************************//*
	@file	| WoodCollect_Job.h
	@brief	| 木材収集職業クラスのhファイル
	@note	| 木材収集職業の処理を定義
			| CCollect_Strategyを継承
*//**************************************************/
#pragma once
#include "Collect_Strategy.h"
#include "JobOperator.h"

// @brief 木材収集職業クラス
class CWoodCollect_Job final : public CCollect_Strategy
{
public:
	// @brief 職業ごとの仕事処理のオーバーライド関数
	void DoWork() override;

	// @brief 職業名を取得するオーバーライド関数
	// @return 職業名の文字列
	std::string GetJobName() const override { return JobName::WoodCollect; }
};

