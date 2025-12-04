/**************************************************//*
	@file	| Builder_Job.h
	@brief	| 建築職業クラスのhファイル
	@note	| 建築職業の処理を定義
			| CCrafter_Strategyを継承
*//**************************************************/
#pragma once
#include "Crafter_Strategy.h"
#include "JobOperator.h"
#include "BillboardRenderer.h"
#include "BuildManager.h"

// @brief 建築職業クラス
class CBuilder_Job final: public CCrafter_Strategy
{
private:
	// @brief 仕事状態
	enum class WorkState
	{
		// 待機中
		Waiting,
		// 素材収集中
		GatheringMaterials,
		// 対象探索中
		SearchingTarget,
		// 建築中
		Building,
		// 強化中
		Upgrading,
		// 休憩中
		Resting
	};

	// @brief クールタイム時間
	static constexpr float COOL_TIME_DURATION = 20.0f;

public:
	// @brief コンストラクタ
	CBuilder_Job();

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
	std::string GetJobName() const override { return JobName::Builder; }

private:
	// @brief 依頼の詳細のImGui表示
	void DrawBuildRequestDetailImGui();


	// @brief 待機中の処理
	void WaitingAction();

	// @brief 素材収集中の処理
	void GatherMaterialsAction();

	// @brief 対象探索中の処理
	void SearchingTargetAction();

	// @brief 建築中の処理
	void BuildingAction();

	// @brief 強化中の処理
	void UpgradingAction();

	// @brief 休憩中の処理
	void RestingAction();

private:
	// @brief 現在の仕事状態
	WorkState m_eCurrentState = WorkState::Resting;
	WorkState m_ePrevState = WorkState::Resting;

	// @brief 受けている建築依頼のポインタ
	CBuildManager::BuildRequest* m_pCurrentBuildRequest = nullptr;

	// @brief 建築中のビルボードレンダラー
	CBillboardRenderer* m_pBuildingBillboard = nullptr;

	// @brief 建築中の建築オブジェクトのポインタ
	CBuildObject* m_pBuildingObject = nullptr;

	// @brief クールタイム
	// @note 次の依頼を受けるまでの待機時間
	float m_fCoolTime = 0.0f;

	// @brief 依頼の詳細表示フラグ
	bool m_isShowRequestDetail = false;

};

