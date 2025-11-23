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
	// @brief コンストラクタ
	CBuildObject();

	// @brief デストラクタ
	virtual ~CBuildObject();

	// @brief 初期化処理
	virtual void Init() override;
};

