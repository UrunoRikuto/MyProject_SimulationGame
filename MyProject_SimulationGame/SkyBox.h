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
private:
	// 時間別色パラメータ
	DirectX::XMFLOAT4 SkyBoxColors[4] =
	{
		{ 0.5f, 0.7f, 1.0f, 1.0f }, // 朝
		{ 0.2f, 0.6f, 1.0f, 1.0f }, // 昼
		{ 0.9f, 0.5f, 0.3f, 1.0f }, // 夕方
		{ 0.05f, 0.05f, 0.2f, 1.0f }  // 夜
	};

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

