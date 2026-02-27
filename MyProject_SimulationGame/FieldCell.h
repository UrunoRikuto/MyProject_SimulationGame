/**************************************************//*
	@file	| FieldCell.h
	@brief	| フィールドセルクラスのヘッダファイル
	@note	| フィールドセルの情報（位置、種類、使用有無、所属オブジェクト等）を定義
*//**************************************************/
#pragma once
#include "GameObject.h"
#include "ModelRenderer.h"

#undef GetObject

// @brief フィールドセルクラス
class CFieldCell
{
public:
	// @brief セルタイプ（見た目）
	enum class CellType
	{
		EMPTY,		// 空
		TREE,		// 木
		ROCK,		// 岩
		GRASS,		// 草
		Build,		// 建築可能地
		MAX		// セルタイプの最大数
	};

	// @brief 縄張りタイプ（所有者等）
	enum class TerritoryType
	{
		Human,		// 人間
		Wolf,		// 狼
		Deer,		// 鹿
		MAX,		// タイプの最大数
		NONE		// 無し
	};

	// @brief セルのサイズ（幅・高さ・奥行き）
	constexpr static DirectX::XMFLOAT3 CELL_SIZE = {2.0f,2.0f,2.0f };

public:
	// @brief コンストラクタ
	CFieldCell(const DirectX::XMFLOAT3 In_vPos, const DirectX::XMINT2 In_vIndex);

	// @brief デストラクタ
	~CFieldCell();

	// @brief デバッグ描画
	// @param nMode: 描画モード(0: セルタイプ表示,1: 縄張りタイプ表示)
	void DebugDraw(int nMode);

	// @brief セルタイプの取得
	const CellType GetCellType() { return m_eCellType; }

	// @brief セルタイプの設定
	void SetCellType(const CellType In_eType);

	// @brief 縄張りタイプの取得
	const TerritoryType GetTerritoryType() { return m_eTerritoryType; }

	// @brief 縄張りタイプの設定
	void SetTerritoryType(const TerritoryType In_eType) { m_eTerritoryType = In_eType; }

	// @brief 使用中フラグの取得（オブジェクトが配置されているか）
	const bool IsUse() { return m_bUse; }

	// @brief 使用中フラグの設定
	void SetUse(const bool In_bUse) { m_bUse = In_bUse; }

	// @brief セルの中心位置の取得
	const DirectX::XMFLOAT3 GetPos() { return m_vPos; }

	// @brief セルのインデックス取得
	DirectX::XMINT2 GetIndex() { return m_n2Index; }

	// @brief 配置されているオブジェクトの取得
	CGameObject* GetObject() { return m_pObject; }

	// @brief 配置されているオブジェクトの設定
	void SetObject(CGameObject* In_pObject) { m_pObject = In_pObject; }

private:

	// @brief セル中心位置
	DirectX::XMFLOAT3 m_vPos;

	// @brief グリッド座標（インデックス）
	DirectX::XMINT2 m_n2Index;

	// @brief セルタイプ
	CellType m_eCellType;

	// @brief 縄張りタイプ
	TerritoryType m_eTerritoryType;

	// @brief 使用中フラグ
	bool m_bUse;

	// @brief 配置されているゲームオブジェクトへのポインタ
	CGameObject* m_pObject;
};

