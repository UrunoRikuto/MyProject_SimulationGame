/**************************************************//*
	@file	| FieldGrid.cpp
	@brief	| フィールドグリッドクラスのcppファイル
	@note	| フィールドをグリッド状に管理
*//**************************************************/
#include "FieldGrid.h"

/****************************************//*
	@brief	| コンストラクタ
	@param		| In_XSize	: グリッドのXサイズ
	@param		| In_YSize	: グリッドのYサイズ
*//****************************************/
CFieldGrid::CFieldGrid(const DirectX::XMFLOAT3 In_vPos)
{
	DirectX::XMFLOAT3 StartPos;
	StartPos.x = In_vPos.x - (GridSizeX / 2) * CFieldCell::CELL_SIZE.x + CFieldCell::CELL_SIZE.x / 2;
	StartPos.y = In_vPos.y;
	StartPos.z = In_vPos.z - (GridSizeY / 2) * CFieldCell::CELL_SIZE.z + CFieldCell::CELL_SIZE.z / 2;

	// ストレージを準備：セル実体を連続配列で確保し、多数の小さな new/delete を回避する
	m_cellsStorage.reserve(static_cast<size_t>(GridSizeX) * static_cast<size_t>(GridSizeY));
	for (int x = 0; x < GridSizeX; ++x)
	{
		for (int y = 0; y < GridSizeY; ++y)
		{
			m_cellsStorage.emplace_back(StartPos, DirectX::XMINT2{ x,y });
			StartPos.z += CFieldCell::CELL_SIZE.z;
		}
		StartPos.x += CFieldCell::CELL_SIZE.x;
		StartPos.z = In_vPos.z - (GridSizeY / 2) * CFieldCell::CELL_SIZE.z + CFieldCell::CELL_SIZE.z / 2;
	}

	// m_cellsStorage の要素を指す2 次元ポインタ配列を構築
	m_pFieldCells.resize(GridSizeX);
	for (int x = 0; x < GridSizeX; ++x)
	{
		m_pFieldCells[x].resize(GridSizeY);
		for (int y = 0; y < GridSizeY; ++y)
		{
			// m_cellsStorage の対応要素へのポインタ
			m_pFieldCells[x][y] = &m_cellsStorage[static_cast<size_t>(x) * GridSizeY + static_cast<size_t>(y)];
		}
	}
}

/****************************************//*
	@brief	| デストラクタ
*//****************************************/
CFieldGrid::~CFieldGrid()
{
	// m_pFieldCells 内のポインタは m_cellsStorage の要素を指しており、m_cellsStorage は自動で破棄される
	for (int x =0; x < m_pFieldCells.size(); ++x)
	{
		m_pFieldCells[x].clear();
	}
	m_pFieldCells.clear();

	// ストレージをクリア
	m_cellsStorage.clear();
}

/****************************************//*
	@brief	| フィールドセルの取得
	@param		| In_Type	: セルタイプ
	@param		| In_Use	: true:使用中のセルを取得 false:未使用のセルを取得
*//****************************************/
std::vector<CFieldCell*> CFieldGrid::GetFieldCells(CFieldCell::CellType In_Type, bool In_Use)
{
	// 条件に合うセルを収集
	std::vector<CFieldCell*> FieldCells;
	FieldCells.reserve(256); // 頻繁なリサイズを避けるための小さな予約

	for (int x =0; x < GridSizeX; ++x)
	{
		for (int y =0; y < GridSizeY; ++y)
		{
			CFieldCell* cell = m_pFieldCells[x][y];
			if (cell->GetCellType() == In_Type && cell->IsUse() == In_Use)
			{
				FieldCells.push_back(cell);
			}
		}
	}

	return FieldCells;
}
