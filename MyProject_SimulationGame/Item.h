/**************************************************//*
	@file	| Item.h
	@brief	| アイテムクラスヘッダ
	@note	| アイテムに関する情報を管理するクラス
*//**************************************************/
#pragma once
#include <string>

// @brief アイテムクラス
class CItem
{
public:
	// @brief アイテムタイプ列挙型
	enum class ITEM_TYPE
	{
		// 木材
		Wood,
		// 石材
		Stone,
		// 鉄材
		Iron,
		// 繊維
		Fiber,

		// 麦の種
		WheatSeed, 

		// つるはし
		Pickaxe,
		// 斧
		Axe,
		// 鎌
		Sickle,

		// リンゴ
		Apple,
		// 砂糖
		Sugar,
		// アップルパイ
		ApplePie,
		// 麦
		Wheat,
		// パン
		Bread,

		MAX
	};

	// @brief アイテムタイプを文字列に変換する関数
	static std::string ITEM_TYPE_TO_STRING(ITEM_TYPE eType)
	{
		switch (eType)
		{
		case ITEM_TYPE::Wood:		return u8"木";
		case ITEM_TYPE::Stone:		return u8"石";
		case ITEM_TYPE::Iron:		return u8"鉄";
		case ITEM_TYPE::Fiber:		return u8"繊維";
		case ITEM_TYPE::WheatSeed:	return u8"麦の種";
		case ITEM_TYPE::Pickaxe:	return u8"つるはし";
		case ITEM_TYPE::Axe:		return u8"斧";
		case ITEM_TYPE::Sickle:		return u8"鎌";
		case ITEM_TYPE::Apple:		return u8"リンゴ";
		case ITEM_TYPE::Sugar:		return u8"砂糖";
		case ITEM_TYPE::ApplePie:	return u8"アップルパイ";
		case ITEM_TYPE::Wheat:		return u8"麦";
		case ITEM_TYPE::Bread:		return u8"パン";
		default:					return u8"Unknown";
		}
	}

	// @brief アイテムカテゴリー列挙型
	enum class ITEM_CATEGORY
	{
		// 資源
		Material,
		// 未調理食料
		UnCookedFood,
		// 調理済み食料
		CookedFood,
		// 道具
		Tool,
		// 種
		Seed,

		MAX
	};

	// @brief アイテムカテゴリーを文字列に変換する関数
	static std::string ITEM_CATEGORY_TO_STRING(ITEM_CATEGORY eCategory)
	{
		switch (eCategory)
		{
		case ITEM_CATEGORY::Material:		return u8"素材";
		case ITEM_CATEGORY::UnCookedFood:	return u8"未調理食材";
		case ITEM_CATEGORY::CookedFood:		return u8"調理済み食品";
		case ITEM_CATEGORY::Tool:			return u8"装備";
		case ITEM_CATEGORY::Seed:			return u8"種";
		default:							return u8"Unknown";
		}
	}

	// @brief アイテムタイプからアイテムカテゴリーを取得する関数
	static ITEM_CATEGORY GetItemCategoryFromType(ITEM_TYPE eType)
	{
		switch (eType)
		{
		case ITEM_TYPE::Wood:
		case ITEM_TYPE::Stone:
		case ITEM_TYPE::Iron:
		case ITEM_TYPE::Sugar:
		case ITEM_TYPE::Fiber:
		case ITEM_TYPE::Wheat:
			return ITEM_CATEGORY::Material;
		case ITEM_TYPE::Apple:
			return ITEM_CATEGORY::UnCookedFood;
		case ITEM_TYPE::ApplePie:
		case ITEM_TYPE::Bread:
			return ITEM_CATEGORY::CookedFood;
		case ITEM_TYPE::Pickaxe:
		case ITEM_TYPE::Axe:
		case ITEM_TYPE::Sickle:
			return ITEM_CATEGORY::Tool;
		case ITEM_TYPE::WheatSeed:
			return ITEM_CATEGORY::Seed;
		default:
			return ITEM_CATEGORY::Material;
		}
	}


	// @brief 素材構造体
	struct Material
	{
		CItem::ITEM_TYPE eItemType;		// アイテムタイプ
		int nRequiredAmount;			// 必要数
	};

public:
	// @brief コンストラクタ
	// @param In_eType：アイテムタイプ
	CItem(ITEM_TYPE In_eType);

	// @brief デストラクタ
	~CItem();

	// @brief アイテムタイプ取得
	const ITEM_TYPE GetItemType() const { return m_eItemType; }

	// @brief アイテムタイプから空腹回復値を取得する関数
	// @param eType：アイテムタイプ
	// @return 空腹回復値
	static float GetHungerRecoveryValue(CItem::ITEM_TYPE eType);

	// @brief アイテムカテゴリーを取得する関数
	const ITEM_CATEGORY GetItemCategory() const { return m_eItemCategory; }
			

private:

	// @brief アイテムカテゴリー
	ITEM_CATEGORY m_eItemCategory;

	// @brief アイテムタイプ
	ITEM_TYPE m_eItemType;

};