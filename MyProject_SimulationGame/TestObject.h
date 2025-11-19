/**************************************************//*
	@file	| TestObject.h
	@brief	| テスト用ゲームオブジェクトクラスのhファイル
	@note	| CGameObjectを継承
*//**************************************************/
#pragma once
#include "GameObject.h"

// @brief テスト用ゲームオブジェクトクラス
class CTestObject : public CGameObject
{
public:
	// @brief コンストラクタ
	CTestObject();

	// @brief デストラクタ
	~CTestObject();

	// @brief 初期化処理
	void Init() override;

	// @brief 更新処理
	void Update() override;

};

