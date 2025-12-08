/**************************************************//*
	@file	| Wolf.h
	@brief	| オオカミクラスのhファイル
	@note	| オオカミクラスを定義
			| CAnimalを継承
*//**************************************************/
#pragma once
#include "Animal.h"

// @brief オオカミクラス
class CWolf : public CAnimal
{
public:
	// @brief コンストラクタ
	CWolf();

	// @brief デストラクタ
	~CWolf();

	// @brief 更新処理
	void Update() override;
};

