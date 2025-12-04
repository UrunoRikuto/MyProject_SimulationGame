/**************************************************//*
	@file	| Smith_Job.h
	@brief	| 鍛冶職業クラスのhファイル
	@note	| 鍛冶職業の処理を定義
			| CCrafter_Strategyを継承
*//**************************************************/
#pragma once
#include "Crafter_Strategy.h"
#include "JobOperator.h"
#include "BlackSmith.h"

// @brief 鍛冶職業クラス
class CSmith_Job final: public CCrafter_Strategy
{
private:
	// @brief 仕事状態
	enum class WorkState
	{
		// 待機中
		Waiting,
		// 素材収集中
		GatheringMaterials,
		// 製作中
		Crafting,
		// 完成品運搬中
		TransportingFinishedGoods,
		// 休憩中
		Resting
	};

	// @brief クールタイム時間
	static constexpr float COOL_TIME_DURATION = 20.0f;

public:

	// @brief コンストラクタ
	CSmith_Job();

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
	std::string GetJobName() const override { return JobName::Smith; }

private:

	// @brief 待機処理
	void WaitingAction();

	// @brief 素材収集処理
	void GatheringMaterialsAction();

	// @brief 製作処理
	void CraftingAction();

	// @brief 完成品運搬処理
	void TransportingFinishedGoodsAction();

	// @brief 休憩処理
	void RestingAction();

private:
	// @brief 現在の仕事状態
	WorkState m_eCurrentState = WorkState::Resting;
	WorkState m_ePrevState = WorkState::Resting;

	// @brief 受けた生産依頼の道具タイプのポインタ
	CBlackSmith::ToolRequest* m_pRequest = nullptr;

	// @brief クールタイム
	// @note 次の依頼を受けるまでの待機時間
	float m_fCoolTime = 0.0f;
};

