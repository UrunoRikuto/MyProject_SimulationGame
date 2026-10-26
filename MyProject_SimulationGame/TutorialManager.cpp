/**************************************************//*
	@file	| TutorialManager.cpp
	@brief	| チュートリアル管理クラスのcppファイル
	@note	| 本編の画面の上に、手順ごとの説明とハイライトを重ねて表示するチュートリアル
*//**************************************************/
#include "TutorialManager.h"
#include "imgui.h"
#include "Main.h"
#include "Scene.h"
#include "Defines.h"
#include "Camera.h"
#include "ImguiSystem.h"
#include "Human.h"
#include "StorageHouse.h"
#include "CivLevelManager.h"
#include "JobOperator.h"
#include <cmath>
#include <cstdio>

namespace
{
	// @brief 完了済みかどうかを記録するファイル
	constexpr const char* Tutorial_Done_File = "tutorial_done.txt";

	// @brief カメラを動かしたと見なす移動量
	constexpr float Camera_Moved_Threshold = 12.0f;

	// @brief 進行条件を満たしてから、次の手順へ進むまでの時間(秒)
	constexpr float Complete_Wait_Time = 1.2f;

	// @brief チュートリアル中、人間の空腹度をこの値より下げない(餓死しないようにするため)
	constexpr float Tutorial_Min_Hunger = 60.0f;

	// @brief 手順の説明ウィンドウの幅(高さは内容に合わせて決まる)
	// @note 左の建築物ウィンドウ(x400まで)と右の人間ウィンドウ(x880から)の間に収める。重なると操作を奪われる
	constexpr float Window_Width = 470.0f;

	// @brief 手順の途中で、文明レベルをここまで上げる(建築職の解放)
	constexpr int Tutorial_Civ_Level = 3;
}

/*****************************************//*
	@brief　	| 手順のリスト
*//*****************************************/
const std::vector<CTutorialManager::Step>& CTutorialManager::GetSteps()
{
	static const std::vector<Step> steps =
	{
		{ u8"ようこそ",
		  u8"人間たちに職業を割り当てて、文明を発展させるゲームです。\n目標は、文明レベルを上げて、全ての職業(レベル12)を解放すること。\nまずは基本の操作を覚えましょう。",
		  Condition::Manual, Area::None },

		{ u8"カメラを動かそう",
		  u8"W/A/S/D で前後左右、Space/Shift で上下に移動、矢印キーで向きを変えられます。\n少し動かしてみましょう。",
		  Condition::CameraMoved, Area::None },

		{ u8"人間を選ぼう",
		  u8"画面右上の「人間選択」か、その左の「人間一覧」から、人間を1人選んでください。\n人間が増えたら、一覧から名前を押して選ぶと便利です。",
		  Condition::HumanSelected, Area::HumanSelect },

		{ u8"職業を割り当てよう",
		  u8"選んだ人間の「職業」を、「木の採取職」などに変更しましょう。\n職業を持つ人間は、自分で資源を集めに行きます。\n「人間一覧」の職業欄からも変更でき、職業ごとの+/-ボタンで人数をまとめて増減できます。",
		  Condition::JobAssigned, Area::HumanWindow },

		{ u8"資源は貯蔵庫へ",
		  u8"集めた資源は貯蔵庫に運ばれます。左下の「建築物選択」から貯蔵庫を選ぶと、中身を確認できます。\nしばらく待つと資源が貯まります。",
		  Condition::StoredItem, Area::BuildWindow },

		{ u8"空腹と食料",
		  u8"人間は時間とともに空腹になり、貯蔵庫の食料(リンゴや肉)を自分で食べに行きます。\n食料が足りなくなると餓死するので、採取や狩猟で食料を確保しましょう。\n(チュートリアル中は餓死しません)",
		  Condition::Manual, Area::HumanWindow },

		{ u8"文明レベル",
		  u8"資源を集めたり建築したりすると経験値が貯まり、文明レベルが上がります。\nレベルが上がると、新しい職業が選べるようになります。\n(今回は特別に経験値をあげました)",
		  Condition::Manual, Area::CivLevel },

		{ u8"建築職を割り当てよう",
		  u8"文明レベル3で「建築職」が解放されました。\n建築職の人間は、必要な建物(家や施設)を自動で建てていきます。",
		  Condition::BuilderAssigned, Area::HumanWindow },

		{ u8"ゲーム速度",
		  u8"1～4キーで、ゲーム速度(1倍/2倍/4倍/8倍)を変えられます。\nキーを押してみましょう。",
		  Condition::SpeedChanged, Area::GameSpeed },

		{ u8"昼夜と季節",
		  u8"時間とともに昼夜が巡り、10日ごとに春・夏・秋・冬が切り替わります。\n冬は木の実(リンゴ)が取れないので、秋までに食料をためておきましょう。",
		  Condition::Manual, Area::GameTime },

		{ u8"準備完了!",
		  u8"基本は以上です。ここからは自由に、文明を発展させてください。\nチュートリアルは、画面下のボタンからいつでも見直せます。",
		  Condition::Manual, Area::None },
	};
	return steps;
}

