/**************************************************//*
	@file	| Crafter_Strategy.h
	@brief	| 加工職業クラスのhファイル
	@note	| 加工職業の処理を定義
			| IJob_Strategyを継承
*//**************************************************/
#pragma once
#include "Job_Strategy.h"

// @brief 加工職業クラス
class CCrafter_Strategy : public IJob_Strategy
{
	public:
	// @brief 仮想デストラクタ
	virtual ~CCrafter_Strategy() = default;
};