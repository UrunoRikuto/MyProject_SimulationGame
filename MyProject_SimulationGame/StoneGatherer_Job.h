/**************************************************//*
	@file	| StoneGatherer_Job.h
	@brief	| 石を探して収集する職業クラスのhファイル
	@note	| 石を探して収集する職業の処理を定義
			| CCollect_Strategyを継承
*//**************************************************/
#pragma once
#include "Gatherer_Strategy.h"
#include "JobOperator.h"

// @brief 石を探して収集する職業クラス
class CStoneGatherer_Job : public CGatherer_Strategy
{
public:
	// @brief 職業名を取得するオーバーライド関数
	// @return 職業名の文字列
	std::string GetJobName() const override { return JobName::StoneGatherer; }

private:

	// @brief 標的を探す処理
	// @return 採取対象オブジェクトのポインタ
	CCollectTarget* SearchTarget(std::vector<ObjectID> vNotTargetIDs) override;

};

