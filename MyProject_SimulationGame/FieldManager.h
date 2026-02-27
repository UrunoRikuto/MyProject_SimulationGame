/**************************************************//*
	@file	| FieldManager.h
	@brief	| フィールド管理クラスのhファイル
	@note	| フィールド全体の管理を行う
			| シングルトンパターンで作成
*//**************************************************/
#pragma once
#include "Singleton.h"
#include "FieldGrid.h"
#include <functional>
#include "BuildObject.h"
#include "Scene.h"

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

	// @brief フィールドタイプの作成
	void CreateFieldType();

	// @brief 初期村の作成
	void CreateInitialVillage();

	// @brief 縄張りの作成
	void CreateTerritory();


	// @brief 建築物の作成と配置
	// @param cells：建築物を配置するフィールドセルのリスト
	// @param factory：建築物を生成するためのファクトリー関数
	// @return 配置した建築物のポインタ
	CBuildObject* CreateAndPlaceBuilding(std::vector<CFieldCell*>& cells, std::function<CBuildObject* (CScene*)> factory);

private:
	
	// @brief フィールドグリッドのポインタ
	CFieldGrid* m_pFieldGrid;
};

