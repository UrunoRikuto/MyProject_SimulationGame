/**************************************************//*
	@file	| HumanGenerator.h
	@brief	| 人間ジェネレータークラスのhファイル
	@note	| 人間を生成するジェネレーター
			| IGeneratorを継承
*//**************************************************/
#pragma once
#include "Generator.h"

// @brief 人間ジェネレータークラス
class CHumanGenerator : public IGenerator
{
public:
	// @brief 更新処理
	// @note 家の空き枠を定期的に確認し、足りない分の生成リクエストを追加する
	void Update() override;

private:
	// @brief 生成処理
	void Generate() override;

	// @brief 空き枠の確認タイマー(秒)
	float m_fCheckTimer = 0.0f;
};
