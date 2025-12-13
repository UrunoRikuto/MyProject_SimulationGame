/**************************************************//*
	@file	| HerbivorousAnimal.h
	@brief	| 草食動物クラス定義
	@note	| 草食の動物の振る舞いを定義
			| CAnimalを継承
*//**************************************************/
#pragma once
#include "Animal.h"

// @brief 草食動物クラス
class CHerbivorousAnimal : public CAnimal
{
public:
	// @brief コンストラクタ
	CHerbivorousAnimal();

	// @brief デストラクタ
	~CHerbivorousAnimal();
};

