/**************************************************//*
	@file	| GoalManager.cpp
	@brief	| 目標管理クラスのcppファイル
	@note	| 大飢餓までの進行、大飢餓の開始と終了、クリアとゲームオーバーの判定、画面表示を行う
*//**************************************************/
#include "GoalManager.h"
#include "imgui.h"
#include "Main.h"
#include "Scene.h"
#include "SceneGame.h"
#include "SceneTitle.h"
#include "Defines.h"
#include "GameTimeManager.h"
#include "CivLevelManager.h"
#include "ImguiSystem.h"
#include "Human.h"
#include "Deer_Animal.h"
#include "Wolf_Animal.h"
#include "BuildObject.h"
#include "StorageHouse.h"
#include "Item.h"
#include <algorithm>
#include <cmath>
#include <cstdio>

namespace
{
	// @brief 人口と食料の数を集計し直す間隔(ゲームの更新の回数。60回で、ゲーム内の1秒)
	constexpr int Count_Interval_Ticks = 60;

	// @brief お知らせを表示する時間(秒)
	constexpr double Notice_Show_Seconds = 7.0;

	// @brief 目標の表示の位置と大きさ
	constexpr float Hud_Pos_X = 254.0f;
	constexpr float Hud_Width = 184.0f;

	// @brief 結果の画面の幅
	constexpr float Result_Width = 520.0f;

	// @brief 自動計測用に、結果を書き出すファイル
	const char* const Result_Log_File = "goal_result.txt";

	// @brief 0～1に収めた値を、なめらかに変化する値にする
	float SmoothStep01(float fT)
	{
		fT = std::clamp(fT, 0.0f, 1.0f);
		return fT * fT * (3.0f - 2.0f * fT);
	}
}

/*****************************************//*
	@brief　	| 新しいゲームを始めるときの初期化
*//*****************************************/
void CGoalManager::Start()
{
	s_ePhase = Phase::Normal;
	s_fFamineVisual = 0.0f;
	s_eResult = Result::None;
	s_bResultOpen = false;
	s_nTick = 0;
	s_nPopulation = 0;
	s_nFoodStock = 0;
	s_nBaseDeathCount = CHuman::GetDeathCount();
	s_Stats = Stats();
	s_dNoticeStart = -1.0;
}

/*****************************************//*
	@brief　	| ゲームシーン中かどうか
*//*****************************************/
bool CGoalManager::IsGameScene()
{
	return dynamic_cast<CSceneGame*>(GetScene()) != nullptr;
}

/*****************************************//*
	@brief　	| ゲーム内の経過日数(小数部あり)の取得
*//*****************************************/
float CGoalManager::GetElapsedDays()
{
	CGameTimeManager* pTime = CGameTimeManager::GetInstance();
	return static_cast<float>(pTime->GetGameDays()) + pTime->GetDayProgress();
}

/*****************************************//*
	@brief　	| 生存している人間の数を数える
*//*****************************************/
int CGoalManager::CountSurvivors()
{
	int nCount = 0;
	for (CHuman* pHuman : GetScene()->GetGameObjects<CHuman>())
	{
		if (!pHuman->IsDestroy()) nCount++;
	}
	return nCount;
}

/*****************************************//*
	@brief　	| 貯蔵庫の食料の数を数える
	@note		| 空腹を回復できるアイテムを、食料として数える
*//*****************************************/
int CGoalManager::CountFoodStock()
{
	CStorageHouse* pStorage = GetScene()->GetGameObject<CStorageHouse>();
	if (pStorage == nullptr) return 0;

	int nCount = 0;
	for (const CItem* pItem : pStorage->GetStoredItems())
	{
		if (CItem::GetHungerRecoveryValue(pItem->GetItemType()) > 0.0f) nCount++;
	}
	return nCount;
}

/*****************************************//*
	@brief　	| 建築が完了している施設の数を数える
*//*****************************************/
int CGoalManager::CountFacilities()
{
	int nCount = 0;
	for (CBuildObject* pBuild : GetScene()->GetGameObjects<CBuildObject>())
	{
		if (!pBuild->IsDestroy() && pBuild->IsCompleted()) nCount++;
	}
	return nCount;
}

/*****************************************//*
	@brief　	| 人口と食料の表示用の値を最新にする
*//*****************************************/
void CGoalManager::RefreshCounts()
{
	s_nPopulation = CountSurvivors();
	s_nFoodStock = CountFoodStock();
	s_Stats.nPeakPopulation = (std::max)(s_Stats.nPeakPopulation, s_nPopulation);
}

