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
	case CItem::ITEM_TYPE::Apple:
		return 20.0f; // リンゴの空腹回復値
	default:
		return 0.0f;  // その他のアイテムは空腹回復値なし
	}
}

/****************************************//*
	@brief　	| 指定した道具の素材リストを取得する関数
	@param　	| eType：道具のアイテムタイプ
	@return　	| 指定した道具の素材リスト
*//****************************************/
std::vector<CItem::ToolMaterial> ToolMaterials::GetToolMaterials(CItem::ITEM_TYPE eType)
{
	switch (eType)
	{
	case CItem::ITEM_TYPE::Pickaxe:
		return Pickaxe;
	case CItem::ITEM_TYPE::Axe:
		return Axe;
	default:
		MessageBox(nullptr, "タイプのケースが用意されていません", "Error", MB_OK);
		return {};
	}
}
