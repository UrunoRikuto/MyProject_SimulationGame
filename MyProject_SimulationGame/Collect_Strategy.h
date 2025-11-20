/**************************************************//*
	@file	| Collect_Strategy.h
	@brief	| 収集職業クラスのhファイル
	@note	| 収集職業の処理を定義
			| IJob_Strategyを継承
*//**************************************************/
#pragma once
#include "Job_Strategy.h"

// @brief 収集職業クラス
class CCollect_Strategy : public IJob_Strategy
{
public:
	// @brief 仮想デストラクタ
	virtual ~CCollect_Strategy() = default;
};