/*****************************************//*
	@brief　	| ゲームシーンかどうか
*//*****************************************/
bool CTutorialManager::IsGameScene()
{
	return dynamic_cast<CSceneGame*>(GetScene()) != nullptr;
}

/*****************************************//*
	@brief　	| 完了済みかどうかの記録の読み込み
*//*****************************************/
bool CTutorialManager::IsDoneRecorded()
{
	FILE* pFile = nullptr;
	fopen_s(&pFile, Tutorial_Done_File, "r");
	if (pFile == nullptr) return false;
	fclose(pFile);
	return true;
}

/*****************************************//*
	@brief　	| 完了したことの記録
*//*****************************************/
void CTutorialManager::RecordDone()
{
	FILE* pFile = nullptr;
	fopen_s(&pFile, Tutorial_Done_File, "w");
	if (pFile == nullptr) return;
	fputs("done\n", pFile);
	fclose(pFile);
}

/*****************************************//*
	@brief　	| チュートリアルを最初から始める
*//*****************************************/
void CTutorialManager::Start()
{
	if (!s_bEnabled) return;

	s_bActive = true;
	EnterStep(0);
}

/*****************************************//*
	@brief　	| チュートリアルを終える
	@param　	| bMarkDone：true:完了として記録する
*//*****************************************/
void CTutorialManager::Finish(bool bMarkDone)
{
	s_bActive = false;
	if (bMarkDone) RecordDone();
}

/*****************************************//*
	@brief　	| 手順を始めた時の処理
*//*****************************************/
void CTutorialManager::EnterStep(int nIndex)
{
	s_nStep = nIndex;
	s_fCompleteTimer = -1.0f;

	// 基準値の記録
	s_f3LastCameraPos = CCamera::GetInstance()->GetPos();
	s_f3LastCameraLook = CCamera::GetInstance()->GetLook();
	s_fCameraMoved = 0.0f;
	s_nBaseGameSpeed = CImguiSystem::GetInstance()->GetGameSpeed();
	if (CStorageHouse* pStorage = GetScene()->GetGameObject<CStorageHouse>())
	{
		s_nBaseStoredCount = static_cast<int>(pStorage->GetStoredItems().size());
	}

	// 文明レベルの手順では、建築職が解放されるレベルまで経験値を与える
	if (nIndex == 6)
	{
		CCivLevelManager* pCivLevel = CCivLevelManager::GetInstance();
		while (pCivLevel->GetCivLevel() < Tutorial_Civ_Level)
		{
			pCivLevel->AddExp(CCivLevelManager::ExpType::Gathering);
		}
	}
}

/*****************************************//*
	@brief　	| 次の手順へ進む
*//*****************************************/
void CTutorialManager::NextStep()
{
	if (s_nStep + 1 >= static_cast<int>(GetSteps().size()))
	{
		Finish(true);
		return;
	}
	EnterStep(s_nStep + 1);
}

