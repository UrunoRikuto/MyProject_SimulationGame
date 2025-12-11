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

	// @brief 体力の取得
	float GetHealth() const { return m_fHealth; }
	// @brief 最大体力の取得
	float GetMaxHealth() const { return m_fMaxHealth; }

	// @brief 脅威度の取得
	// @return 脅威度
	float GetThreat() const { return m_fThreat; }

protected:
	// @brief 体力
	float m_fHealth;
	// @brief 体力最大値
	float m_fMaxHealth;

	// @brief 空腹度
	float m_fHunger;
	// brief 食事フラグ
	bool m_isEating;

	// @brief 脅威度
	float m_fThreat;
	// @brief 脅威度の初期値
	float m_fDefaultThreat;
};

// @brief 脅威度の定義
// @note 0.0f～100.0fの範囲で設定
namespace ThreatLevels
{
	// @brief 脅威度の最大値
	constexpr float Max_Threat = 100.0f;

	// @brief 空腹値の増加量
	constexpr float Hunger_Increase_Amount = 1.0f;

	// @brief エンティティの初期値
	namespace Entity
	{
		// @brief 人間の脅威度
		constexpr float Human = 20.0f;

		// @brief オオカミの脅威度
		constexpr float Wolf = 10.0f;
	}
}