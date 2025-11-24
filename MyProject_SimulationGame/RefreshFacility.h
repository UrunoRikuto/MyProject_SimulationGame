/**************************************************//*
	@file	| RefreshFacility.h
	@brief	| 休憩施設クラスのhファイル
	@note	| 休憩施設の処理を定義
			| CBuildObjectを継承
*//**************************************************/
#pragma once
#include "BuildObject.h"
#include "Defines.h"

// @brief 休憩施設クラス
class CRefreshFacility : public CBuildObject
{
public:
	// @brief スタミナ回復量(マイフレーム)
	static constexpr float REFRESH_STAMINA_AMOUNT = 1.0f;

public:
	// @brief コンストラクタ
	CRefreshFacility();

	// @brief デストラクタ
	virtual ~CRefreshFacility();

	// @brief 初期化処理
	virtual void Init() override;

};

