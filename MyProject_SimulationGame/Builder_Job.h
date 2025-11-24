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

// @brief 建築職業クラス
class CBuilder_Job final: public CCrafter_Strategy
{
private:
	// @brief 仕事状態
	enum class WorkState
	{
		// 待機中
		Waiting,
		// 建築対象探索中
		SearchingTarget,
		// 建築中
		Building,
		// 休憩中
		Resting
	};

public:
	// @brief コンストラクタ
	CBuilder_Job();

	// @brief 仕事処理
	virtual void DoWork() override;

	// @brief 職業名を取得するオーバーライド関数
	// @return 職業名の文字列
	std::string GetJobName() const override { return JobName::Builder; }

	// @brief 建築対象のフィールドセルインデックスを取得
	DirectX::XMINT2 GetBuildTargetIndex() const { return m_n2BuildTargetIndex; }

private:
	// @brief 現在の仕事状態
	WorkState m_eCurrentState = WorkState::Waiting;

	// @brief 建築対象のフィールドセルインデックス
	DirectX::XMINT2 m_n2BuildTargetIndex = DirectX::XMINT2(-1, -1);

	// @brief 建築中のビルボードレンダラー
	CBillboardRenderer* m_pBuildingBillboard = nullptr;

};

