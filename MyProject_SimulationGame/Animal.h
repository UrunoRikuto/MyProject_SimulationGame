/**************************************************//*
	@file	| Animal.h
	@brief	| 動物基底クラスのhファイル
	@note	| 動物の基底クラスを定義
			| CEntityを継承
*//**************************************************/
#pragma once
#include "Entity.h"
#include "AnimalAI.h"
#include "ModelRenderer.h"

// @brief 動物基底クラス
class CAnimal : public CEntity
{
public:
	// @brief コンストラクタ
	CAnimal();

	// @brief デストラクタ
	~CAnimal();

protected:
	// @brief 動物の速度
	DirectX::XMFLOAT3 m_f3Velocity;

	// @brief 周りにいる同種の動物リスト
	std::vector<BoidsNeighbor> m_SameAnimalNeighbors;

	// @brief 動物の行動AI
	CAnimalAI* m_pActionAI;
};

