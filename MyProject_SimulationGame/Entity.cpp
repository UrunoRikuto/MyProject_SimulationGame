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
	, m_fThreat(0.0f)
	, m_fDefaultThreat(0.0f)
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

		// 脅威度上昇量を計算して設定
		// ーーーーーーーーーーーーーーー
		// 空腹時の上昇値 * 空腹度の不足分
		// 例：空腹度が10の場合
		// (脅威度上昇値1.0) * (不足分10) = (脅威度上昇量10.0)
		// ーーーーーーーーーーーーーーー
		float fHungerUpThreat = ThreatLevels::Hunger_Increase_Amount * fHungerDeficit;
		m_fThreat = m_fDefaultThreat + fHungerUpThreat;
	}
	// 空腹状態でないなら脅威度を初期値に戻す
	else m_fThreat = m_fDefaultThreat;

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
