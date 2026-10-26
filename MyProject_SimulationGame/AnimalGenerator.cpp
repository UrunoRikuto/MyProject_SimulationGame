/**************************************************//*
	@file	| AnimalGenerator.cpp
	@brief	| 動物ジェネレータークラスのcppファイル
	@note	| 動物を生成するジェネレーター
			| IGeneratorを継承
*//**************************************************/
#include "AnimalGenerator.h"
#include "FieldManager.h"
#include "Main.h"
#include "Human.h"
#include "ImguiSystem.h"

#include "Wolf_Animal.h"
#include "Deer_Animal.h"
#include <cstdlib>
#include <vector>

namespace
{
	// @brief 動物の補充を確認する間隔(秒)
	constexpr float Animal_Respawn_Interval = 10.0f;

	// @brief 人間からこの距離以内には動物を補充しない
	constexpr float Animal_Spawn_Safe_Distance = 30.0f;

	// @brief 狼の数が「鹿の数 × この値」を超える場合は、狼を補充しない
	constexpr float Wolf_Respawn_Max_Deer_Ratio = 0.7f;

	// @brief 生存している動物を取得する
	template<typename T>
	std::vector<T*> GetAliveAnimals()
	{
		std::vector<T*> aliveList;
		for (T* pAnimal : GetScene()->GetGameObjects<T>())
		{
			if (!pAnimal->IsDestroy() && !pAnimal->IsDead())
			{
				aliveList.push_back(pAnimal);
			}
		}
		return aliveList;
	}
}

/*****************************************//*
	@brief　	| 生成処理(初期生成)
*//*****************************************/
void CAnimalGenerator::Generate()
{
	// フィールドセルの2次元配列を取得
	std::vector<std::vector<CFieldCell*>> pFieldCells = CFieldManager::GetInstance()->GetFieldGrid()->GetFieldCells();

	// 生成した動物のリスト
	std::vector<CWolf_Animal*> wolfList;
	std::vector<CDeer_Animal*> deerList;

	// フィールドセルを走査して動物を生成
	for (std::vector<CFieldCell*> cells : pFieldCells)
	{
		for (CFieldCell* cell : cells)
		{
			// オオカミ
			if (cell->GetTerritoryType() == CFieldCell::TerritoryType::Wolf && !cell->IsUse())
			{
				CWolf_Animal* pWolf = GetScene()->AddGameObject<CWolf_Animal>(Tag::GameObject, u8"狼");
				pWolf->SetPos(cell->GetPos());
				pWolf->RegisterToCell(cell->GetIndex());
				wolfList.push_back(pWolf);
				cell->SetUse(true);
			}
			// 鹿
			else if (cell->GetTerritoryType() == CFieldCell::TerritoryType::Deer && !cell->IsUse())
			{
				CDeer_Animal* pDeer = GetScene()->AddGameObject<CDeer_Animal>(Tag::GameObject, u8"鹿");
				pDeer->SetPos(cell->GetPos());
				pDeer->RegisterToCell(cell->GetIndex());
				deerList.push_back(pDeer);
				cell->SetUse(true);
			}
		}
	}
	// 群れの形成
	for (CWolf_Animal* wolf : wolfList)
	{
		wolf->RegisterToFlock(wolfList);
	}
	for (CDeer_Animal* deer : deerList)
	{
		deer->RegisterToFlock(deerList);
	}

	// 初期生成した数を、補充するときの目標数にする
	const int nDeerCount = static_cast<int>(deerList.size());
	const int nWolfCount = static_cast<int>(wolfList.size());
	m_nTargetDeer = (nDeerCount > m_nTargetDeer) ? nDeerCount : m_nTargetDeer;
	m_nTargetWolf = (nWolfCount > m_nTargetWolf) ? nWolfCount : m_nTargetWolf;
}

/*****************************************//*
	@brief　	| 更新処理
	@note		| 目標数に満たない動物を、一定間隔で1頭ずつ補充する
*//*****************************************/
void CAnimalGenerator::Update()
{
	// 初期生成が行われるまでは何もしない
	if (m_nTargetDeer <= 0 && m_nTargetWolf <= 0) return;

	// 補充の確認は一定間隔で行う
	m_fRespawnTimer -= 1.0f / fFPS;
	if (m_fRespawnTimer > 0.0f) return;
	m_fRespawnTimer = Animal_Respawn_Interval;

	// 生存している動物の数
	const int nDeerCount = static_cast<int>(GetAliveAnimals<CDeer_Animal>().size());
	const int nWolfCount = static_cast<int>(GetAliveAnimals<CWolf_Animal>().size());

	// 鹿は目標数に満たなければ補充する
	if (nDeerCount < m_nTargetDeer)
	{
		RespawnDeer();
	}

	// 狼は目標数に満たず、かつ鹿に対する比率の上限を超えない場合だけ補充する
	// (鹿が少ない状態で狼だけ増えて、鹿が枯渇してしまうのを防ぐ)
	if (nWolfCount < m_nTargetWolf &&
		static_cast<float>(nWolfCount + 1) <= Wolf_Respawn_Max_Deer_Ratio * static_cast<float>(nDeerCount))
	{
		RespawnWolf();
	}
}

