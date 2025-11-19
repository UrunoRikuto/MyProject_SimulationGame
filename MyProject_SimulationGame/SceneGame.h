/**************************************************//*
	@file	| SceneGame.h
	@brief	| ゲームシーンクラスのhファイル
	@note	| ゲームシーン内のゲームオブジェクト管理、更新、描画等を行う
			| CSceneを継承
*//**************************************************/
#pragma once
#include "Scene.h"

// @brief ゲームシーンクラス
class CSceneGame : public CScene
{
public:
	// @brief コンストラクタ
	CSceneGame();

	// @brief デストラクタ
	~CSceneGame();

	// @brief 初期化処理
	void Init() override;

	// @brief 更新処理
	void Update() override;

	// @brief 描画処理
	void Draw() override;
};

