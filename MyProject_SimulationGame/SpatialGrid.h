/**************************************************//*
	@file	| SpatialGrid.h
	@brief	| 空間分割グリッドクラスのhファイル
	@note	| フィールドを格子状に区切り、近くにいるオブジェクトだけを素早く探すためのクラス
			| 群れの近隣検索で、全員との距離を調べずに済むようにする(O(N^2) → O(N))
*//**************************************************/
#pragma once
#include "FieldGrid.h"
#include <vector>
#include <algorithm>
#include <cmath>

// @brief 空間分割グリッドクラス
// @tparam T：管理するオブジェクトの型(GetPos()を持つこと)
template<typename T>
class CSpatialGrid
{
public:
	// @brief グリッドの作り直し
	// @param items：管理するオブジェクトのリスト
	// @param fCellSize：1マスの大きさ(検索する距離以上にすること)
	void Build(const std::vector<T*>& items, float fCellSize)
	{
		m_fCellSize = fCellSize;
		m_fOriginX = -CFieldGrid::GetHalfExtentX();
		m_fOriginZ = -CFieldGrid::GetHalfExtentZ();
		m_nCountX = static_cast<int>(std::ceil(CFieldGrid::GetHalfExtentX() * 2.0f / fCellSize)) + 1;
		m_nCountZ = static_cast<int>(std::ceil(CFieldGrid::GetHalfExtentZ() * 2.0f / fCellSize)) + 1;

		// マスごとのオブジェクト数を数え、先頭位置を求める
		m_CellStart.assign(static_cast<size_t>(m_nCountX) * m_nCountZ + 1, 0);
		m_CellOfItem.resize(items.size());
		for (size_t i = 0; i < items.size(); ++i)
		{
			const DirectX::XMFLOAT3 pos = items[i]->GetPos();
			const int nCell = CellIndex(CellX(pos.x), CellZ(pos.z));
			m_CellOfItem[i] = nCell;
			m_CellStart[static_cast<size_t>(nCell) + 1]++;
		}
		for (size_t c = 1; c < m_CellStart.size(); ++c)
		{
			m_CellStart[c] += m_CellStart[c - 1];
		}

		// マスごとに並べ替える
		m_Sorted.resize(items.size());
		std::vector<int> cursor(m_CellStart.begin(), m_CellStart.end() - 1);
		for (size_t i = 0; i < items.size(); ++i)
		{
			m_Sorted[cursor[m_CellOfItem[i]]++] = items[i];
		}
	}

	// @brief 指定位置の周囲(自身のマスと隣接する8マス)にいるオブジェクトに対して、順に処理を行う
	// @param pos：基準位置
	// @param func：各オブジェクト(T*)に対して呼び出す処理
	// @note 1マスの大きさ以内の距離にいるオブジェクトは、必ず含まれる(距離の判定は呼び出し側で行う)
	template<typename F>
	void ForEachNear(const DirectX::XMFLOAT3& pos, F&& func) const
	{
		if (m_CellStart.empty()) return;

		const int cx = CellX(pos.x);
		const int cz = CellZ(pos.z);
		for (int z = (std::max)(cz - 1, 0); z <= (std::min)(cz + 1, m_nCountZ - 1); ++z)
		{
			for (int x = (std::max)(cx - 1, 0); x <= (std::min)(cx + 1, m_nCountX - 1); ++x)
			{
				const int nCell = CellIndex(x, z);
				for (int k = m_CellStart[nCell]; k < m_CellStart[static_cast<size_t>(nCell) + 1]; ++k)
				{
					func(m_Sorted[k]);
				}
			}
		}
	}

private:
	// @brief 位置からマスの番号(X方向)を求める(範囲外は端のマスに収める)
	int CellX(float fX) const { return (std::min)((std::max)(static_cast<int>(std::floor((fX - m_fOriginX) / m_fCellSize)), 0), m_nCountX - 1); }

	// @brief 位置からマスの番号(Z方向)を求める(範囲外は端のマスに収める)
	int CellZ(float fZ) const { return (std::min)((std::max)(static_cast<int>(std::floor((fZ - m_fOriginZ) / m_fCellSize)), 0), m_nCountZ - 1); }

	// @brief マスの番号(X, Z)から、一次元の番号を求める
	int CellIndex(int x, int z) const { return z * m_nCountX + x; }

private:
	float m_fCellSize = 1.0f;
	float m_fOriginX = 0.0f;
	float m_fOriginZ = 0.0f;
	int m_nCountX = 0;
	int m_nCountZ = 0;

	// @brief マスごとの先頭位置(最後に終端を足した、マスの数+1個)
	std::vector<int> m_CellStart;
	// @brief 各オブジェクトが入っているマスの番号
	std::vector<int> m_CellOfItem;
	// @brief マスごとに並べたオブジェクト
	std::vector<T*> m_Sorted;
};
