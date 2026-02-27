/**************************************************//*
	@file	| FieldGround.h
	@brief	| フィールド地面クラスのヘッダファイル
	@note	| 地面オブジェクト（シーンに配置されるCGameObjectの派生）
*//**************************************************/
#pragma once
#include "GameObject.h"

// @brief フィールド地面オブジェクト
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

