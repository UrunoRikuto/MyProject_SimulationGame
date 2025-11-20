/**************************************************//*
	@file	| Human.h
	@brief	| 人間オブジェクトクラスのhファイル
	@note	| 人間オブジェクトに関する処理の実装
			| CGameObjectを継承
*//**************************************************/
#pragma once
#include "GameObject.h"

// @brief 人間オブジェクトクラス
class CHuman final : public CGameObject
{
public:
	// @brief コンストラクタ
	CHuman();

	// @brief デストラクタ
	~CHuman();

	// @brief 初期化処理
	void Init() override;

	// @brief 終了処理
	void Uninit() override;

	// @brief 更新処理
	void Update() override;

	// @brief 描画処理
	void Draw() override;

	// @brief インスペクター表示処理
	// @param isEnd：true:ImGuiのEnd()を呼ぶ false:呼ばない
	// @return 表示した項目数
	int Inspecter(bool isEnd = true) override;
};

