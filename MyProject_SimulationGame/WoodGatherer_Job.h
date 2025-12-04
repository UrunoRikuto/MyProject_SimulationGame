/**************************************************//*
	@file	| WoodGatherer_Job.h
	@brief	| 木を探して収集する職業クラスのhファイル
	@note	| 木を探して収集する職業の処理を定義
			| CCollect_Strategyを継承
*//**************************************************/
#pragma once
#include "Gatherer_Strategy.h"
#include "JobOperator.h"

// @brief 木を探して収集する職業クラス
class CWoodGatherer_Job final : public CGatherer_Strategy
{
public:
	// @brief コンストラクタ
	CWoodGatherer_Job();

	// @brief 職業名を取得するオーバーライド関数
	// @return 職業名の文字列
	std::string GetJobName() const override { return JobName::WoodGatherer; }

	// @brief 採取ツールを持っているかどうかを取得
	// @return true:持っている false:持っていない
	virtual bool HasCollectTool() override;

	// @brief 自身が必要な収集ツールのアイテムタイプを取得するオーバーライド関数
	// @return 斧のアイテムタイプ
	CItem::ITEM_TYPE GetRequiredCollectToolType() const override { return CItem::ITEM_TYPE::Axe; }

private:

	// @brief 標的を探す処理
	// @return 採取対象オブジェクトのポインタ
	CCollectTarget* SearchTarget(std::vector<ObjectID> vNotTargetIDs) override;
};

