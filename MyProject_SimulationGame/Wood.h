/**************************************************//*
	@file	| Wood.h
	@brief	| 木オブジェクトクラスのhファイル
	@note	| CGameObjectを継承
*//**************************************************/
#pragma once
#include "GameObject.h"

// @brief 木オブジェクトクラス
class CWood : public CGameObject
{
public:
	// @brief コンストラクタ
	CWood();

	// @brief デストラクタ
	virtual ~CWood();

	// @brief 初期化処理
	virtual void Init() override;

	// @brief 終了処理
	virtual void Uninit() override;

	// @brief 描画処理
	virtual void Draw() override;
};