/*****************************************//*
	@brief　	| 大飢餓が始まったときの処理
	@note		| 獲物の鹿と、狼が大きく減る(以降、大飢餓の間は鹿は補充されない)
*//*****************************************/
void CGoalManager::OnFamineStart()
{
	RefreshCounts();
	s_Stats.nFoodAtFamineStart = s_nFoodStock;

	// 鹿と狼を、決まった割合だけ減らす(一定の間隔で選び、偏らないようにする)
	int nAccumulate = 0;
	for (CDeer_Animal* pDeer : GetScene()->GetGameObjects<CDeer_Animal>())
	{
		if (pDeer->IsDestroy()) continue;
		nAccumulate += Animal_Loss_Percent;
		if (nAccumulate >= 100)
		{
			nAccumulate -= 100;
			pDeer->Destroy();
		}
	}
	nAccumulate = 0;
	for (CWolf_Animal* pWolf : GetScene()->GetGameObjects<CWolf_Animal>())
	{
		if (pWolf->IsDestroy()) continue;
		nAccumulate += Animal_Loss_Percent;
		if (nAccumulate >= 100)
		{
			nAccumulate -= 100;
			pWolf->Destroy();
		}
	}
}

/*****************************************//*
	@brief　	| 大飢餓が終わったときの、結果の判定
*//*****************************************/
void CGoalManager::Judge()
{
	RefreshCounts();

	s_Stats.nDays = static_cast<int>(GetElapsedDays());
	s_Stats.nSurvivors = s_nPopulation;
	s_Stats.nCivLevel = CCivLevelManager::GetInstance()->GetCivLevel();
	s_Stats.nFacilities = CountFacilities();
	s_Stats.nDeaths = CHuman::GetDeathCount() - s_nBaseDeathCount;

	s_eResult = (s_nPopulation >= Required_Survivors) ? Result::Cleared : Result::Failed;
	s_bResultOpen = !s_bHeadless;

	// 自動計測のときは、結果をファイルに残す(バランスの確認用)
	if (s_bHeadless)
	{
		FILE* pFile = nullptr;
		fopen_s(&pFile, Result_Log_File, "w");
		if (pFile != nullptr)
		{
			fprintf(pFile, "result=%s day=%d survivors=%d peak=%d civ=%d facilities=%d deaths=%d foodAtFamineStart=%d foodNow=%d\n",
				s_eResult == Result::Cleared ? "clear" : "gameover", s_Stats.nDays, s_Stats.nSurvivors, s_Stats.nPeakPopulation,
				s_Stats.nCivLevel, s_Stats.nFacilities, s_Stats.nDeaths, s_Stats.nFoodAtFamineStart, s_nFoodStock);
			fclose(pFile);
		}
	}
}

/*****************************************//*
	@brief　	| お知らせを表示する
	@param　	| sTitle：見出し
	@param　	| sBody：本文
*//*****************************************/
void CGoalManager::ShowNotice(const std::string& sTitle, const std::string& sBody)
{
	s_sNoticeTitle = sTitle;
	s_sNoticeBody = sBody;
	s_dNoticeStart = ImGui::GetTime();
}

/*****************************************//*
	@brief　	| 更新処理
*//*****************************************/
void CGoalManager::Update()
{
	// 一定間隔で、人口と食料を数える
	if (s_nTick++ % Count_Interval_Ticks == 0) RefreshCounts();

	// 荒廃した見た目の強さ(大飢餓が近づくと強くなり、終わると弱くなる)
	{
		const float fElapsedDays = GetElapsedDays();
		const float fFamineStart = static_cast<float>(Famine_Start_Day);
		const float fFamineEnd = static_cast<float>(Famine_Start_Day + Famine_Duration_Days);
		const float fRampIn = SmoothStep01((fElapsedDays - (fFamineStart - Visual_Ramp_In_Days)) / (Visual_Ramp_In_Days + 1.0f));
		const float fRampOut = 1.0f - SmoothStep01((fElapsedDays - fFamineEnd) / Visual_Ramp_Out_Days);
		s_fFamineVisual = fRampIn * fRampOut;
	}

	// 結果が決まった後は、段階の進行は止める
	if (s_eResult != Result::None) return;

	// 経過日数から、段階を求める
	const float fElapsed = GetElapsedDays();
	Phase eNext = Phase::Normal;
	if (fElapsed >= static_cast<float>(Famine_Start_Day + Famine_Duration_Days)) eNext = Phase::Finished;
	else if (fElapsed >= static_cast<float>(Famine_Start_Day)) eNext = Phase::Famine;
	else if (fElapsed >= static_cast<float>(Famine_Start_Day - Warning_Lead_Days)) eNext = Phase::Warning;

	if (eNext == s_ePhase) return;

	// 大飢餓の開始を飛ばして(自動計測の日数指定などで)進んだ場合も、開始の処理は行う
	if (eNext == Phase::Famine || (eNext == Phase::Finished && s_ePhase != Phase::Famine))
	{
		OnFamineStart();
	}

	s_ePhase = eNext;

	switch (eNext)
	{
	case Phase::Warning:
		ShowNotice(u8"凶兆", u8"遠くで不吉な気配がする…。\n約10日後(1年後)に、大飢餓が訪れる。\n食料を蓄え、人口を増やして備えよう。");
		break;
	case Phase::Famine:
		ShowNotice(u8"大飢餓が始まった!", u8"木の実は実りにくく、麦は育ちにくく、木も獲物も増えなくなる。\n蓄えを守り、20人以上で生き残れ。");
		break;
	case Phase::Finished:
		Judge();
		break;
	default:
		break;
	}
}

