/**************************************************//*
	@file	| AnimalAI.h
	@brief	| 動物AI基底クラスの定義
	@note	| 動物AIの抽象基底クラスを定義
*//**************************************************/
#pragma once
#include "BoidsSteering.h"

// @brief 動物AI基底クラス
class CAnimalAI
{
public:
	// @brief デストラクタ
	virtual ~CAnimalAI() = default;

	// @brief 動物の行動を更新
	// @param pos：動物の現在位置
	// @param vel：動物の現在速度
	// @param neighbors：近隣の動物情報リスト
	virtual DirectX::XMFLOAT3 UpdateAI(const DirectX::XMFLOAT3& pos,const DirectX::XMFLOAT3& vel,const std::vector<BoidsNeighbor>& neighbors) = 0;
};

