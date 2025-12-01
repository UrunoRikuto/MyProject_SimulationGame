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
		// 繊維
		Faiber,
		// リンゴ
		Apple,
		// 石材
		Stone,
		// 鉄材
		Iron,

		MAX
	};

	// @brief アイテムタイプを文字列に変換する関数
	static std::string ITEM_TYPE_TO_STRING(ITEM_TYPE eType)
	{
		switch (eType)
		{
		case ITEM_TYPE::Wood:	return "Wood";
		case ITEM_TYPE::Faiber:	return "Faiber";
		case ITEM_TYPE::Apple:	return "Apple";
		case ITEM_TYPE::Stone:	return "Stone";
		case ITEM_TYPE::Iron:	return "Iron";
		default:				return "Unknown";
		}
	}

public:
	// @brief コンストラクタ
	// @param In_eType：アイテムタイプ
	CItem(ITEM_TYPE In_eType);

	// @brief デストラクタ
	~CItem();

	// @brief アイテムタイプ取得
	const ITEM_TYPE GetItemType() const { return m_eItemType; }

private:

	// @brief アイテムタイプ
	ITEM_TYPE m_eItemType;

};

