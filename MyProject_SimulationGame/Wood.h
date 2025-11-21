/**************************************************//*
	@file	| Wood.h
	@brief	| 木オブジェクトクラスのhファイル
	@note	| CGameObjectを継承
*//**************************************************/
#pragma once
#include "CollectTarget.h"

// @brief 木オブジェクトクラス
class CWood final: public CCollectTarget
{
public:
	// @brief コンストラクタ
	CWood();

	// @brief デストラクタ
	virtual ~CWood();

	// @brief 初期化処理
	virtual void Init() override;
};

