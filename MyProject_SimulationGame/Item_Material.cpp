/**************************************************//*
	@file	| Item_Material.cpp
	@brief	| 素材管理のクラス実装
	@note	| 作成するときの素材に関する情報を管理するクラスの実装
*//**************************************************/
#include "Item_Material.h"


/****************************************//*
	@brief　	| 指定した道具の素材リストを取得する関数
	@param　	| eType：道具のアイテムタイプ
	@return　	| 指定した道具の素材リスト
*//****************************************/
std::vector<CItem::Material> ToolMaterials::GetToolMaterials(CItem::ITEM_TYPE eType)
{
	switch (eType)
	{
	case CItem::ITEM_TYPE::Pickaxe:
		return Pickaxe;
	case CItem::ITEM_TYPE::Axe:
		return Axe;
	default:
		MessageBox(nullptr, "タイプのケースが用意されていません", "Error", MB_OK);
	}

	return std::vector<CItem::Material>();
}

/*****************************************//*
	@brief	| 建築素材を取得
	@param	| eType：建築物タイプ
	@return	| 建築素材の配列
*//*****************************************/
std::vector<CItem::Material> BuildMaterials::GetBuildMaterials(CBuildManager::BuildType eType, int nLevel)
{
	switch (eType)
	{
	case CBuildManager::BuildType::RefreshFacility:
		return RefreshFacility[nLevel];
	case CBuildManager::BuildType::HumanHouse:
		return HumanHouse[nLevel];
	case CBuildManager::BuildType::BlackSmith:
		return BlackSmith[nLevel];
	case CBuildManager::BuildType::FoodFactory:
		return FoodFactory[nLevel];
	case CBuildManager::BuildType::FarmFacility:
		return FarmFacility[nLevel];
	default:
		MessageBox(nullptr, "タイプのケースが用意されていません", "Error", MB_OK);
	}

	return std::vector<CItem::Material>();
}

/*****************************************//*
	@brief	| 調理素材を取得
	@param	| eType：調理済み食料のアイテムタイプ
	@return	| 調理素材の配列
*//*****************************************/
std::vector<CItem::Material> CookMaterials::GetCookMaterials(CItem::ITEM_TYPE eType)
{
	switch (eType)
	{
	case CItem::ITEM_TYPE::Sugar:
		return Sugar;
	case CItem::ITEM_TYPE::ApplePie:
		return ApplePie;
	default:
		MessageBox(nullptr, "タイプのケースが用意されていません", "Error", MB_OK);
	}

    return std::vector<CItem::Material>();
}

/*****************************************//*
	@brief	| 完成した料理アイテムを取得
	@param	| eType：調理済み食料のアイテムタイプ
	@return	| 完成した料理アイテムの配列
*//*****************************************/
std::vector<CItem*> CookMaterials::GetFinishedMealItems(CItem::ITEM_TYPE eType)
{
	// 完成品アイテムの配列
	std::vector<CItem*> finishedItems;
	// 完成品アイテムの数
	int ItemCount = 0;

	// アイテムタイプに応じた完成品アイテムの数を決定
	switch (eType)
	{
	case CItem::ITEM_TYPE::Sugar:
		// 砂糖は3個生成
		ItemCount = 3;
		break;
	case CItem::ITEM_TYPE::ApplePie:
		// アップルパイは1個生成
		ItemCount = 1;
		break;
	default:
		MessageBox(nullptr, "タイプのケースが用意されていません", "Error", MB_OK);
		break;
	}

	// 指定された数だけ完成品アイテムを生成して配列に追加
	for (int i = 0; i < ItemCount; ++i)
	{
		// 新しいアイテムを生成して配列に追加
		finishedItems.push_back(new(std::nothrow) CItem(eType));
	}

	return finishedItems;
}
