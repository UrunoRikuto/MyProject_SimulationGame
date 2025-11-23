/**************************************************//*
	@file	| StorageHouse.h
	@brief	| 貯蔵庫クラスのhファイル
	@note	| 貯蔵庫の処理を定義
			| CBuildObjectを継承
*//**************************************************/
#pragma once
#include "BuildObject.h"
#include "Item.h"

// @brief 貯蔵庫クラス
class CStorageHouse final:  public CBuildObject
{
public:
	// @brief コンストラクタ
	CStorageHouse();

	// @brief デストラクタ
	virtual ~CStorageHouse();

	// @brief 初期化処理
	virtual void Init() override;

	// @brief インスペクター表示処理
	virtual int Inspecter(bool isEnd = true) override;

	// @brief アイテムの収納
	void StoreItem(CItem* pItem);

	// @brief 収納されているアイテムの取り出し
	CItem* TakeOutItem(CItem::ITEM_TYPE eType);

private:
	// @brief 収納されているアイテムのマップ
	std::list<CItem*> m_StoredItems;

};

