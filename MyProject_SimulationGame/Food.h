/**************************************************//*
	@file	| Food.h
	@brief	| 空腹値を管理するクラスのhファイル
	@note	| アイテムタイプから空腹回復値を取得する関数を定義
*//**************************************************/
#pragma once
#include "Item.h"

// @brief アイテムタイプから空腹回復値を取得する関数
// @param eType：アイテムタイプ
// @return 空腹回復値
static float GetHungerRecoveryValue(CItem::ITEM_TYPE eType)
{
	switch (eType)
	{
	case CItem::ITEM_TYPE::Apple:
		return 20.0f; // リンゴの空腹回復値
	default:
		return 0.0f;  // その他のアイテムは空腹回復値なし
	}
}