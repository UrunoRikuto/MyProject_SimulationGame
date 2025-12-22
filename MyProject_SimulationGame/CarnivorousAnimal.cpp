/**************************************************//*
	@file	| CarnivorousAnimal.cpp
	@brief	| 肉食動物クラス実装
	@note	| 肉食の動物の振る舞いを定義
			| CAnimalを継承
*//**************************************************/
#include "CarnivorousAnimal.h"

/****************************************//*
	@brief　	| コンストラクタ
*//****************************************/
CCarnivorousAnimal::CCarnivorousAnimal()
	: CAnimal()
	, m_fAttack(0.0f)
	, m_fAttackInterval(0.0f)
{
}

/****************************************//*
	@brief　	| デストラクタ
*//****************************************/
CCarnivorousAnimal::~CCarnivorousAnimal()
{
}
