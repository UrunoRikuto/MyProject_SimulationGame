/**************************************************//*
	@file	| BlackSmith.h
	@brief	| 鍛冶屋クラスのhファイル
	@note	| 鍛冶屋オブジェクトの処理を定義
			| CBuildObjectを継承
*//**************************************************/
#pragma once
#include "BuildObject.h"
#include "Item.h"

// @brief 鍛冶屋クラス
class CBlackSmith : public CBuildObject
{
public:
	// @brief コンストラクタ
	CBlackSmith();

	// @brief デストラクタ
	virtual ~CBlackSmith();

	// @brief 初期化処理
	virtual void Init() override;

	// @brief インスペクター表示処理
	// @return 表示した項目数
	virtual int Inspecter() override;

	// @brief 指定の道具の生産依頼があるかどうかを取得
	// @param eToolType：道具のアイテムタイプ
	// @return true:依頼がある false:依頼がない
	bool HasRequestTool(CItem::ITEM_TYPE eToolType) const;

	// @brief 生産依頼の追加
	// @param eToolType：依頼する道具のアイテムタイプ
	void AddRequestTool(CItem::ITEM_TYPE eToolType);

private:
	// @brief 生産依頼アイテムリスト
	std::list<CItem::ITEM_TYPE> m_vRequestToolList;
};

