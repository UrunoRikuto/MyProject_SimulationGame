/**************************************************//*
	@file	| SpriteObject.h
	@brief	| スプライトオブジェクトクラスのhファイル
	@note	| スプライトオブジェクトクラス
			| CGameObjectを継承
*//**************************************************/
#pragma once
#include "GameObject.h"

// @brief スプライトオブジェクトクラス
class CSpriteObject : public CGameObject
{
public:

	// @brief コンストラクタ
	CSpriteObject();

	// @brief デストラクタ
	~CSpriteObject();

	// @brief 初期化処理
	void Init() override;
};

