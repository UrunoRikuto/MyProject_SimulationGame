/**************************************************//*
	@file	| GeneratorManager.cpp
	@brief	| 生成管理システム実装ファイル
	@note	| オブザーバーパターンで作成
			| シングルトンパターンで作成
*//**************************************************/
#include "GeneratorManager.h"
#include "GoalManager.h"
#include "Main.h"
#include "FieldManager.h"
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <vector>

namespace
{
	// @brief 1回の更新で、予約されている生成に使ってよい時間(ミリ秒)
	constexpr float Reserved_Spawn_Budget_Ms = 1.5f;

	// @brief 何個生成するごとに経過時間を確認するか
	constexpr int Reserved_Spawn_Check_Interval = 8;
}

/*****************************************
	@brief　	| デストラクタ
*//*****************************************/
CGeneratorManager::~CGeneratorManager()
{
	// 登録されているジェネレーターは、シーンの初期化時にnewして渡されたものなので、ここで破棄する
	for (IObserver* observer : m_Observers)
	{
		delete observer;
	}
	m_Observers.clear();
}

/*****************************************
	@brief　	| オブザーバーの通知処理
*//*****************************************/
template<typename T>
void CGeneratorManager::NotifyObserver()
{
	// 登録されている全オブザーバーに対して通知を行う
	for (IObserver* observer : m_Observers)
	{
		// ダウンキャストを試みる
		T* specificObserver = dynamic_cast<T*>(observer);

		// ダウンキャストに成功した場合のみ通知を行う
		if (specificObserver)
		{
			// 通知処理を呼び出す
			specificObserver->Notify();
		}
	}
}

/*****************************************
	@brief　	| 指定セルへの生成処理
	@param　	| n2Cell：生成先のセルのインデックス
*//*****************************************/
template<typename T>
void CGeneratorManager::GenerateAtCell(const DirectX::XMINT2& n2Cell)
{
	// セルの指定が無い場合は、そのジェネレーターに全体の生成を行わせる
	if (n2Cell.x < 0 || n2Cell.y < 0)
	{
		NotifyObserver<T>();
		return;
	}

	for (IObserver* observer : m_Observers)
	{
		// ダウンキャストに成功した場合のみ生成を行う
		if (T* specificObserver = dynamic_cast<T*>(observer))
		{
			specificObserver->GenerateAt(n2Cell);
		}
	}
}

/*****************************************
	@brief　	| 更新処理
*//*****************************************/
void CGeneratorManager::Update()
{
	// 予約されている初期生成を、制限時間内で少しずつ行う
	ProcessReservedSpawns();

	// 各ジェネレーターの更新処理(動物の補充や人間の空き枠の確認など)
	for (IObserver* observer : m_Observers)
	{
		if (IGenerator* pGenerator = dynamic_cast<IGenerator*>(observer))
		{
			pGenerator->Update();
		}
	}

	// 生成リクエストリストがない場合は処理を抜ける
	if (m_GenerateRequestList.empty())return;

	// 経過時間を進め、生成時間に達したリクエストを取り出す
	std::vector<GenerateRequest> firedRequests;
	const float fDeltaSec = 1.0f / fFPS;
	for (auto it = m_GenerateRequestList.begin(); it != m_GenerateRequestList.end();)
	{
		// 大飢餓の間は、木の再生成を待たせる(大飢餓が終わるまで、木は増えない)
		if (it->m_GenerateType == GenerateType::Wood && CGoalManager::IsFamine())
		{
			++it;
			continue;
		}

		// 経過時間を加算
		it->m_fGenerateTime -= fDeltaSec;

		if (it->m_fGenerateTime <= 0.0f)
		{
			// 生成時間に達した場合は取り出してリストから削除
			firedRequests.push_back(*it);
			it = m_GenerateRequestList.erase(it);
		}
		else
		{
			++it;
		}
	}

	// 取り出したリクエストの生成処理を行う
	// (生成中に新しいリクエストが追加されても影響しないよう、リストの走査とは分けて行う)
	for (const GenerateRequest& request : firedRequests)
	{
		// 生成タイプに応じたオブザーバーへの通知処理
		switch (request.m_GenerateType)
		{
		case GenerateType::Wood:
			GenerateAtCell<CWoodGenerator>(request.m_n2CellIndex);
			break;
		case GenerateType::Stone:
			GenerateAtCell<CStoneGenerator>(request.m_n2CellIndex);
			break;
		case GenerateType::Grass:
			GenerateAtCell<CGrassGenerator>(request.m_n2CellIndex);
			break;
		case GenerateType::Human:
			NotifyObserver<CHumanGenerator>();
			break;
		}
	}
}

/*****************************************
	@brief　	| オブザーバーへの通知処理
*//*****************************************/
void CGeneratorManager::NotifyObservers()
{
	for (IObserver* observer : m_Observers)
	{
		observer->Notify();
	}
}

