/**************************************************//*
	@file	| Neet_Job.h
	@brief	| 無職クラスのhファイル
	@note	| 無職の処理を定義
			| IJob_Strategyを継承
*//**************************************************/
#pragma once
#include "Job_Strategy.h"

// @brief 職業名を管理する名前空間
namespace JobName
{
	const std::string Neet = u8"無職";
}

// @brief 無職クラス
class CNeet_Job : public IJob_Strategy
{
public:
	// @brief コンストラクタ
	CNeet_Job();

	// @brief 職業ごとの仕事処理
	void DoWork() override;

	// @brief 職業ごとの切り替え処理
	void OnChangeJob() override;

	// @brief インスペクター表示処理
	int Inspecter(bool isEnd = true) override;

	// @brief 職業ステータスのImGui描画処理
	void DrawJobStatusImGui() override;

	// @brief 職業名を取得
	// @return 職業名の文字列
	std::string GetJobName() const override { return JobName::Neet; }
};

