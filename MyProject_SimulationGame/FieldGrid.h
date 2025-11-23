/**************************************************//*
	@file	| FieldGrid.h
	@brief	| フィールドグリッドクラスのhファイル
	@note	| フィールドをグリッド状に管理
*//**************************************************/
#pragma once
#include "FieldCell.h"



// @brief フィールドグリッドクラス
class CFieldGrid
{
public:
	static const int GridSizeX = 50;	// グリッドのXサイズ
	static const int GridSizeY = 50;	// グリッドのYサイズ

public:
	// @brief コンストラクタ
	// @param In_vPos グリッドの中心座標
	// @param In_X グリッドのXサイズ
	// @param In_Y グリッドのYサイズ
	CFieldGrid(const DirectX::XMFLOAT3 In_vPos);
	
	// @brief デストラクタ
	~CFieldGrid();

	// @brief グリッドの初期化
	void Init();

	// @brief グリッドの解放
	void Uninit();

	// @brief フィールドセルの取得
	std::vector<std::vector<CFieldCell*>> GetFieldCells() { return m_pFieldCells; }

	// @brief フィールドセルの取得
	// @param In_Type セルタイプ
	// @param In_Use：true:使用中のセルを取得 false:未使用のセルを取得
	std::vector<CFieldCell*> GetFieldCells(CFieldCell::CellType In_Type, bool In_Use);

private:
	// @brief フィールドセルの2次元配列
	std::vector<std::vector<CFieldCell*>> m_pFieldCells;
};

