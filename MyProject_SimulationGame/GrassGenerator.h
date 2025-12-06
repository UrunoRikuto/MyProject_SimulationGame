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
private:
	// @brief 生成処理
	void Generate() override;
};

