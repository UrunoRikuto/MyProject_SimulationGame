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
private:
	// @brief 生成処理
	void Generate() override;
};

