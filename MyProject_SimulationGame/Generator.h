/**************************************************//*
	@file	| IGenerator.h
	@brief	| ジェネレーターインターフェースのhファイル
	@note	| ジェネレーターの基底クラス
			| IObserverを継承
			| ストラテジーパターンで作成
*//**************************************************/
#pragma once
#include "Observer.h"
#include "FieldManager.h"

// @brief ジェネレーターインターフェース
class IGenerator : public IObserver
{
public:
	// @brief 仮想デストラクタ
	virtual ~IGenerator() = default;

	// @brief 通知処理
	void Notify() override { Generate(); }

	// @brief 更新処理(生成管理システムから毎フレーム呼ばれる)
	// @note 時間経過で補充するなど、通知を待たずに行う処理を実装する
	virtual void Update() {}

	// @brief 指定したセルに1つだけ生成する処理
	// @param n2Cell：生成先のセルのインデックス
	virtual void GenerateAt(const DirectX::XMINT2& n2Cell) { (void)n2Cell; }

protected:

	// @brief 生成処理
	virtual void Generate() = 0;

	// @brief 指定したタイプで、まだ何も置かれていないセルを取得する処理
	// @param n2Cell：セルのインデックス
	// @param eType：生成先として期待するセルタイプ
	// @return セルのポインタ、範囲外・タイプが違う・使用中の場合はnullptr
	CFieldCell* GetGeneratableCell(const DirectX::XMINT2& n2Cell, CFieldCell::CellType eType) const
	{
		auto& cells = CFieldManager::GetInstance()->GetFieldGrid()->GetFieldCells();
		if (n2Cell.x < 0 || n2Cell.x >= static_cast<int>(cells.size())) return nullptr;
		if (n2Cell.y < 0 || n2Cell.y >= static_cast<int>(cells[n2Cell.x].size())) return nullptr;

		// 開拓などで別の用途の土地になった場合や、既に何かが置かれている場合は生成しない
		CFieldCell* pCell = cells[n2Cell.x][n2Cell.y];
		if (pCell->GetCellType() != eType || pCell->IsUse()) return nullptr;

		return pCell;
	}

};

// 派生クラスのヘッダーファイルのインクルード
#include "HumanGenerator.h"
#include "WoodGenerator.h"
#include "StoneGenerator.h"
