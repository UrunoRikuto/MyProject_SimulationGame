/**************************************************//*
	@file	| Entity.h
	@brief	| エンティティオブジェクトクラスのhファイル
	@note	| エンティティオブジェクトに関する処理の実装
			| CGameObjectを継承
*//**************************************************/
#pragma once
#include "GameObject.h"

// @brief 最大空腹値
constexpr float Max_Hunger = 100.0f;
// @brief 満腹値(通常状態に移行する値(少し幅持たせるため100未満にする))
constexpr float Full_Hunger = 95.0f;
// @brief 空腹警告値(この値以下になると食事状態に移行)
constexpr float Warning_Hunger = 20.0f;
// @brief 自然空腹減少値
constexpr float Natural_Hunger_Decrease = 0.01f;

// @brief スタミナ値の最大値
constexpr float Job_Max_Stamina = 100.0f;

// @brief エンティティオブジェクトクラス
class CEntity : public CGameObject
{
public:
	// @brief コンストラクタ
	CEntity();

	// @brief デストラクタ
	~CEntity();

	// @brief 更新処理
	void Update() override;

	// @brief インスペクター表示処理
	int Inspecter(bool isEnd = true) override;

	// @brief 空腹度が満タンかどうかの取得
	// @return true:満タン false:満タンではない
	bool IsMaxHunger() const { return m_fHunger >= Max_Hunger; }
	// @brief 空腹度が満腹値以上かどうかの取得
	// @return true:満腹値以上 false:満腹値未満
	bool IsFullHunger() const { return m_fHunger >= Full_Hunger; }
	// @brief 空腹度が警告値以下かどうかの取得
	// @return true:警告値以下 false:警告値以上
	bool IsWarningHunger() const { return m_fHunger <= Warning_Hunger; }
	// @brief 空腹度の取得
	// @return 空腹度
	float GetHunger() const { return m_fHunger; }
	// @brief 空腹度の減少
	// @param fAmount：減少量
	void DecreaseHunger(float fAmount);
	// @brief 空腹度の回復
	// @param fAmount：回復量
	void RecoverHunger(float fAmount);

	// @brief 体力の取得
	float GetHealth() const { return m_fHealth; }
	// @brief 最大体力の取得
	float GetMaxHealth() const { return m_fMaxHealth; }
	// @brief 死亡しているかどうかの取得
	bool IsDead() const { return m_fHealth <= 0.0f; }

	// @brief ダメージを受ける
	// @param fDamage : ダメージ量
	void TakeDamage(float fDamage);

	// @brief スタミナの取得
	// @return スタミナ値
	float GetStamina() const { return m_fStamina; }
	// @brief 最大スタミナの取得
	// @return 最大スタミナ値
	float GetMaxStamina() const { return m_fMaxStamina; }
	// @brief スタミナが最大かどうかの取得
	// @return true:最大 false:最大ではない
	bool IsMaxStamina() const { return m_fStamina >= m_fMaxStamina; }
	// @brief スタミナが0かどうかの取得
	// @return true:0 false:0ではない
	bool IsZeroStamina() const { return m_fStamina <= 0.0f; }
	// @brief スタミナの減少
	// @param fAmount：減少量
	void DecreaseStamina(float fAmount);
	// @brief スタミナの回復
	// @param fAmount：回復量
	void RecoverStamina(float fAmount);

protected:
	// @brief 体力
	float m_fHealth;
	// @brief 体力最大値
	float m_fMaxHealth;

	// @brief 空腹度
	float m_fHunger;
	// @brief 食事フラグ
	bool m_isEating;

	// @brief スタミナ
	float m_fStamina;
	// @brief 最大スタミナ
	float m_fMaxStamina;
};