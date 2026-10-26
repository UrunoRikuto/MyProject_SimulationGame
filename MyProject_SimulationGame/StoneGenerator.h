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
public:
	// @brief 指定したセルに石を1個生成する処理
	// @param n2Cell：生成先のセルのインデックス
	void GenerateAt(const DirectX::XMINT2& n2Cell) override;

private:
	// @brief 生成処理
	void Generate() override;
};