/*****************************************//*
	@brief　	| 描画処理
	@param　	| pFont：表示に使うフォント
*//*****************************************/
void CGoalManager::Draw(ImFont* pFont)
{
	if (s_bHeadless || !IsGameScene()) return;

	ImGui::PushFont(pFont);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.05f, 0.07f, 0.12f, 0.88f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.45f, 0.55f, 0.90f, 0.90f));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 0.35f, 0.75f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.35f, 0.45f, 0.90f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.20f, 0.30f, 0.65f, 1.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);

	DrawHud();
	DrawNotice();
	DrawResult();

	ImGui::PopStyleVar();
	ImGui::PopStyleColor(5);
	ImGui::PopFont();
}

/*****************************************//*
	@brief　	| 目標の表示
*//*****************************************/
void CGoalManager::DrawHud()
{
	ImGui::SetNextWindowPos(ImVec2(Hud_Pos_X, 0.0f));
	ImGui::SetNextWindowSizeConstraints(ImVec2(Hud_Width, 0.0f), ImVec2(Hud_Width, 300.0f));
	ImGui::Begin("Goal", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar);
	ImGui::SetWindowFontScale(0.58f);

	const ImVec4 colorTitle(1.0f, 0.9f, 0.4f, 1.0f);
	const ImVec4 colorAlert(1.0f, 0.45f, 0.4f, 1.0f);
	const ImVec4 colorGood(0.5f, 1.0f, 0.6f, 1.0f);

	const int nDaysToStart = static_cast<int>(std::ceil(static_cast<float>(Famine_Start_Day) - GetElapsedDays()));
	const int nDaysToEnd = static_cast<int>(std::ceil(static_cast<float>(Famine_Start_Day + Famine_Duration_Days) - GetElapsedDays()));
	const bool bEnough = s_nPopulation >= Required_Survivors;

	ImGui::TextColored(colorTitle, u8"目標");
	ImGui::TextColored(colorTitle, u8"大飢餓を%d人以上で生き残る", Required_Survivors);
	ImGui::Separator();

	if (s_eResult == Result::Continued)
	{
		ImGui::TextColored(colorGood, u8"大飢餓を乗り越えた! (目標達成)");
		ImGui::Text(u8"人口: %d人", s_nPopulation);
	}
	else if (s_ePhase == Phase::Famine)
	{
		ImGui::TextColored(colorAlert, u8"大飢餓の最中! 残り%d日", (std::max)(nDaysToEnd, 0));
		ImGui::TextColored(bEnough ? colorGood : colorAlert, u8"生存者: %d / %d人", s_nPopulation, Required_Survivors);
		ImGui::Text(u8"食料の備蓄: %d個", s_nFoodStock);
	}
	else
	{
		if (s_ePhase == Phase::Warning) ImGui::TextColored(colorAlert, u8"凶兆! 大飢餓まであと%d日", (std::max)(nDaysToStart, 0));
		else ImGui::Text(u8"大飢餓まであと%d日", (std::max)(nDaysToStart, 0));
		ImGui::TextColored(bEnough ? colorGood : ImVec4(1.0f, 1.0f, 1.0f, 1.0f), u8"人口: %d / %d人", s_nPopulation, Required_Survivors);
		ImGui::Text(u8"食料の備蓄: %d個", s_nFoodStock);
	}

	ImGui::End();
}

