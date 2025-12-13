/**************************************************//*
	@file	| Entity.cpp
	@brief	| エンティティオブジェクトクラスのcppファイル
	@note	| エンティティオブジェクトに関する処理の実装
			| CGameObjectを継承
*//**************************************************/
#include "Entity.h"
#include "ImguiSystem.h"

/****************************************//* 
	@brief　	| コンストラクタ
*//****************************************/
CEntity::CEntity()
	: CGameObject()
	, m_fHealth(100.0f)
	, m_fMaxHealth(100.0f)
	, m_fHunger(Max_Hunger)
	, m_isEating(false)
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

	// 空腹状態なら脅威度を上げる
	if (IsWarningHunger())
	{
		// 空腹度の不足分に応じて脅威度を上昇させる
		// ーーーーーーーーーーーーーーー
		// 警告値 - 空腹度 = 不足分
		// 例：空腹度が10の場合
		//(警告値20)-(空腹度10) = (不足分10)
		// ーーーーーーーーーーーーーーー
		float fHungerDeficit = Warning_Hunger - m_fHunger;
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

/****************************************//* 
	@brief　	| スタミナの減少
	@param		| fAmount：減少量
*//****************************************/
void CEntity::DecreaseStamina(float fAmount)
{
	// スタミナを減少
	m_fStamina -= fAmount;

	// スタミナが0未満にならないように補正
	if (m_fStamina < 0.0f)
	{
		m_fStamina = 0.0f;
	}
}

/****************************************//* 
	@brief　	| スタミナの回復
	@param		| fAmount：回復量
*//****************************************/
void CEntity::RecoverStamina(float fAmount)
{
	// スタミナを回復
	m_fStamina += fAmount;

	// スタミナが最大値を超えないように補正
	if (m_fStamina > m_fMaxStamina)
	{
		m_fStamina = m_fMaxStamina;
	}
}