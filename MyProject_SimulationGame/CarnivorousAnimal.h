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

	// @brief 攻撃力
	float GetAttack() const { return m_fAttack; }

protected:

	// @brief 攻撃力
	float m_fAttack;
	// @brief 攻撃間隔
	float m_fAttackInterval;
};

