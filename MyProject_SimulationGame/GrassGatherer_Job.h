/**************************************************//*
	@file	| GrassGatherer.h
	@brief	| 草を探して収集する職業クラスのhファイル
	@note	| 草を探して収集する職業の処理を定義
			| CGatherer_Strategyを継承
*//**************************************************/
#pragma once
#include "Gatherer_Strategy.h"

// @brief 職業名を管理する名前空間
namespace JobName
{
	const std::string GrassGatherer = "GrassGatherer";
}

// @brief 草を探して収集する職業クラス
class CGrassGatherer_Job : public CGatherer_Strategy
{
public:
	// @brief コンストラクタ
	CGrassGatherer_Job();

	// @brief 職業名を取得するオーバーライド関数
	// @return 職業名の文字列
	std::string GetJobName() const override { return JobName::GrassGatherer; }

	// @brief 採取ツールを持っているかどうかを取得
	// @return true:持っている false:持っていない
	virtual bool HasCollectTool() override;

	// @brief 自身が必要な収集ツールのアイテムタイプを取得するオーバーライド関数
	// @return ナイフのアイテムタイプ
	CItem::ITEM_TYPE GetRequiredCollectToolType() const override { return CItem::ITEM_TYPE::Sickle; }

private:

	// @brief 標的を探す処理
	// @return 採取対象オブジェクトのポインタ
	CCollectTarget* SearchTarget(std::vector<ObjectID> vNotTargetIDs) override;

};

