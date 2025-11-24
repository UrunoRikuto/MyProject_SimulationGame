/**************************************************//*
	@file	| SkyBox.h
	@brief	| スカイボックスクラスのヘッダファイル
	@note	| CGameObjectを継承
*//**************************************************/
#pragma once
#include "GameObject.h"

// @brief スカイボックスクラス
class CSkyBox : public CGameObject
{
public:
	// @brief コンストラクタ
	CSkyBox();

	// @brief デストラクタ
	virtual ~CSkyBox();

	// @brief 初期化処理
	virtual void Init() override;

	// @brief 更新処理
	virtual void Update() override;
};

