/**************************************************//*
	@file	| CarnivorousAnimal.h
	@brief	| 肉食動物クラス定義
	@note	| 肉食の動物の振る舞いを定義
			| CAnimalを継承
*//**************************************************/
#pragma once
#include "Animal.h"

// @brief 肉食動物クラス
class CCarnivorousAnimal : public CAnimal
{
public:
	// @brief コンストラクタ
	CCarnivorousAnimal();

	// @brief デストラクタ
	~CCarnivorousAnimal();
};

