/**************************************************//*
	@file	| FieldManager.h
	@brief	| フィールド管理クラスのhファイル
	@note	| フィールド全体の管理を行う
			| シングルトンパターンで作成
*//**************************************************/
#pragma once
#include "Singleton.h"
#include "FieldGrid.h"

// @brief フィールド管理クラス
class CFieldManager : public ISingleton<CFieldManager>
{
private:
	// @brief コンストラクタ
	CFieldManager();

	friend class ISingleton<CFieldManager>;

public:
	
	// @brief デストラクタ
	~CFieldManager();

	// @brief フィールドセルのタイプ選出
	void AssignFieldCellType();

	// @brief フィールドグリッドの取得
	CFieldGrid* GetFieldGrid() { return m_pFieldGrid; }

	// @brief フィールドグリッドの表示
	void DebugDraw();

private:

	// @brief 初期村の作成
	void CreateInitialVillage();

	// @brief 縄張りの作成
	void CreateTerritory();

private:
	
	// @brief フィールドグリッドのポインタ
	CFieldGrid* m_pFieldGrid;
};

