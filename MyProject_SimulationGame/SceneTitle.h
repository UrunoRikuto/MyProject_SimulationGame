/**************************************************//*
	@file	| SceneTitle.h
	@brief	| タイトルシーンクラスのhファイル
	@note	| タイトルシーン内のゲームオブジェクト管理、更新、描画等を行う
			| CSceneを継承
*//**************************************************/
#pragma once
#include "Scene.h"
#include <vector>
#include "TitleObject.h"

// @brief タイトルシーンクラス
class CSceneTitle : public CScene
{
public:
	// @brief コンストラクタ
	CSceneTitle();
	
	// @brief デストラクタ
	~CSceneTitle();
	
	// @brief 初期化処理
	void Init() override;

	// @brief 終了処理
	void Uninit() override;
	
	// @brief 更新処理
	void Update() override;

	// @brief 描画処理
	void Draw() override;

private:
	// @brief シード値入力モードの移行
	void Mode_SeedInputMode();

private:
	// @brief メニュー選択インデックス
	int m_nSelectIndex = 0;

	// @brief メニュー選択肢オブジェクトリスト
	std::vector<CTitleObject*> m_pMenuObjectList;

	// @brief シード値入力モードフラグ
	bool m_bSeedInputMode = false;
};

