/**************************************************//*
	@file	| Wolf_Animal.h
	@brief	| オオカミクラスの定義
	@note	| オオカミの処理を定義
*//**************************************************/
#pragma once
#include "CarnivorousAnimal.h"


// @brief オオカミクラス
class CWolf_Animal final: public CCarnivorousAnimal
{
public:
	// @brief コンストラクタ
	CWolf_Animal();

	// @brief デストラクタ
	~CWolf_Animal()override;

	// @brief 初期化処理
	void Init() override;

	// @brief 更新処理
	void Update() override;

	// @brief 標的の設定
	void SetTarget();

	// @brief 群れの登録
	// @param In_Wolfs：オオカミリスト
	void RegisterToFlock(std::vector<CWolf_Animal*> In_Wolfs);

private:
	// @brief 攻撃クールダウン（秒）
	float m_fAttackCooldown =0.0f;
};

