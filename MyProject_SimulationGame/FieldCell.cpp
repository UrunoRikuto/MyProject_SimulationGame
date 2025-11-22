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
	, m_bUse(false)
	, m_vPos(In_vPos)
	, m_vIndex(In_vIndex)
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
*//****************************************/
void CFieldCell::DebugDraw()
{
	DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

	switch (m_eCellType)
	{
	case CFieldCell::CellType::EMPTY:
		// 黒
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