#pragma once
#include "HerbivorousAnimal.h"

// @brief シカクラス
class CDeer_Animal final : public CHerbivorousAnimal
{
public:
	CDeer_Animal();
	~CDeer_Animal() override;

	void Init() override;
	void Update() override;

	// @brief 脅威（肉食動物）を群れで共有して逃避開始
	void SetThreat();

	// @brief 群れの登録
	void RegisterToFlock(std::vector<CDeer_Animal*> In_Deers);

private:
	// @brief 逃避開始距離（群れの誰かがこの距離で見つけたら全員逃げ）
	float m_AlertRange = 15.0f;

	// @brief 逃避解除距離
	float m_ClearRange = 20.0f;

	// ------------------------------------------------------------
	// Idle/Wander 用（個体差＋立ち止まり）
	// ------------------------------------------------------------
	DirectX::XMFLOAT3 m_WanderDir{0.0f,0.0f,1.0f };
	float m_fWanderUpdateTimer =0.0f;
	float m_fIdleTimer =0.0f;
	float m_fNextWanderUpdate =1.0f;
	float m_fNextIdleChange =2.0f;
	bool m_bIdle = false;
	unsigned int m_uRandSeed =0;

	// @brief 個体毎の乱数（0..1）
	float Rand01();
};