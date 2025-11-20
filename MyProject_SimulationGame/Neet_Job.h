/**************************************************//*
	@file	| Neet_Job.h
	@brief	| 無職クラスのhファイル
	@note	| 無職の処理を定義
			| IJob_Strategyを継承
*//**************************************************/
#pragma once
#include "Job_Strategy.h"
#include "JobOperator.h"

// @brief 無職クラス
class CNeet_Job : public IJob_Strategy
{
public:
	// @brief 職業ごとの仕事処理のオーバーライド関数
	void DoWork() override;

	// @brief 職業名を取得するオーバーライド関数
	// @return 職業名の文字列
	std::string GetJobName() const override { return JobName::Neet; }
};

