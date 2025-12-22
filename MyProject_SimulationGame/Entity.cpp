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
	, m_fStamina(Job_Max_Stamina)
	, m_fMaxStamina(Job_Max_Stamina)
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
	@brief　	| インスペクター表示処理
	@param		| isEnd：true:ImGuiのEnd()を呼ぶ false:呼ばない
	@return		| 表示した項目数
*//****************************************/
int CEntity::Inspecter(bool isEnd)
{
	int nItemCount = 0;

	// 基底クラスのインスペクター表示処理
	nItemCount += CGameObject::Inspecter(false);

	// ステータス表示
	if (ImGui::CollapsingHeader(std::string(u8"[ステータス]").c_str()))
	{
		// 体力
		float fMaxHealth = m_fMaxHealth;
		float fCurrentHealth = m_fHealth;
		ImGui::ProgressBar(fCurrentHealth / fMaxHealth, ImVec2(0.0f, 0.0f), u8"体力");

		// 空腹度
		float fMaxHunger = Max_Hunger;
		float fCurrentHunger = m_fHunger;
		ImGui::ProgressBar(fCurrentHunger / fMaxHunger, ImVec2(0.0f, 0.0f), u8"空腹度");

		// スタミナ
		float fMaxStamina = m_fMaxStamina;
		float fCurrentStamina = m_fStamina;
		ImGui::ProgressBar(fCurrentStamina / fMaxStamina, ImVec2(0.0f, 0.0f), u8"スタミナ");
	}
	
	if(isEnd)
	{
		// 子要素の終了
		ImGui::EndChild();
		ImGui::End();
		// 表示項目のカウントを増やす
		nItemCount++;
	}

	return nItemCount;
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
	@brief　	| 空腹度の回復
	@param		| fAmount：回復量
*//****************************************/
void CEntity::RecoverHunger(float fAmount)
{
	// 空腹度を回復
	m_fHunger += fAmount;

	// 空腹度が最大値を超えないように補正
	if (m_fHunger > Max_Hunger)
	{
		m_fHunger = Max_Hunger;
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

/****************************************//*
	@brief	| ダメージを受ける
	@param	| fDamage : ダメージ量
*//****************************************/
void CEntity::TakeDamage(float fDamage)
{
	m_fHealth -= fDamage;
	if (m_fHealth <= 0.0f)
	{
		m_fHealth =0.0f;
		Destroy();
	}
}