/**************************************************//*
	@file	| TitleObject.h
	@brief	| タイトルシーン用ゲームオブジェクトのhファイル
	@note	| タイトルシーン用のゲームオブジェクトクラス
			| CGameObjectを継承
*//**************************************************/
#pragma once
#include "GameObject.h"

// @brief タイトルシーン用ゲームオブジェクトクラス
class CTitleObject : public CGameObject
{
public:
	// @brief コンストラクタ
	CTitleObject();

	// @brief デストラクタ
	~CTitleObject();

	// @brief 関数登録
	void SetFunction(std::function<void()> inFunc) { m_onFunc = inFunc; }

	// @brief 実行
	void Execute();

private:
	// @brief 登録されている関数
	std::function<void()> m_onFunc;
};