/*****************************************//*
	@brief　	| 手順の進行条件を満たしているかどうかの判定
*//*****************************************/
bool CTutorialManager::CheckCondition(Condition eCondition)
{
	CImguiSystem* pImgui = CImguiSystem::GetInstance();
	switch (eCondition)
	{
	case Condition::Manual:
		return false;

	case Condition::CameraMoved:
		return s_fCameraMoved >= Camera_Moved_Threshold;

	case Condition::HumanSelected:
		return pImgui->GetSelectedHuman() != nullptr;

	case Condition::JobAssigned:
	{
		CHuman* pHuman = pImgui->GetSelectedHuman();
		return pHuman != nullptr && pHuman->GetHumanJob()->GetJobName() != JobName::Neet;
	}

	case Condition::StoredItem:
	{
		CStorageHouse* pStorage = GetScene()->GetGameObject<CStorageHouse>();
		return pStorage != nullptr && static_cast<int>(pStorage->GetStoredItems().size()) > s_nBaseStoredCount;
	}

	case Condition::BuilderAssigned:
		for (CHuman* pHuman : GetScene()->GetGameObjects<CHuman>())
		{
			if (!pHuman->IsDestroy() && pHuman->GetHumanJob()->GetJobName() == JobName::Builder) return true;
		}
		return false;

	case Condition::SpeedChanged:
		return pImgui->GetGameSpeed() != s_nBaseGameSpeed;
	}
	return false;
}

/*****************************************//*
	@brief　	| 毎フレームの更新処理
*//*****************************************/
void CTutorialManager::Update()
{
	// ゲームシーン以外では何もしない(ゲームシーンを離れたら、次のゲーム開始時に確認し直す)
	if (!s_bEnabled || !IsGameScene())
	{
		s_bAutoChecked = false;
		s_bActive = false;
		return;
	}

	// ゲームシーンに入って最初の1回だけ、まだ完了していなければ自動で始める
	if (!s_bAutoChecked)
	{
		s_bAutoChecked = true;
		if (!IsDoneRecorded()) Start();
	}

	if (!s_bActive) return;

	// チュートリアル中は、人間が餓死しないようにする
	for (CHuman* pHuman : GetScene()->GetGameObjects<CHuman>())
	{
		if (!pHuman->IsDestroy() && pHuman->GetHunger() < Tutorial_Min_Hunger)
		{
			pHuman->RecoverHunger(Tutorial_Min_Hunger - pHuman->GetHunger());
		}
	}

	// カメラの移動量を積算する
	CCamera* pCamera = CCamera::GetInstance();
	const DirectX::XMFLOAT3 pos = pCamera->GetPos();
	const DirectX::XMFLOAT3 look = pCamera->GetLook();
	auto distance = [](const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b)
	{
		const float dx = a.x - b.x, dy = a.y - b.y, dz = a.z - b.z;
		return std::sqrt(dx * dx + dy * dy + dz * dz);
	};
	s_fCameraMoved += distance(pos, s_f3LastCameraPos) + distance(look, s_f3LastCameraLook);
	s_f3LastCameraPos = pos;
	s_f3LastCameraLook = look;

	// 進行条件を満たしたら、少し待ってから次の手順へ進む
	const Step& step = GetSteps()[s_nStep];
	if (s_fCompleteTimer < 0.0f)
	{
		if (CheckCondition(step.eCondition)) s_fCompleteTimer = Complete_Wait_Time;
	}
	else
	{
		s_fCompleteTimer -= 1.0f / 60.0f;
		if (s_fCompleteTimer <= 0.0f) NextStep();
	}
}

