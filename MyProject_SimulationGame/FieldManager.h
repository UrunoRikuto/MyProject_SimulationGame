/**************************************************//*
	@file	| FieldManager.h
	@brief	| フィールド管理クラスのhファイル
	@note	| フィールド全体の管理を行う
			| シングルトンパターンで作成
*//**************************************************/
#pragma once
#include "FieldGrid.h"

// @brief フィールド管理クラス
class CFieldManager
{
private:
	// @brief コンストラクタ
	CFieldManager();
	// デフォルトコンストラクタ禁止
	CFieldManager(const CFieldManager&) = delete;
	// 代入演算子禁止
	CFieldManager& operator=(const CFieldManager&) = delete;

public:
	
	// @brief デストラクタ
	~CFieldManager();

	// @brief インスタンスの取得
	static CFieldManager* GetInstance();

	// @brief インスタンスの解放
	static void ReleaseInstance();

	// @brief フィールドセルのタイプ選出
	void AssignFieldCellType();

	// @brief フィールドグリッドの取得
	CFieldGrid* GetFieldGrid() { return m_pFieldGrid; }

private:
	// @brief 静的メンバ変数:インスタンスのポインタ
	static CFieldManager* m_pInstance;
	
	// @brief フィールドグリッドのポインタ
	CFieldGrid* m_pFieldGrid;
};

