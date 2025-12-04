/**************************************************//*
	@file	| Collect_Strategy.h
	@brief	| 収集職業クラスのhファイル
	@note	| 収集職業の処理を定義
			| IJob_Strategyを継承
*//**************************************************/
#pragma once
#include "Job_Strategy.h"
#include "Item.h"

// @brief 収集職業クラス
class CCollect_Strategy : public IJob_Strategy
{
public:
	// @brief 仮想デストラクタ
	virtual ~CCollect_Strategy() = default;

	// @brief 収集ツールを取得する関数
	// @return true:持っている false:持っていない
	virtual bool HasCollectTool() = 0;

	// @brief 自身が必要な収集ツールのアイテムタイプを取得する純粋仮想関数
	virtual CItem::ITEM_TYPE GetRequiredCollectToolType() const = 0;
	
protected:
	// @brief 収集ツール
	CItem* m_pCollectItem = nullptr;
};

