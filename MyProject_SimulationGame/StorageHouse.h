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
	// @param isEnd：true:ImGuiのEnd()を呼ぶ false:呼ばない
	virtual int Inspecter(bool isEnd = true) override;

	// @brief アイテムの収納
	// @param pItem：収納するアイテムポインタ
	void StoreItem(CItem* pItem);

	// @brief 収納されているアイテムの取り出し
	// @param eType：取り出すアイテムタイプ
	// @return 取り出したアイテムポインタ、存在しなかった場合はnullptr
	CItem* TakeOutItem(CItem::ITEM_TYPE eType);

	// @brief 収納されているアイテムリストの取得
	// @return 収納されているアイテムリストの参照
	const std::list<CItem*>& GetStoredItems() const { return m_StoredItems; }


private:
	// @brief 収納されているアイテムのマップ
	std::list<CItem*> m_StoredItems;

};

