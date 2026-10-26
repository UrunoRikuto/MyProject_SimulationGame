/**************************************************//*
	@file	| GrassGenerator.h
	@brief	| 草ジェネレータークラスのhファイル
	@note	| 草を生成するジェネレーター
			| IGeneratorを継承
*//**************************************************/
#pragma once
#include "Generator.h"

// @brief 草ジェネレータークラス
class CGrassGenerator : public IGenerator
{
public:
	// @brief 指定したセルに草を1つ生成する処理
	// @param n2Cell：生成先のセルのインデックス
	void GenerateAt(const DirectX::XMINT2& n2Cell) override;

private:
	// @brief 生成処理
	void Generate() override;
};
