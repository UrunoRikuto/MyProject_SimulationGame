/**************************************************//*
	@file	| AnimalGenerator.h
	@brief	| 動物ジェネレータークラスのhファイル
	@note	| 動物を生成するジェネレーター
			| IGeneratorを継承
*//**************************************************/
#pragma once
#include "Generator.h"

// @brief 動物ジェネレータークラス
class CAnimalGenerator : public IGenerator
{
private:
	// @brief 生成処理
	void Generate() override;
};

