/**************************************************//*
	@file	| Stone.h
	@brief	| 石オブジェクトクラスのhファイル
	@note	| CGameObjectを継承
*//**************************************************/
#pragma once
#include "GameObject.h"

// @brief 石オブジェクトクラス
class CStone final : public CGameObject
{
public:
	// @brief コンストラクタ
	CStone();

	// @brief デストラクタ
	virtual ~CStone();

	// @brief 初期化処理
	virtual void Init() override;
};

