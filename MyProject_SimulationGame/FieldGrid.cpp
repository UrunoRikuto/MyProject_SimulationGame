/**************************************************//*
	@file	| FieldGrid.cpp
	@brief	| フィールドグリッドクラスのcppファイル
	@note	| フィールドをグリッド状に管理
*//**************************************************/
#include "FieldGrid.h"

// 定数定義
constexpr int GridSizeX = 50;	// グリッドのXサイズ
constexpr int GridSizeY = 50;	// グリッドのYサイズ


/****************************************//*
	@brief　	| コンストラクタ
	@param		| In_XSize	: グリッドのXサイズ
	@param		| In_YSize	: グリッドのYサイズ
*//****************************************/
CFieldGrid::CFieldGrid(const DirectX::XMFLOAT3 In_vPos)
{
	DirectX::XMFLOAT3 StartPos;
	StartPos.x = In_vPos.x - (GridSizeX / 2) * CFieldCell::CELL_SIZE.x + CFieldCell::CELL_SIZE.x / 2;
	StartPos.y = In_vPos.y;
	StartPos.z = In_vPos.z - (GridSizeY / 2) * CFieldCell::CELL_SIZE.z + CFieldCell::CELL_SIZE.z / 2;

	// フィールドセル配列のサイズ設定
	m_pFieldCells.resize(GridSizeX);
	for (int x = 0; x < GridSizeX; ++x)
	{
		m_pFieldCells[x].resize(GridSizeY);
	}

	// フィールドセルの2次元配列の動的確保
	for(int x = 0; x < GridSizeX; ++x)
	{
		for(int y = 0; y < GridSizeY; ++y)
		{
			CFieldCell* NewCell = new CFieldCell(StartPos, { x,y });
			m_pFieldCells[x][y] = NewCell;

			// Z方向にセルサイズ分移動
			StartPos.z += CFieldCell::CELL_SIZE.z;
		}

		// X方向にセルサイズ分移動し、Z位置を初期位置に戻す
		StartPos.x += CFieldCell::CELL_SIZE.x;
		StartPos.z = In_vPos.z - (GridSizeY / 2) * CFieldCell::CELL_SIZE.z + CFieldCell::CELL_SIZE.z / 2;
	}
}

/****************************************//*
	@brief　	| デストラクタ
*//****************************************/
CFieldGrid::~CFieldGrid()
{
	for(int x = m_pFieldCells.size() - 1; x >= 0; --x)
	{
		for(int y = m_pFieldCells[x].size() - 1; y >= 0; --y)
		{
			delete m_pFieldCells[x][y];
			m_pFieldCells[x][y] = nullptr;
		}
		m_pFieldCells[x].clear();
	}
	m_pFieldCells.clear();
}

/****************************************//*
	@brief　	| フィールドセルの取得
	@param		| In_Type	: セルタイプ
	@param		| In_Use	: true:使用中のセルを取得 false:未使用のセルを取得
*//****************************************/
std::vector<CFieldCell*> CFieldGrid::GetFieldCells(CFieldCell::CellType In_Type, bool In_Use)
{
	// セルタイプに一致するフィールドセルの配列
	std::vector<CFieldCell*> FieldCells;

	for(int x = 0; x < m_pFieldCells.size(); ++x)
	{
		for(int y = 0; y < m_pFieldCells[x].size(); ++y)
		{
			// セルタイプが一致した場合
			if(m_pFieldCells[x][y]->GetCellType() == In_Type)
			{
				// 使用状態が一致した場合
				if (m_pFieldCells[x][y]->IsUse() == In_Use)
					FieldCells.push_back(m_pFieldCells[x][y]);
			}
		}
	}

	// フィールドセル配列を返す
	return FieldCells;
}
