/**************************************************//*
	@file	| CameraGame.h
	@brief	| ゲームカメラクラスのヘッダーファイル
*//**************************************************/
#pragma once
#include "Camera.h"

// ゲームカメラクラス
class CCameraGame final: public CCamera
{
public:
	// @brief コンストラクタ
	CCameraGame();

	// @brief デストラクタ
	virtual ~CCameraGame();

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
	// @brief 古いRadius保存用
	float m_fChangeOldRadius;
	// @brief 古いRadius保存フラグ
	bool m_bSetOldRadius = false;
};