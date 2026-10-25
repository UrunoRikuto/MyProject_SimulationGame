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

	// @brief セルの登録
	void RegisterToCell(DirectX::XMINT2 In_n2Cell) { m_n2BornCellIndex = In_n2Cell; }

	// @brief 群れの情報(近隣の位置・速度・標的)を毎フレーム最新に更新するかどうか
	// @note デバッグ用。falseにすると、生成時にコピーした情報のままで動作する(従来の動作)
	inline static bool s_bRefreshFlockInfo = true;

protected:
	// @brief 生誕した場所のセルインデックス
	DirectX::XMINT2 m_n2BornCellIndex;

	// @brief 動物の速度
	DirectX::XMFLOAT3 m_f3Velocity;

	// @brief 周りにいる同種の動物リスト
	std::vector<BoidsNeighbor> m_SameAnimalNeighbors;

	// @brief 動物の行動AI
	CAnimalAI* m_pActionAI;
};

