/**************************************************//*
	@file	| Item.cpp
	@brief	| アイテムクラス実装
	@note	| アイテムに関する情報を管理するクラスの実装
*//**************************************************/
#include "Item.h"
#include "Item_Material.h"
#include "Main.h"

/****************************************//*
	@brief　	| コンストラクタ
	@param　	| In_eType：アイテムタイプ
*//****************************************/
CItem::CItem(ITEM_TYPE In_eType)
	: m_eItemType(In_eType)
{
	// アイテムカテゴリーの設定
	m_eItemCategory = GetItemCategoryFromType(In_eType);
}

/****************************************//*
	@brief　	| デストラクタ
*//****************************************/
CItem::~CItem()
{
}

/****************************************//*
	@brief　	| アイテムタイプから空腹回復値を取得する関数
	@param　	| eType：アイテムタイプ
	@return　	| 空腹回復値
*//****************************************/
float CItem::GetHungerRecoveryValue(CItem::ITEM_TYPE eType)
{
	switch (eType)
	{
	case CItem::ITEM_TYPE::Meat:
		return 40.0f; // 肉の空腹回復値
	case CItem::ITEM_TYPE::Apple:
		return 30.0f; // リンゴの空腹回復値
	case CItem::ITEM_TYPE::ApplePie:
		return 100.0f; // アップルパイの空腹回復値(材料のリンゴ3個分より高くして、調理する価値を持たせる)
	case CItem::ITEM_TYPE::Bread:
		return 50.0f; // パンの空腹回復値
	default:
		return 0.0f;  // その他のアイテムは空腹回復値なし
	}
}
