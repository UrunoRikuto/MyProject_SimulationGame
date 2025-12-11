/**************************************************//*
	@file	| Farmer_Job.h
	@brief	| 農夫の職業クラスヘッダ
	@note	| 農夫の職業クラスを定義
			| CCrafter_Strategyを継承
*//**************************************************/
#pragma once
#include "Crafter_Strategy.h"
#include "FarmFacility.h"

// @brief 職業名を管理する名前空間
namespace JobName
{
	const std::string Farmer = u8"農夫";
}

// @brief 農夫の職業クラス
class CFarmer_Job : public CCrafter_Strategy
{
private:
	// @brief 仕事状態
	enum class WorkState
	{
		// 作物の世話中
		TendingCrops,

		// 種を倉庫に取りに行く
		RetrievingSeeds,

		// 収穫中
		Harvesting,

		// 運搬中
		TransportingCrops,

		// 休憩中
		Resting
	};

public:

	// @brief コンストラクタ
	CFarmer_Job();

	// @brief 仕事処理
	virtual void DoWork() override;

	// @brief 切り替え処理
	virtual void OnChangeJob() override;

	// @brief インスペクター表示処理
	// @param isEnd：インスペクター終了フラグ
	virtual int Inspecter(bool isEnd = true) override;

	// @brief 職業ステータスのImGui描画処理
	virtual void DrawJobStatusImGui() override;

	// @brief 職業名を取得するオーバーライド関数
	// @return 職業名の文字列
	std::string GetJobName() const override { return JobName::Farmer; }

private:
	// @brief 必要な道具のアイテムのタイプを取得
	CItem::ITEM_TYPE GetRequiredToolType() const { return CItem::ITEM_TYPE::Sickle; }


	// @brief 作物の世話をする処理
	void TendingCropsAction();

	// @brief 種を倉庫に取りに行く処理
	void RetrievingSeedsAction();

	// @brief 収穫する処理
	void HarvestingAction();

	// @brief 作物を運搬する処理
	void TransportingCropsAction();

	// @brief 休憩する処理
	void RestingAction();

private:
	// @brief 現在の仕事状態
	WorkState m_eCurrentState;
	// @brief 前回の仕事状態
	WorkState m_ePrevState;

	// @brief 対象にしている農作施設のポインタ
	CFarmFacility* m_pTargetFarmFacility = nullptr;

};
