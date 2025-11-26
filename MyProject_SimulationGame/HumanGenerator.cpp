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

	// 生成ログ出力
	CImguiSystem::GetInstance()->AddDebugLog("HumanGenerator: Generate [Human]", false);

	// 人間を生成
	CHuman* pHuman = GetScene()->AddGameObject<CHuman>(Tag::GameObject, "Human");

	// 生成した人間の位置を家の位置に設定
	pHuman->SetPos(targetHouse->GetPos());

	// 生成した人間と家を紐付ける
	targetHouse->AddResident(pHuman);
	pHuman->SetLivingHouse(targetHouse);
}
