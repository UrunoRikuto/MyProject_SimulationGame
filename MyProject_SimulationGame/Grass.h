/**************************************************//*
	@file	| Grass.h
	@brief	| 草オブジェクトクラスのhファイル
	@note	| CGameObjectを継承
*//**************************************************/
#pragma once
#include "CollectTarget.h"

// @brief 草オブジェクトクラス
class CGrass : public CCollectTarget
{
public:
	// @brief コンストラクタ
	CGrass();

	// @brief デストラクタ
	virtual ~CGrass();

	// @brief 初期化処理
	virtual void Init() override;
};

