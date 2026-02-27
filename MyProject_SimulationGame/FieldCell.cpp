/**************************************************//*
	@file	| FieldCell.cpp
	@brief	| フィールドセルクラスのcppファイル
	@note	| フィールドセルの挙動（コンストラクタ、デストラクタ、デバッグ描画等）を実装
*//**************************************************/
#include "FieldCell.h"
#include "Geometory.h"
#include "Camera.h"

/****************************************//*
	@brief	| コンストラクタ
	@param	| In_vPos セルの中心位置
	@param	| In_vIndex セルのグリッドインデックス
*//****************************************/
CFieldCell::CFieldCell(const DirectX::XMFLOAT3 In_vPos, const DirectX::XMINT2 In_vIndex)
	: m_eCellType(CellType::EMPTY)
	, m_eTerritoryType(TerritoryType::NONE)
	, m_bUse(false)
	, m_vPos(In_vPos)
	, m_n2Index(In_vIndex)
	, m_pObject(nullptr)
{
}

/****************************************//*
	@brief	| デストラクタ
*//****************************************/
CFieldCell::~CFieldCell()
{
}

/****************************************//*
	@brief	| デバッグ描画
	@param	| nMode: 描画モード (0: セルタイプ表示,1: 縄張りタイプ表示)
*//****************************************/
void CFieldCell::DebugDraw(int nMode)
{
	DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(0.0f,0.0f,0.0f,0.0f);

	switch (nMode)
	{
	case 0:
		switch (m_eCellType)
		{
		case CFieldCell::CellType::EMPTY:
			// 空
			color = DirectX::XMFLOAT4(1.0f,1.0f,1.0f,1.0f);
			break;
		case CFieldCell::CellType::TREE:
			// 木
			color = DirectX::XMFLOAT4(0.0f,1.0f,0.0f,1.0f);
			break;
		case CFieldCell::CellType::ROCK:
			// 岩
			color = DirectX::XMFLOAT4(0.0f,0.0f,1.0f,1.0f);
			break;
		case CFieldCell::CellType::GRASS:
			// 草
			color = DirectX::XMFLOAT4(1.0f,0.0f,0.0f,1.0f);
			break;
		case CFieldCell::CellType::Build:
			// 建築可能地
			color = DirectX::XMFLOAT4(1.0f,1.0f,0.0f,1.0f);
			break;
		}
		break;
	case 1:
		switch (m_eTerritoryType)
		{
		case CFieldCell::TerritoryType::NONE:
			// 無し
			color = DirectX::XMFLOAT4(1.0f,1.0f,1.0f,1.0f);
			break;
		case CFieldCell::TerritoryType::Human:
			// 人間
			color = DirectX::XMFLOAT4(1.0f,1.0f,0.0f,1.0f);
			break;
		case CFieldCell::TerritoryType::Wolf:
			// 狼
			color = DirectX::XMFLOAT4(1.0f,0.0f,0.0f,1.0f);
			break;
		case CFieldCell::TerritoryType::Deer:
			// 鹿
			color = DirectX::XMFLOAT4(0.6f,0.3f,0.0f,1.0f);
			break;
		}
		break;
	default:
		break;
	}

	Geometory::DrawPlane(m_vPos, CELL_SIZE, DirectX::XMFLOAT3(0.0f,1.0f,0.0f), color);
}

/****************************************//*
	@brief	| セルタイプの設定
	@param	| In_eType セルタイプ
*//****************************************/
void CFieldCell::SetCellType(const CellType In_eType)
{
	m_eCellType = In_eType;
}