/*****************************************
	@brief　	| 生成リクエストの追加
	@param　	| request 生成リクエスト情報
*//*****************************************/
void CGeneratorManager::AddGenerateRequest(GenerateRequest request)
{
	// 生成リクエストリストに追加
	m_GenerateRequestList.push_back(request);
}

/*****************************************
	@brief　	| 環境オブジェクトの再生成リクエストの追加
	@param　	| eType：再生成するタイプ(Wood/Stone/Grass)
	@param　	| n2Cell：再生成するセルのインデックス
*//*****************************************/
void CGeneratorManager::RequestRegenerate(GenerateType eType, const DirectX::XMINT2& n2Cell)
{
	// タイプごとの再生成待ち時間の範囲を決定
	float fMinTime = 0.0f;
	float fMaxTime = 0.0f;
	switch (eType)
	{
	case GenerateType::Wood:
		fMinTime = Wood_Regenerate_Min_Time;
		fMaxTime = Wood_Regenerate_Max_Time;
		break;
	case GenerateType::Stone:
		fMinTime = Stone_Regenerate_Min_Time;
		fMaxTime = Stone_Regenerate_Max_Time;
		break;
	case GenerateType::Grass:
		fMinTime = Grass_Regenerate_Min_Time;
		fMaxTime = Grass_Regenerate_Max_Time;
		break;
	default:
		// 環境オブジェクト以外は対象外
		return;
	}

	// 範囲内でランダムな待ち時間にして、再生成のタイミングをばらけさせる
	const float fRate = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	AddGenerateRequest({ eType, fMinTime + (fMaxTime - fMinTime) * fRate, n2Cell });
}

/*****************************************
	@brief　	| 待機中の生成リクエスト数の取得
	@param　	| eType：数えるタイプ
	@return　	| 待機中のリクエスト数
*//*****************************************/
int CGeneratorManager::CountRequests(GenerateType eType) const
{
	int nCount = 0;
	for (const GenerateRequest& request : m_GenerateRequestList)
	{
		if (request.m_GenerateType == eType)
		{
			nCount++;
		}
	}
	return nCount;
}

/*****************************************
	@brief　	| 初期生成の予約処理
	@param　	| eType：生成するタイプ
	@param　	| n2Cell：生成先のセルのインデックス
*//*****************************************/
void CGeneratorManager::ReserveInitialSpawn(GenerateType eType, const DirectX::XMINT2& n2Cell)
{
	// フィールドの中心(原点)からの距離を求める
	const DirectX::XMFLOAT3 cellPos = CFieldManager::GetInstance()->GetFieldGrid()->GetFieldCells()[n2Cell.x][n2Cell.y]->GetPos();
	m_ReservedSpawns.push_back({ eType, n2Cell, cellPos.x * cellPos.x + cellPos.z * cellPos.z });
	m_bReservedSorted = false;
}

/*****************************************
	@brief　	| 予約されている生成の処理
	@note		| 1回の更新で使ってよい時間を超えたら、残りは次の更新に回す
				| 初期の村に近いものから生成するため、最初に見える範囲は早く揃う
*//*****************************************/
void CGeneratorManager::ProcessReservedSpawns()
{
	if (m_ReservedSpawns.empty()) return;

	// 中心から遠い順に並べる(末尾から取り出すことで、近いものから生成する)
	if (!m_bReservedSorted)
	{
		std::sort(m_ReservedSpawns.begin(), m_ReservedSpawns.end(),
			[](const ReservedSpawn& a, const ReservedSpawn& b) { return a.m_fDistanceSq > b.m_fDistanceSq; });
		m_bReservedSorted = true;
	}

	const auto startTime = std::chrono::steady_clock::now();
	int nSpawned = 0;
	while (!m_ReservedSpawns.empty())
	{
		const ReservedSpawn spawn = m_ReservedSpawns.back();
		m_ReservedSpawns.pop_back();

		switch (spawn.m_GenerateType)
		{
		case GenerateType::Wood:
			GenerateAtCell<CWoodGenerator>(spawn.m_n2CellIndex);
			break;
		case GenerateType::Stone:
			GenerateAtCell<CStoneGenerator>(spawn.m_n2CellIndex);
			break;
		case GenerateType::Grass:
			GenerateAtCell<CGrassGenerator>(spawn.m_n2CellIndex);
			break;
		default:
			break;
		}

		// 一定数ごとに経過時間を確認し、制限時間を超えていたら残りは次の更新で行う
		if (++nSpawned % Reserved_Spawn_Check_Interval == 0)
		{
			const float fElapsedMs = std::chrono::duration<float, std::milli>(std::chrono::steady_clock::now() - startTime).count();
			if (fElapsedMs > Reserved_Spawn_Budget_Ms) break;
		}
	}
}
