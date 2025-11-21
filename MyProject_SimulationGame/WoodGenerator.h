/**************************************************//*
	@file	| WoodGenerator.h
	@brief	| 木材ジェネレータークラスのhファイル
	@note	| 木材を生成するジェネレーター
			| IGeneratorを継承
*//**************************************************/
#pragma once
#include "Generator.h"

// @brief 木材ジェネレータークラス
class CWoodGenerator final : public IGenerator
{
private:
	// @brief 生成処理
	void Generate() override;
};
