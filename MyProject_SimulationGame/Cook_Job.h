/**************************************************//*
	@file	| Cook_Job.h
	@brief	| 料理職業クラスのhファイル
	@note	| 料理職業の処理を定義
			| CCrafter_Strategyを継承
*//**************************************************/
#pragma once
#include "Crafter_Strategy.h"
#include "FoodFactory.h"

// @brief 職業名を管理する名前空間
namespace JobName
{
	const std::string Cook = u8"料理人";
}

// @brief 料理職業クラス
class CCook_Job : public CCrafter_Strategy
{
public:
	// @brief 仕事状態
	enum class WorkState
	{
		// 待機中
		Waiting,
		// 素材収集中
		GatheringMaterials,
		// 料理中
		Cooking,
		// 料理運搬中
		TransportingFood,
		// 休憩中
		Resting
	};

	// @brief クールタイム時間
	static constexpr float COOL_TIME_DURATION = 10.0f;

public:
	// @brief コンストラクタ
	CCook_Job();

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
	std::string GetJobName() const override { return JobName::Cook; }

private:

	// @brief 待機処理
	void WaitingAction();

	// @brief 素材収集処理
	void GatheringMaterialsAction();

	// @brief 料理処理
	void CookingAction();

	// @brief 料理運搬処理
	void TransportingFoodAction();

	// @brief 休憩処理
	void RestingAction();

private:
	// @brief 現在の仕事状態
	WorkState m_eCurrentState = WorkState::Resting;
	WorkState m_ePrevState = WorkState::Resting;

	// @brief 受けている料理依頼のポインタ
	CFoodFactory::CookRequest* m_pRequest = nullptr;
	// @brief 料理工場のポインタ
	CFoodFactory* m_pFoodFactory = nullptr;

	// @brief クールタイム時間
	float m_fCoolTime = 0.0f;

};

