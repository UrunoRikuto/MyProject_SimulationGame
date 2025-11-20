/**************************************************//*
	@file	| StoneCollect_Job.h
	@brief	| 石収集職業クラスのhファイル
	@note	| 石収集職業の処理を定義
			| CCollect_Strategyを継承
*//**************************************************/
#pragma once
#include "Gatherer_Strategy.h"
#include "JobOperator.h"

// @brief 石収集職業クラス
class CStoneCollect_Job : public CGatherer_Strategy
{
public:
	// @brief 職業名を取得するオーバーライド関数
	// @return 職業名の文字列
	std::string GetJobName() const override { return JobName::StoneCollect; }

private:

	// @brief 標的を探す処理
	void SearchTarget() override;

};

