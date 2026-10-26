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

// @brief 家の空き枠を確認する間隔(秒)
constexpr float Human_Vacancy_Check_Interval = 1.0f;


/*****************************************
	@brief　	| 生成処理
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
