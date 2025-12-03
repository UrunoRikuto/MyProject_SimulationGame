/**************************************************//*
	@file	| Item.cpp
	@brief	| アイテムクラス実装
	@note	| アイテムに関する情報を管理するクラスの実装
*//**************************************************/
#include "Item.h"
#include "Main.h"

/****************************************//*
	@brief　	| コンストラクタ
	@param　	| In_eType：アイテムタイプ
*//****************************************/
CItem::CItem(ITEM_TYPE In_eType)
	: m_eItemType(In_eType)
{
	switch (In_eType)
	{
	case ITEM_TYPE::Wood:
	case ITEM_TYPE::Stone:
	case ITEM_TYPE::Iron:
		m_eItemCategory = ITEM_CATEGORY::Material;
		break;
	case ITEM_TYPE::Apple:
		m_eItemCategory = ITEM_CATEGORY::UnCookedFood;
		break;
	default:
		MessageBox(nullptr, "未対応のアイテムタイプです。", "Error", MB_OK | MB_ICONERROR);
		m_eItemCategory = ITEM_CATEGORY::Material; // デフォルトは資源
		break;
	}
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
	case CItem::ITEM_TYPE::Apple:
		return 20.0f; // リンゴの空腹回復値
	default:
		return 0.0f;  // その他のアイテムは空腹回復値なし
	}
}