/*****************************************//*
	@brief　	| 鹿を1頭補充する処理
*//*****************************************/
void CAnimalGenerator::RespawnDeer()
{
	// 補充できるセルがなければ何もしない
	std::vector<CFieldCell*> cells = CollectSpawnCells(CFieldCell::TerritoryType::Deer);
	if (cells.empty()) return;

	// 補充先のセルをランダムに選ぶ
	CFieldCell* pCell = cells[rand() % cells.size()];

	CDeer_Animal* pDeer = GetScene()->AddGameObject<CDeer_Animal>(Tag::GameObject, u8"鹿");
	if (pDeer == nullptr) return;
	pDeer->SetPos(pCell->GetPos());
	pDeer->RegisterToCell(pCell->GetIndex());
	pCell->SetUse(true);

	// 群れの登録
	pDeer->RegisterToFlock(GetAliveAnimals<CDeer_Animal>());

	// デバッグログに補充した旨を追加
	CImguiSystem::GetInstance()->AddDebugLog(std::string(u8"鹿を1頭補充しました(現在") + std::to_string(GetAliveAnimals<CDeer_Animal>().size()) + std::string(u8"頭 / 目標") + std::to_string(m_nTargetDeer) + std::string(u8"頭)"), false);
}

/*****************************************//*
	@brief　	| 狼を1頭補充する処理
*//*****************************************/
void CAnimalGenerator::RespawnWolf()
{
	// 補充できるセルがなければ何もしない
	std::vector<CFieldCell*> cells = CollectSpawnCells(CFieldCell::TerritoryType::Wolf);
	if (cells.empty()) return;

	// 補充先のセルをランダムに選ぶ
	CFieldCell* pCell = cells[rand() % cells.size()];

	CWolf_Animal* pWolf = GetScene()->AddGameObject<CWolf_Animal>(Tag::GameObject, u8"狼");
	if (pWolf == nullptr) return;
	pWolf->SetPos(pCell->GetPos());
	pWolf->RegisterToCell(pCell->GetIndex());
	pCell->SetUse(true);

	// 群れの登録(ホーム位置も設定される)
	pWolf->RegisterToFlock(GetAliveAnimals<CWolf_Animal>());

	// デバッグログに補充した旨を追加
	CImguiSystem::GetInstance()->AddDebugLog(std::string(u8"狼を1頭補充しました(現在") + std::to_string(GetAliveAnimals<CWolf_Animal>().size()) + std::string(u8"頭 / 目標") + std::to_string(m_nTargetWolf) + std::string(u8"頭)"), false);
}

/*****************************************//*
	@brief　	| 動物を補充できるセルを集める処理
	@param　	| eTerritory：補充する動物の縄張りタイプ
	@return　	| 補充できるセルのリスト
	@note		| 縄張り内で、何も置かれていない空き地(初期生成と同じ条件)のうち、
				| 人間から一定距離以上離れているセルだけを対象にする
*//*****************************************/
std::vector<CFieldCell*> CAnimalGenerator::CollectSpawnCells(CFieldCell::TerritoryType eTerritory) const
{
	// 人間の位置を取得
	std::vector<DirectX::XMFLOAT3> humanPositions;
	for (CHuman* pHuman : GetScene()->GetGameObjects<CHuman>())
	{
		if (!pHuman->IsDestroy())
		{
			humanPositions.push_back(pHuman->GetPos());
		}
	}

	const float fSafeDistanceSq = Animal_Spawn_Safe_Distance * Animal_Spawn_Safe_Distance;

	std::vector<CFieldCell*> spawnCells;
	for (const std::vector<CFieldCell*>& cells : CFieldManager::GetInstance()->GetFieldGrid()->GetFieldCells())
	{
		for (CFieldCell* pCell : cells)
		{
			// 縄張り内の、何も置かれていない空き地のみ
			if (pCell->GetTerritoryType() != eTerritory) continue;
			if (pCell->GetCellType() != CFieldCell::CellType::EMPTY) continue;
			if (pCell->IsUse()) continue;

			// 人間の近くには補充しない
			const DirectX::XMFLOAT3 cellPos = pCell->GetPos();
			bool bNearHuman = false;
			for (const DirectX::XMFLOAT3& humanPos : humanPositions)
			{
				const float dx = cellPos.x - humanPos.x;
				const float dz = cellPos.z - humanPos.z;
				if (dx * dx + dz * dz <= fSafeDistanceSq)
				{
					bNearHuman = true;
					break;
				}
			}
			if (bNearHuman) continue;

			spawnCells.push_back(pCell);
		}
	}

	return spawnCells;
}
