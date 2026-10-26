/**************************************************//*
	@file	| HumanGenerator.cpp
	@brief	| 人間ジェネレータークラスのcppファイル
	@note	| 人間を生成するジェネレーター
			| IGeneratorを継承
*//**************************************************/
#include "HumanGenerator.h"
#include "Human.h"
#include "Main.h"
#include "ImguiSystem.h"
#include "HumanHouse.h"
#include "GeneratorManager.h"
#include "StorageHouse.h"
#include "Item.h"

// @brief 家の空き枠を確認する間隔(秒)
constexpr float Human_Vacancy_Check_Interval = 1.0f;

// @brief 人口が少ない間は食料に関係なく生まれる人数
constexpr int Human_Free_Birth_Population = 3;

// @brief 新しく生まれる人間1人あたりに必要な食料の備蓄(空腹回復値の合計)
constexpr float Human_Birth_Food_Reserve_Per_Human = 40.0f;

/*****************************************
	@brief　	| 食料の備蓄量を計算する処理
	@return		| 貯蔵庫と人間が持っている食料の空腹回復値の合計
*//*****************************************/
static float CalcFoodReserve(CScene* pScene)
{
	float fReserve = 0.0f;

	if (CStorageHouse* pStorage = pScene->GetGameObject<CStorageHouse>())
	{
		for (const CItem* pItem : pStorage->GetStoredItems())
		{
			fReserve += CItem::GetHungerRecoveryValue(pItem->GetItemType());
		}
	}

	for (CHuman* pHuman : pScene->GetGameObjects<CHuman>())
	{
		if (pHuman->IsDestroy()) continue;
		for (const CItem* pItem : pHuman->GetItemList())
		{
			fReserve += CItem::GetHungerRecoveryValue(pItem->GetItemType());
		}
	}

	return fReserve;
}

/*****************************************
	@brief　	| 生成処理
	@note		| 人口が少ない間を除き、新しい人間の分の食料の備蓄がない場合は生まれない
				| (食料がないまま生まれても、飢え死にと再生成を繰り返すだけになるため)
*//*****************************************/
void CHumanGenerator::Generate()
{
	// 人間の家を取得
	auto houses = GetScene()->GetGameObjects<CHumanHouse>();

	// 人間の家が存在しない場合は生成しない
	if(houses.empty())return;

	// 生成可能フラグ
	bool canGenerate = false;

	// 生成先の家ポインタ
	CHumanHouse* targetHouse = nullptr;

	// いずれかの家に空きがあるか確認
	for (auto& house : houses)
	{
		// 現在の居住者数が最大人数以下の場合
		if (static_cast<int>(house->GetResidentCount()) < house->GetMaxResidents())
		{
			// 生成可能
			canGenerate = true;
			// 生成先の家を保存
			targetHouse = house;
			break;
		}
	}

	// 生成不可の場合は処理終了
	if (!canGenerate)return;

	// 人口が増える分の食料の備蓄がない場合は生まれない
	int nPopulation = 0;
	for (CHuman* pHuman : GetScene()->GetGameObjects<CHuman>())
	{
		if (!pHuman->IsDestroy()) nPopulation++;
	}
	if (nPopulation >= Human_Free_Birth_Population &&
		CalcFoodReserve(GetScene()) < Human_Birth_Food_Reserve_Per_Human * static_cast<float>(nPopulation + 1))
	{
		return;
	}

	// 人間を生成
	CHuman* pHuman = GetScene()->AddGameObject<CHuman>(Tag::GameObject, u8"人間");

	// 生成した人間の位置を家の位置に設定
	pHuman->SetPos(targetHouse->GetPos());

	// 生成した人間と家を紐付ける
	targetHouse->AddResident(pHuman);
	pHuman->SetLivingHouse(targetHouse);
}

/*****************************************
	@brief　	| 更新処理
	@note		| 全ての家の空き枠を確認し、待機中の生成リクエストでは足りない分だけリクエストを追加する
*//*****************************************/
void CHumanGenerator::Update()
{
	// 空き枠の確認は負荷軽減のため一定間隔で行う
	m_fCheckTimer -= 1.0f / fFPS;
	if (m_fCheckTimer > 0.0f) return;
	m_fCheckTimer = Human_Vacancy_Check_Interval;

	// 全ての家の空き枠の合計を数える
	int nVacancy = 0;
	for (CHumanHouse* pHouse : GetScene()->GetGameObjects<CHumanHouse>())
	{
		if (pHouse->IsDestroy()) continue;

		const int nRest = pHouse->GetMaxResidents() - pHouse->GetResidentCount();
		if (nRest > 0) nVacancy += nRest;
	}

	// 待機中のリクエスト数が空き枠に足りない分だけ追加する
	CGeneratorManager* pGeneratorManager = CGeneratorManager::GetInstance();
	const int nPending = pGeneratorManager->CountRequests(CGeneratorManager::GenerateType::Human);
	for (int i = nPending; i < nVacancy; ++i)
	{
		pGeneratorManager->AddGenerateRequest({ CGeneratorManager::GenerateType::Human, Human_Regenerate_Wait_Time });
	}
}
