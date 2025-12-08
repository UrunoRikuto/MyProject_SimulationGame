/**************************************************//*
	@file	| Entity.cpp
	@brief	| エンティティオブジェクトクラスのcppファイル
	@note	| エンティティオブジェクトに関する処理の実装
			| CGameObjectを継承
*//**************************************************/
#include "Entity.h"


/****************************************//* 
	@brief　	| コンストラクタ
*//****************************************/
CEntity::CEntity()
	: CGameObject()
	, m_fHealth(100.0f)
	, m_fMaxHealth(100.0f)
	, m_fHunger(Max_Hunger)
	, m_isEating(false)
	, m_fThreat(0.0f)
{
}

/****************************************//* 
	@brief　	| デストラクタ
*//****************************************/
CEntity::~CEntity()
{
}

/****************************************//* 
	@brief　	| 更新処理
*//****************************************/
void CEntity::Update()
{
	// 体力が0以下なら死亡処理
	if (m_fHealth <= 0.0f)
	{
		// オブジェクト破棄
		Destroy();
		return;
	}

	// 基底クラスの更新処理
	CGameObject::Update();
}

/****************************************//*
	@brief　	| 空腹度の減少
	@param		| fAmount：減少量
*//****************************************/
void CEntity::DecreaseHunger(float fAmount)
{
	// 空腹度を減少
	m_fHunger -= fAmount;

	// 空腹度が0未満にならないように補正
	if (m_fHunger < 0.0f)
	{
		m_fHunger = 0.0f;
	}
}
