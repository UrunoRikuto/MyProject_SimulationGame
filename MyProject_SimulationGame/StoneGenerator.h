/**************************************************//*
	@file	| StoneGenerator.h
	@brief	| 石ジェネレータークラスのhファイル
	@note	| 石を生成するジェネレーター
			| IGeneratorを継承
*//**************************************************/
#pragma once
#include "Generator.h"

// @brief 石ジェネレータークラス
class CStoneGenerator final : public IGenerator
{
private:
	// @brief 生成処理
	void Generate() override;
};