/*****************************************//*
	@brief　	| お知らせの表示
*//*****************************************/
void CGoalManager::DrawNotice()
{
	if (s_dNoticeStart < 0.0) return;

	const double dElapsed = ImGui::GetTime() - s_dNoticeStart;
	if (dElapsed > Notice_Show_Seconds)
	{
		s_dNoticeStart = -1.0;
		return;
	}

	// 出る時と消える時は、なめらかに透明にする
	const float fAlpha = static_cast<float>(std::clamp((std::min)(dElapsed, Notice_Show_Seconds - dElapsed) / 0.5, 0.0, 1.0));

	ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH / 2.0f, 135.0f), ImGuiCond_Always, ImVec2(0.5f, 0.0f));
	ImGui::SetNextWindowSizeConstraints(ImVec2(420.0f, 0.0f), ImVec2(420.0f, 400.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, fAlpha);
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.5f, 0.35f, 1.0f));
	ImGui::Begin("GoalNotice", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);
	ImGui::SetWindowFontScale(0.85f);
	ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.4f, 1.0f), "%s", s_sNoticeTitle.c_str());
	ImGui::SetWindowFontScale(0.65f);
	ImGui::Separator();
	ImGui::PushTextWrapPos(400.0f);
	ImGui::TextUnformatted(s_sNoticeBody.c_str());
	ImGui::PopTextWrapPos();
	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
}

/*****************************************//*
	@brief　	| 結果の画面の表示
*//*****************************************/
void CGoalManager::DrawResult()
{
	if (!s_bResultOpen) return;

	const bool bCleared = (s_eResult == Result::Cleared);

	// 画面全体を暗くする(他のウィンドウの上、結果のウィンドウの下に重ねる)
	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
	ImGui::SetNextWindowSize(ImVec2(SCREEN_WIDTH, SCREEN_HEIGHT));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.6f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::Begin("GoalDim", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoFocusOnAppearing);
	ImGui::End();
	ImGui::PopStyleVar();
	ImGui::PopStyleColor();

	ImGui::SetNextWindowFocus();

	ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSizeConstraints(ImVec2(Result_Width, 0.0f), ImVec2(Result_Width, 600.0f));
	ImGui::PushStyleColor(ImGuiCol_Border, bCleared ? ImVec4(1.0f, 0.85f, 0.3f, 1.0f) : ImVec4(0.8f, 0.3f, 0.3f, 1.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 3.0f);
	ImGui::Begin("GoalResult", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar);

	// 見出し
	ImGui::SetWindowFontScale(1.1f);
	if (bCleared) ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.3f, 1.0f), u8"大飢餓を乗り越えた!");
	else ImGui::TextColored(ImVec4(1.0f, 0.45f, 0.4f, 1.0f), u8"文明は大飢餓に耐えられなかった…");

	ImGui::SetWindowFontScale(0.7f);
	ImGui::Separator();
	ImGui::PushTextWrapPos(Result_Width - 24.0f);
	if (bCleared)
	{
		ImGui::Text(u8"%d人の人間が、厳しい大飢餓を生き延びた。文明は、これからも続いていく。", s_Stats.nSurvivors);
	}
	else
	{
		ImGui::Text(u8"生き残ったのは%d人(必要な人数は%d人)。次は、もっと備えて挑もう。", s_Stats.nSurvivors, Required_Survivors);
	}
	ImGui::PopTextWrapPos();
	ImGui::Spacing();

	// 集計
	if (ImGui::BeginTable("GoalStats", 2, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_RowBg))
	{
		auto row = [](const char* pName, const std::string& sValue)
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::TextUnformatted(pName);
			ImGui::TableSetColumnIndex(1);
			ImGui::TextUnformatted(sValue.c_str());
		};
		row(u8"経過日数", std::to_string(s_Stats.nDays) + u8"日");
		row(u8"生存者", std::to_string(s_Stats.nSurvivors) + u8"人 (最大" + std::to_string(s_Stats.nPeakPopulation) + u8"人)");
		row(u8"亡くなった人数", std::to_string(s_Stats.nDeaths) + u8"人");
		row(u8"文明レベル", std::to_string(s_Stats.nCivLevel));
		row(u8"建てた施設", std::to_string(s_Stats.nFacilities) + u8"個");
		row(u8"大飢餓の前の食料の備蓄", std::to_string(s_Stats.nFoodAtFamineStart) + u8"個");
		ImGui::EndTable();
	}
	ImGui::Spacing();

	// ボタン
	if (bCleared)
	{
		if (ImGui::Button(u8"続ける"))
		{
			s_eResult = Result::Continued;
			s_bResultOpen = false;
		}
	}
	else
	{
		if (ImGui::Button(u8"もう一度挑む"))
		{
			s_bResultOpen = false;
			ChangeScene(new CSceneGame());
		}
	}
	ImGui::SameLine();
	if (ImGui::Button(u8"タイトルへ"))
	{
		s_bResultOpen = false;
		ChangeScene(new CSceneTitle());
		SetOnImgui(false);
	}

	ImGui::End();
	ImGui::PopStyleVar();
	ImGui::PopStyleColor();
}