/*****************************************//*
	@brief　	| 描画処理
*//*****************************************/
void CTutorialManager::Draw(ImFont* pFont)
{
	if (!s_bEnabled || !IsGameScene()) return;

	ImGui::PushFont(pFont);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.08f, 0.10f, 0.24f, 0.93f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.55f, 0.65f, 1.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 0.35f, 0.75f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.35f, 0.45f, 0.90f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.20f, 0.30f, 0.65f, 1.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0f);

	if (!s_bActive)
	{
		// チュートリアルの外では、画面下に見直すためのボタンだけを表示する
		ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT - 10.0f), ImGuiCond_Always, ImVec2(0.5f, 1.0f));
		ImGui::Begin("TutorialButton", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);
		ImGui::SetWindowFontScale(0.7f);
		if (ImGui::Button(u8"チュートリアル")) Start();
		ImGui::End();
	}
	else
	{
		const Step& step = GetSteps()[s_nStep];

		// ハイライト(枠を点滅させる)
		ImVec2 areaMin(0, 0), areaMax(0, 0);
		bool bArea = true;
		switch (step.eArea)
		{
		case Area::HumanSelect:	areaMin = ImVec2(SCREEN_WIDTH - 400.0f, 40.0f);	areaMax = ImVec2(SCREEN_WIDTH, 200.0f); break;
		case Area::HumanWindow:	areaMin = ImVec2(SCREEN_WIDTH - 400.0f, 0.0f);	areaMax = ImVec2(SCREEN_WIDTH, 700.0f); break;
		case Area::CivLevel:	areaMin = ImVec2(0.0f, 0.0f);					areaMax = ImVec2(250.0f, 100.0f); break;
		case Area::GameTime:	areaMin = ImVec2(0.0f, 100.0f);					areaMax = ImVec2(230.0f, 400.0f); break;
		case Area::GameSpeed:	areaMin = ImVec2(SCREEN_WIDTH / 2 - 150.0f, 0.0f);	areaMax = ImVec2(SCREEN_WIDTH / 2 + 150.0f, 60.0f); break;
		case Area::BuildWindow:	areaMin = ImVec2(0.0f, SCREEN_HEIGHT - 400.0f);	areaMax = ImVec2(400.0f, SCREEN_HEIGHT); break;
		default: bArea = false; break;
		}
		if (bArea)
		{
			const float pulse = 0.55f + 0.45f * std::sin(static_cast<float>(ImGui::GetTime()) * 5.0f);
			ImGui::GetForegroundDrawList()->AddRect(areaMin, areaMax, ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 0.85f, 0.2f, pulse)), 8.0f, 0, 5.0f);
		}

		// 説明ウィンドウ(画面下の中央。幅は固定で、高さは内容に合わせる)
		ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT - 10.0f), ImGuiCond_Always, ImVec2(0.5f, 1.0f));
		ImGui::SetNextWindowSizeConstraints(ImVec2(Window_Width, 0.0f), ImVec2(Window_Width, 10000.0f));
		ImGui::Begin("Tutorial", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::SetWindowFontScale(0.72f);

		// 見出しと、手順の番号
		ImGui::TextColored(ImVec4(1.0f, 0.9f, 0.4f, 1.0f), u8"%s", step.pTitle);
		ImGui::SameLine(Window_Width - 150.0f);
		ImGui::TextColored(ImVec4(0.7f, 0.8f, 1.0f, 1.0f), u8"%d / %d", s_nStep + 1, static_cast<int>(GetSteps().size()));
		ImGui::Separator();

		// 説明
		ImGui::PushTextWrapPos(Window_Width - 24.0f);
		ImGui::TextUnformatted(step.pBody);
		ImGui::PopTextWrapPos();

		// 進行の状態
		if (s_fCompleteTimer >= 0.0f)
		{
			ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.5f, 1.0f), u8"できました!");
		}
		else if (step.eCondition != Condition::Manual)
		{
			ImGui::TextColored(ImVec4(0.7f, 0.8f, 1.0f, 1.0f), u8"操作すると、次へ進みます");
		}

		// ボタン
		ImGui::Spacing();
		const bool bLast = (s_nStep + 1 >= static_cast<int>(GetSteps().size()));
		if (step.eCondition == Condition::Manual)
		{
			if (ImGui::Button(bLast ? u8"閉じる" : u8"次へ")) NextStep();
		}
		else
		{
			if (ImGui::Button(u8"この手順をスキップ")) NextStep();
		}
		if (!bLast)
		{
			ImGui::SameLine();
			if (ImGui::Button(u8"チュートリアルを終了")) Finish(true);
		}

		ImGui::End();
	}

	ImGui::PopStyleVar();
	ImGui::PopStyleColor(5);
	ImGui::PopFont();
}
