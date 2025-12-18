/**************************************************//*
	@file	| FieldCell.cpp
	@brief	| フィールドセルクラスのcppファイル
	@note	| フィールドを構成するセルの情報を管理
*//**************************************************/
#include "FieldCell.h"
#include "Geometory.h"
#include "Camera.h"

/****************************************//*
	@brief　	| コンストラクタ
	@param		| In_eType	セルタイプ
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
	@brief　	| デストラクタ
*//****************************************/
CFieldCell::~CFieldCell()
{
}

/****************************************//*
	@brief　	| デバック描画
	@param		| nMode：描画モード(0:セルタイプ, 1:縄張りタイプ)
*//****************************************/
void CFieldCell::DebugDraw(int nMode)
{
	DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

	switch (nMode)
	{
	case 0:
		switch (m_eCellType)
		{
		case CFieldCell::CellType::EMPTY:
			// 白
			color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			break;
		case CFieldCell::CellType::TREE:
			// 緑
			color = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
			break;
		case CFieldCell::CellType::ROCK:
			// 青
			color = DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
			break;
		case CFieldCell::CellType::GRASS:
			// 赤
			color = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
			break;
		case CFieldCell::CellType::Build:
			// 黄
			color = DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
			break;
		}
		break;
	case 1:
		switch (m_eTerritoryType)
		{
		case CFieldCell::TerritoryType::NONE:
			// 白
			color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			break;
		case CFieldCell::TerritoryType::Human:
			// 黄
			color = DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
			break;
		case CFieldCell::TerritoryType::Wolf:
			// 赤
			color = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
			break;
		case CFieldCell::TerritoryType::Deer:
			// 茶
			color = DirectX::XMFLOAT4(0.6f, 0.3f, 0.0f, 1.0f);
			break;
		}
		break;
	}

	Geometory::DrawPlane(m_vPos, CELL_SIZE, DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), color);
}

/****************************************//*
	@brief　	| セルタイプの設定
	@param		| In_eType	セルタイプ
*//****************************************/
void CFieldCell::SetCellType(const CellType In_eType)
{
	m_eCellType = In_eType;
}