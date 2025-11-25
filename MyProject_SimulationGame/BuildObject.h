/**************************************************//*
	@file	| BuildObject.h
	@brief	| 建築オブジェクトクラスのhファイル
	@note	| 建築オブジェクトの処理を定義
			| CGameObjectを継承
*//**************************************************/
#pragma once
#include "GameObject.h"

// @brief 建築オブジェクトクラス
class CBuildObject : public CGameObject
{
public:
	// 建築物レベルの最大値
	static constexpr int MAX_BUILD_LEVEL = 5;

public:
	// @brief コンストラクタ
	CBuildObject();

	// @brief デストラクタ
	virtual ~CBuildObject();

	// @brief 初期化処理
	virtual void Init() override;

	// @brief インスペクター表示処理
	virtual int Inspecter(bool isEnd = true) override;

protected:

	// @brief 建築物レベル
	int m_nBuildLevel;
};

