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
public:
	// @brief 指定したセルに木を1本生成する処理
	// @param n2Cell：生成先のセルのインデックス
	void GenerateAt(const DirectX::XMINT2& n2Cell) override;

private:
	// @brief 生成処理
	void Generate() override;
};
