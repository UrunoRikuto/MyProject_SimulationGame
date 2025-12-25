/**************************************************//*
	@file	| CameraDebug.h
	@brief	| デバッグカメラクラスのヘッダーファイル
*//**************************************************/
#pragma once
#include "Camera.h"

// デバッグカメラクラス
class CCameraDebug final: public CCamera
{
public:
	// @brief コンストラクタ
	CCameraDebug();

	// @brief デストラクタ
	virtual ~CCameraDebug();

	// @brief 初期化処理
	virtual void Init() override final;

	// @brief 更新処理
	virtual void Update() final;
private:
	// @brief 回転角度XZ平面
	float m_fRadXZ;
	// @brief 回転角度Y軸
	float m_fRadY;
	// @brief カメラの距離
	float m_fRadius;
};