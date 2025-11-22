/**************************************************//*
	@file	| FieldCell.h
	@brief	| フィールドセルクラスのhファイル
	@note	| フィールドを構成するセルの情報を管理
*//**************************************************/
#pragma once
#include "GameObject.h"
#include "ModelRenderer.h"

// @brief フィールドセルクラス
class CFieldCell
{
public:
	enum class CellType
	{
		EMPTY,		// 空地
		TREE,		// 木
		ROCK,		// 岩
		MAX			// セルタイプの最大数
	};

	// @brief セルのサイズ
	constexpr static DirectX::XMFLOAT3 CELL_SIZE = { 2.0f, 2.0f, 2.0f };

public:
	// @brief コンストラクタ
	CFieldCell(const DirectX::XMFLOAT3 In_vPos, const DirectX::XMINT2 In_vIndex);

	// @brief デストラクタ
	~CFieldCell();

	// @brief デバック描画
	void DebugDraw();

	// @brief セルタイプの取得
	const CellType GetCellType() { return m_eCellType; }

	// @brief セルタイプの設定
	void SetCellType(const CellType In_eType);

	// @brief セルを使用しているかどうかの取得
	const bool IsUse() { return m_bUse; }

	// @brief セルを使用しているかどうかの設定
	void SetUse(const bool In_bUse) { m_bUse = In_bUse; }

	// @brief セルの中心座標の取得
	// @return セルの中心座標
	const DirectX::XMFLOAT3 GetPos() { return m_vPos; }

	// @brief セルのインデックスの取得
	// @return セルのインデックス
	DirectX::XMINT2 GetIndex() { return m_vIndex; }

private:

	// @brief セルの中心座標
	DirectX::XMFLOAT3 m_vPos;

	// @brief 自身のインデックス
	DirectX::XMINT2 m_vIndex;

	// @brief セルタイプ
	CellType m_eCellType;

	// @brief セルを使用しているかどうか
	bool m_bUse;
};

