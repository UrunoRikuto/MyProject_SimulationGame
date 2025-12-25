/**************************************************//*
	@file	| Deer_Animal.h
	@brief	| 鹿クラス定義
	@note	| 鹿の振る舞いを定義
			| CHerbivorousAnimalを継承
*//**************************************************/
#pragma once
#include "HerbivorousAnimal.h"

// @brief 鹿クラス
class CDeer_Animal final : public CHerbivorousAnimal
{
public:
	// @brief コンストラクタ
	CDeer_Animal();

	// @brief デストラクタ
	~CDeer_Animal() override;

	// @brief 初期化処理
	void Init() override;

	// @brief 更新処理
	void Update() override;

	// @brief 脅威の設定
	void SetThreat();

	// @brief 群れの登録
	// @param In_Deers：鹿リスト
	void RegisterToFlock(std::vector<CDeer_Animal*> In_Deers);

private:

	// @brief 0..1の乱数取得
	float Rand01();

private:

	// @brief 警戒範囲
	float m_AlertRange = 15.0f;

	// @brief クリア範囲
	float m_ClearRange = 20.0f;

	// @brief 徘徊方向
	DirectX::XMFLOAT3 m_WanderDir{0.0f,0.0f,1.0f };

	// @brief 徘徊と待機のタイマー
	float m_fWanderUpdateTimer =0.0f;
	float m_fIdleTimer =0.0f;

	// @brief 次回更新時間
	float m_fNextWanderUpdate =1.0f;
	float m_fNextIdleChange =2.0f;

	// @brief 待機中かどうかのフラグ
	bool m_bIdle = false;

	// @brief 乱数シード
	unsigned int m_uRandSeed =0;

};