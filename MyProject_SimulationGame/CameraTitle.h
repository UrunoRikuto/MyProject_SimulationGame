/**************************************************//*
	@file	| CameraTitle.h
	@brief	| タイトルカメラクラスのヘッダーファイル
*//**************************************************/
#pragma once
#include "Camera.h"

// @brief タイトルカメラクラス
class CCameraTitle : public CCamera
{
public:
	// @brief コンストラクタ
	CCameraTitle();

	// @brief デストラクタ
	virtual ~CCameraTitle();

	// @brief 初期化処理
	virtual void Init() override final;

	// @brief 更新処理
	virtual void Update() final;
};

