/**************************************************//*
	@file	| FieldGround.h
	@brief	| 地面クラスのhファイル
	@note	| 地面の処理を定義
			| CGameObjectを継承
*//**************************************************/
#pragma once
#include "GameObject.h"

// @brief 地面クラス
class CFieldGround : public CGameObject
{
public:
	// @brief コンストラクタ
	CFieldGround();

	// @brief デストラクタ
	virtual ~CFieldGround();

	// @brief 初期化処理
	virtual void Init() override;
};

