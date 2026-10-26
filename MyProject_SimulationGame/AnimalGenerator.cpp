/**************************************************//*
	@file	| AnimalGenerator.cpp
	@brief	| 動物ジェネレータークラスのcppファイル
	@note	| 動物を生成するジェネレーター
			| IGeneratorを継承
*//**************************************************/
#include "AnimalGenerator.h"
#include "FieldManager.h"
#include "FieldGrid.h"
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

	// @brief 1回の補充の確認で補充する頭数
	// @note フィールドが広いほど動物の数が多く、狩られる数も増えるため、広さに比例して増やす(100x100のときに1頭)
	constexpr int Animal_Respawn_Batch = (CFieldGrid::GridSizeX * CFieldGrid::GridSizeY) / (100 * 100);

	// @brief 補充先のセルを探すために、ランダムなセルを調べる最大回数
	constexpr int Animal_Spawn_Max_Tries = 1000;

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
			if (cell->GetTerritoryType() == CFieldCell::TerritoryType::Wolf && !cell->IsUse() && cell->GetCellType() == CFieldCell::CellType::EMPTY)
			{
				CWolf_Animal* pWolf = GetScene()->AddGameObject<CWolf_Animal>(Tag::GameObject, u8"狼");
				pWolf->SetPos(cell->GetPos());
				pWolf->RegisterToCell(cell->GetIndex());
				wolfList.push_back(pWolf);
				cell->SetUse(true);
			}
			// 鹿
			else if (cell->GetTerritoryType() == CFieldCell::TerritoryType::Deer && !cell->IsUse() && cell->GetCellType() == CFieldCell::CellType::EMPTY)
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
	for (int i = 0; i < Animal_Respawn_Batch && nDeerCount + i < m_nTargetDeer; ++i)
	{
		RespawnDeer();
	}

	// 狼は目標数に満たず、かつ鹿に対する比率の上限を超えない場合だけ補充する
	// (鹿が少ない状態で狼だけ増えて、鹿が枯渇してしまうのを防ぐ)
	for (int i = 0; i < Animal_Respawn_Batch && nWolfCount + i < m_nTargetWolf &&
		static_cast<float>(nWolfCount + i + 1) <= Wolf_Respawn_Max_Deer_Ratio * static_cast<float>(nDeerCount); ++i)
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
	CFieldCell* pCell = PickSpawnCell(CFieldCell::TerritoryType::Deer);
	if (pCell == nullptr) return;

	CDeer_Animal* pDeer = GetScene()->AddGameObject<CDeer_Animal>(Tag::GameObject, u8"鹿");
	if (pDeer == nullptr) return;
	pDeer->SetPos(pCell->GetPos());
	pDeer->RegisterToCell(pCell->GetIndex());
	pCell->SetUse(true);

	// 群れの登録(近隣の情報は、次の更新で最新の状態に作り直されるため、全頭を登録しなくてよい)
	pDeer->RegisterToFlock(CAnimal::s_bRefreshFlockInfo ? std::vector<CDeer_Animal*>() : GetAliveAnimals<CDeer_Animal>());

	// デバッグログに補充した旨を追加
	CImguiSystem::GetInstance()->AddDebugLog(std::string(u8"鹿を1頭補充しました(現在") + std::to_string(GetAliveAnimals<CDeer_Animal>().size()) + std::string(u8"頭 / 目標") + std::to_string(m_nTargetDeer) + std::string(u8"頭)"), false);
}

/*****************************************//*
	@brief　	| 狼を1頭補充する処理
*//*****************************************/
void CAnimalGenerator::RespawnWolf()
{
	// 補充できるセルがなければ何もしない
	CFieldCell* pCell = PickSpawnCell(CFieldCell::TerritoryType::Wolf);
	if (pCell == nullptr) return;

	CWolf_Animal* pWolf = GetScene()->AddGameObject<CWolf_Animal>(Tag::GameObject, u8"狼");
	if (pWolf == nullptr) return;
	pWolf->SetPos(pCell->GetPos());
	pWolf->RegisterToCell(pCell->GetIndex());
	pCell->SetUse(true);

	// 群れの登録(ホーム位置も設定される。近隣の情報は、次の更新で最新の状態に作り直される)
	pWolf->RegisterToFlock(CAnimal::s_bRefreshFlockInfo ? std::vector<CWolf_Animal*>() : GetAliveAnimals<CWolf_Animal>());

	// デバッグログに補充した旨を追加
	CImguiSystem::GetInstance()->AddDebugLog(std::string(u8"狼を1頭補充しました(現在") + std::to_string(GetAliveAnimals<CWolf_Animal>().size()) + std::string(u8"頭 / 目標") + std::to_string(m_nTargetWolf) + std::string(u8"頭)"), false);
}

/*****************************************//*
	@brief　	| 動物を補充するセルを1つ選ぶ処理
	@param　	| eTerritory：補充する動物の縄張りタイプ
	@return　	| 補充先のセル、見つからなかった場合はnullptr
	@note		| 縄張り内で、何も置かれていない空き地(初期生成と同じ条件)のうち、
				| 人間から一定距離以上離れているセルだけを対象にする
				| 全セルを調べず、ランダムなセルを一定回数だけ調べる(補充のたびに処理が止まらないようにするため)
*//*****************************************/
CFieldCell* CAnimalGenerator::PickSpawnCell(CFieldCell::TerritoryType eTerritory) const
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

	const std::vector<std::vector<CFieldCell*>>& fieldCells = CFieldManager::GetInstance()->GetFieldGrid()->GetFieldCells();
	for (int nTry = 0; nTry < Animal_Spawn_Max_Tries; ++nTry)
	{
		CFieldCell* pCell = fieldCells[rand() % CFieldGrid::GridSizeX][rand() % CFieldGrid::GridSizeY];

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

		return pCell;
	}

	return nullptr;
}
