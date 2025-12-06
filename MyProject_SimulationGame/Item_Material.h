/**************************************************//*
	@file	| Item_Material.h
	@brief	| 素材管理のクラスヘッダ
	@note	| 作成するときの素材に関する情報を管理するクラス
*//**************************************************/
#pragma once
#include "Item.h"
#include <vector>
#include "BuildObject.h"
#include "BuildManager.h"

// @brief 道具素材データ名前空間
namespace ToolMaterials
{
	// @brief 斧の素材リスト
	const std::vector<CItem::Material> Axe = {
		{ CItem::ITEM_TYPE::Wood,  5 },
		{ CItem::ITEM_TYPE::Iron,  2 }
	};

	// @brief つるはしの素材リスト
	const std::vector<CItem::Material> Pickaxe = {
		{ CItem::ITEM_TYPE::Wood,  5 },
		{ CItem::ITEM_TYPE::Iron,  3 }
	};

	// @brief 指定された道具タイプの素材リストを取得
	std::vector<CItem::Material> GetToolMaterials(CItem::ITEM_TYPE eType);
}

// @brief 建築素材データ名前空間
namespace BuildMaterials
{
	// @brief 休憩所に必要な素材
	const std::vector<CItem::Material> RefreshFacility[CBuildObject::MAX_BUILD_LEVEL] =
	{
		// Level 1(初期建築)
		{{ CItem::ITEM_TYPE::Wood,  30 },{ CItem::ITEM_TYPE::Stone,   30 }},
		// Level 2
		{{ CItem::ITEM_TYPE::Wood,  10 },{ CItem::ITEM_TYPE::Stone,   10 }},
		// Level 3
		{{ CItem::ITEM_TYPE::Wood,  15 },{ CItem::ITEM_TYPE::Stone,   15 }},
		// Level 4
		{{ CItem::ITEM_TYPE::Wood,  20 },{ CItem::ITEM_TYPE::Stone,   20 }},
		// Level 5
		{{ CItem::ITEM_TYPE::Wood,  25 },{ CItem::ITEM_TYPE::Stone,   25 }},
	};

	// @brief 人間の家に必要な素材
	const std::vector<CItem::Material> HumanHouse[CBuildObject::MAX_BUILD_LEVEL] =
	{
		// Level 1(初期建築)
		{{ CItem::ITEM_TYPE::Wood,  50 },{ CItem::ITEM_TYPE::Stone,   20 }},
		// Level 2
		{{ CItem::ITEM_TYPE::Wood,  20 },{ CItem::ITEM_TYPE::Stone,  10 }},
		// Level 3
		{{ CItem::ITEM_TYPE::Wood,  30 },{ CItem::ITEM_TYPE::Stone,  10 }},
		// Level 4
		{{ CItem::ITEM_TYPE::Wood,  30 },{ CItem::ITEM_TYPE::Stone,  20 }},
		// Level 5
		{{ CItem::ITEM_TYPE::Wood,  30 },{ CItem::ITEM_TYPE::Stone,  30 }},
	};

	// @brief 鍛冶屋に必要な素材
	const std::vector<CItem::Material> BlackSmith[CBuildObject::MAX_BUILD_LEVEL] =
	{
		// Level 1(初期建築)
		{{ CItem::ITEM_TYPE::Wood,  40 },{ CItem::ITEM_TYPE::Stone,   40 },{ CItem::ITEM_TYPE::Iron, 10 }},
		// Level 2
		{{ CItem::ITEM_TYPE::Wood,  20 },{ CItem::ITEM_TYPE::Stone,   20 },{ CItem::ITEM_TYPE::Iron, 10 }},
		// Level 3
		{{ CItem::ITEM_TYPE::Wood,  30 },{ CItem::ITEM_TYPE::Stone,   30 },{ CItem::ITEM_TYPE::Iron, 15 }},
		// Level 4
		{{ CItem::ITEM_TYPE::Wood,  40 },{ CItem::ITEM_TYPE::Stone,   40 },{ CItem::ITEM_TYPE::Iron, 20 }},
		// Level 5
		{{ CItem::ITEM_TYPE::Wood,  50 },{ CItem::ITEM_TYPE::Stone,   50 },{ CItem::ITEM_TYPE::Iron, 25 }},
	};

	// @brief 指定された建築タイプの建築素材リストを取得
	std::vector<CItem::Material> GetBuildMaterials(CBuildManager::BuildType eType, int nLevel);
}

// @brief 調理素材データ名前空間
namespace CookMaterials
{
	// @brief 砂糖の素材リスト
	const std::vector<CItem::Material> Sugar = {
		{ CItem::ITEM_TYPE::Apple,  1 }
	};

	// @brief アップルパイの素材リスト
	const std::vector<CItem::Material> ApplePie = {
		{ CItem::ITEM_TYPE::Apple,  2 },
		{ CItem::ITEM_TYPE::Sugar,  1 }
	};


	// @brief 指定された食事タイプの素材リストを取得
	std::vector<CItem::Material> GetCookMaterials(CItem::ITEM_TYPE eType);
}