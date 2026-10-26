/**********************************************************************************//*
    @file		| Main.cpp
    @brief		| メイン処理
*//***********************************************************************************/
#include "Main.h"
#include "DirectX.h"
#include "Geometory.h"
#include "Sprite.h"
#include "Input.h"
#include "Transition.h"
#include "Camera.h"
#include "ObjectLoad.h"
#include "ImguiSystem.h"
#include "ShaderManager.h"
#include <chrono>
#include "Profiler.h"
#include "Human.h"
#include "JobOperator.h"
#include "CivLevelManager.h"
#include "GameTimeManager.h"
#include "StorageHouse.h"
#include "Hunter_Job.h"
#include "HerbivorousAnimal.h"
#include "CarnivorousAnimal.h"
#include <map>

// @brief 倍速時に、1フレームでゲームの更新に使ってよい時間(ミリ秒)
// @note 処理が重い環境では、描画のフレームレートが下がっても倍速を優先する(30FPS程度まで)
//       これを超えた場合は、描画が止まらないよう残りの更新を打ち切る
constexpr float Max_Update_Time_Per_Frame_Ms = 33.0f;


// 現在のシーンポインタ
CScene* g_pScene = nullptr;
// 次のシーンポインタ
CScene* g_pNextScene = nullptr;

// トランジションポインタ
CTransition* g_pTransition = nullptr;

// ウィンドウハンドル
HWND g_hWnd = nullptr;
// カメラ種別
CameraKind g_ekind;

// シーン切り替え中フラグ
bool g_bSceneChanging = false;

// Imgui有効フラグ
bool g_bOnImgui = false;

// 自動計測モード(起動引数 --profile=秒数)の設定
float g_fAutoProfileSeconds = 0.0f;
int g_nAutoProfileGameSpeed = 1;
int g_nAutoProfileExtraHumans = 0;
// 自動計測の開始時に進めておくゲーム内の日数(0未満の場合は進めない)
int g_nAutoProfileStartDay = -1;
// 無職の人間に割り当てる職業を固定する場合の職業(空の場合は全ての職業を順番に割り当てる)
std::string g_sAutoProfileJob;
// 自動計測用の準備(人間の追加など)が済んだかどうか
bool g_bAutoProfileReady = false;
// 次に無職の人間へ割り当てる職業の番号
int g_nAutoProfileJobIndex = 0;
// 無職の人間を確認するまでのフレーム数のカウンタ
int g_nAutoProfileJobTimer = 0;

/****************************************//*
	@brief　	| 自動計測モードの設定
	@param　	| fSeconds：計測する秒数
	@param　	| nGameSpeed：計測中のゲーム速度(倍速)
	@param　	| nExtraHumans：計測用に追加する人間の数
*//****************************************/
void SetAutoProfile(float fSeconds, int nGameSpeed, int nExtraHumans, const char* pJobKey, int nStartDay)
{
	g_nAutoProfileStartDay = nStartDay;
	g_sAutoProfileJob = pJobKey != nullptr ? pJobKey : "";
	g_fAutoProfileSeconds = fSeconds;
	g_nAutoProfileGameSpeed = nGameSpeed;
	g_nAutoProfileExtraHumans = nExtraHumans;
}

// @brief 自動計測中のバランス確認用のログを出力する(ゲーム内の一定時間ごとにCSVへ1行追記)
static void WriteBalanceLog()
{
	constexpr int Log_Interval_Game_Seconds = 10;

	CGameTimeManager* pTime = CGameTimeManager::GetInstance();
	const int nTotalSeconds = pTime->GetGameDays() * 1000 + static_cast<int>(pTime->GetGameTime());
	static int s_nLastLogged = -1;
	const int nSlot = pTime->GetGameDays() * 1000 + static_cast<int>(pTime->GetGameTime()) / Log_Interval_Game_Seconds;
	if (nSlot == s_nLastLogged) return;
	s_nLastLogged = nSlot;

	// 人間の集計
	int nHumans = 0, nHungry = 0, nEating = 0, nResting = 0, nForaging = 0, nCarriedFood = 0;
	float fHungerSum = 0.0f;
	std::map<std::string, int> jobCount;
	for (CHuman* pHuman : g_pScene->GetGameObjects<CHuman>())
	{
		if (pHuman->IsDestroy()) continue;
		nHumans++;
		fHungerSum += pHuman->GetHunger();
		if (pHuman->GetHunger() < Warning_Hunger) nHungry++;
		if (pHuman->GetHumanState() == CHuman::HUMAN_STATE::Eating) nEating++;
		if (pHuman->GetHumanState() == CHuman::HUMAN_STATE::Resting) nResting++;
		if (pHuman->GetHumanState() == CHuman::HUMAN_STATE::Foraging) nForaging++;
		jobCount[pHuman->GetHumanJob()->GetJobName()]++;
		for (const CItem* pItem : pHuman->GetItemList())
		{
			if (pItem->GetItemCategory() == CItem::ITEM_CATEGORY::CookedFood || pItem->GetItemCategory() == CItem::ITEM_CATEGORY::UnCookedFood) nCarriedFood++;
		}
	}

	// 貯蔵庫の食料の集計
	int nApple = 0, nMeat = 0, nBread = 0, nPie = 0, nSeed = 0, nWheat = 0, nSugar = 0;
	if (CStorageHouse* pStorage = g_pScene->GetGameObject<CStorageHouse>())
	{
		for (const CItem* pItem : pStorage->GetStoredItems())
		{
			switch (pItem->GetItemType())
			{
			case CItem::ITEM_TYPE::Apple: nApple++; break;
			case CItem::ITEM_TYPE::Meat: nMeat++; break;
			case CItem::ITEM_TYPE::Bread: nBread++; break;
			case CItem::ITEM_TYPE::ApplePie: nPie++; break;
			case CItem::ITEM_TYPE::WheatSeed: nSeed++; break;
			case CItem::ITEM_TYPE::Wheat: nWheat++; break;
			case CItem::ITEM_TYPE::Sugar: nSugar++; break;
			default: break;
			}
		}
	}

	int nDeer = static_cast<int>(g_pScene->GetGameObjects<CHerbivorousAnimal>().size());
	int nWolf = static_cast<int>(g_pScene->GetGameObjects<CCarnivorousAnimal>().size());

	// 動物がフィールドの中心から最も離れている距離(フィールドの外に出ていないかの確認用)
	float fAnimalMaxExtent = 0.0f;
	g_pScene->ForEachGameObject<CAnimal>([&](CAnimal* pAnimal)
	{
		const DirectX::XMFLOAT3 animalPos = pAnimal->GetPos();
		fAnimalMaxExtent = (std::max)(fAnimalMaxExtent, (std::max)(fabsf(animalPos.x), fabsf(animalPos.z)));
	});

	FILE* pFile = nullptr;
	static bool s_bHeader = false;
	fopen_s(&pFile, "balance_log.csv", s_bHeader ? "a" : "w");
	if (pFile == nullptr) return;
	if (!s_bHeader)
	{
		fprintf(pFile, "gameSec,day,humans,deaths,avgHunger,hungry,eating,resting,foraging,carriedFood,apple,meat,bread,pie,deer,wolf,animalMaxExtent,huntSearch,noHerbivore,tooDangerous,huntStart,huntKill,giveUpTime,giveUpFar,preyLost,seedStored,wheatStored,sugarStored,seedMade,seedLost,wheatMade,wheatLost,breadMade,breadLost,appleMade,appleLost,sugarMade,sugarLost,pieMade,pieLost,meatMade,meatLost,jobs\n");
		s_bHeader = true;
	}
	fprintf(pFile, "%d,%d,%d,%d,%.1f,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%.1f,", nTotalSeconds, pTime->GetGameDays(), nHumans, CHuman::GetDeathCount(),
		nHumans > 0 ? fHungerSum / static_cast<float>(nHumans) : 0.0f, nHungry, nEating, nResting, nForaging, nCarriedFood, nApple, nMeat, nBread, nPie, nDeer, nWolf, fAnimalMaxExtent);
	const CHunter_Job::HuntStats& hunt = CHunter_Job::GetStats();
	fprintf(pFile, "%d,%d,%d,%d,%d,%d,%d,%d,", hunt.nSearch, hunt.nNoHerbivore, hunt.nTooDangerous, hunt.nStarted, hunt.nKill, hunt.nGiveUpTime, hunt.nGiveUpFar, hunt.nPreyLost);
	fprintf(pFile, "%d,%d,%d,", nSeed, nWheat, nSugar);
	const CItem::ITEM_TYPE ledgerTypes[] = { CItem::ITEM_TYPE::WheatSeed, CItem::ITEM_TYPE::Wheat, CItem::ITEM_TYPE::Bread, CItem::ITEM_TYPE::Apple, CItem::ITEM_TYPE::Sugar, CItem::ITEM_TYPE::ApplePie, CItem::ITEM_TYPE::Meat };
	for (CItem::ITEM_TYPE eType : ledgerTypes)
	{
		fprintf(pFile, "%d,%d,", CItem::GetCreatedCount(eType), CItem::GetDestroyedCount(eType));
	}
	for (const auto& kv : jobCount) fprintf(pFile, "%d ", kv.second);
	fprintf(pFile, "\n");
	fclose(pFile);
}

/****************************************//*
	@brief　	| 自動計測用の処理
	@note		| ゲームシーンの開始後に、人間の追加と、無職の人間への職業の割り当てを行う
*//****************************************/
static void UpdateAutoProfile()
{
	if (g_fAutoProfileSeconds <= 0.0f) return;
	if (dynamic_cast<CSceneGame*>(g_pScene) == nullptr) return;

	// 最初の1回だけ、全ての職業を解放して人間を追加する
	if (!g_bAutoProfileReady)
	{
		g_bAutoProfileReady = true;
		CCivLevelManager::GetInstance()->SetCivLevel(12);

		// 指定された日数まで、ゲーム内の時間を進めておく(1日は120秒。日の中の時間は朝の30秒にする)
		if (g_nAutoProfileStartDay >= 0)
		{
			CGameTimeManager::GetInstance()->SetGameTime(static_cast<float>(g_nAutoProfileStartDay) * 120.0f + 30.0f);
		}
		const std::list<CHuman*> humans = g_pScene->GetGameObjects<CHuman>();
		if (!humans.empty())
		{
			const DirectX::XMFLOAT3 basePos = humans.front()->GetPos();
			for (int i = 0; i < g_nAutoProfileExtraHumans; ++i)
			{
				CHuman* pHuman = g_pScene->AddGameObject<CHuman>(Tag::GameObject, "Human");
				if (pHuman != nullptr) pHuman->SetPos(basePos);
			}
		}
	}

	// バランス確認用のログ
	WriteBalanceLog();

	// 一定間隔で、無職の人間に職業を割り当てる
	if (++g_nAutoProfileJobTimer < 60) return;
	g_nAutoProfileJobTimer = 0;

	const std::string jobNames[] = {
		JobName::WoodGatherer, JobName::StoneGatherer, JobName::Builder, JobName::Pioneer, JobName::Cook,
		JobName::Hunter, JobName::GrassGatherer, JobName::Farmer, JobName::Smith
	};
	const int nJobCount = static_cast<int>(sizeof(jobNames) / sizeof(jobNames[0]));

	// 職業を固定する指定がある場合は、その職業だけを割り当てる(wood, stone, builder, pioneer, cook, hunter, grass, farmer, smith)
	const char* jobKeys[] = { "wood", "stone", "builder", "pioneer", "cook", "hunter", "grass", "farmer", "smith" };
	int nFixedJob = -1;
	for (int i = 0; i < nJobCount; ++i)
	{
		if (g_sAutoProfileJob == jobKeys[i]) nFixedJob = i;
	}
	for (CHuman* pHuman : g_pScene->GetGameObjects<CHuman>())
	{
		if (pHuman->IsDestroy()) continue;
		if (pHuman->GetHumanJob()->GetJobName() != JobName::Neet) continue;

		const int nJob = (nFixedJob >= 0) ? nFixedJob : (g_nAutoProfileJobIndex % nJobCount);
		pHuman->SetHumanJob(CreateJobByName(jobNames[nJob], *pHuman));
		g_nAutoProfileJobIndex++;
	}
}


/****************************************//*
	@brief　	| 初期化処理
	@param　	| hWnd：ウィンドウハンドル
	@param　	| width：画面幅
	@param　	| height：画面高さ
	@return　	| 初期化に成功したかどうか
*//****************************************/
HRESULT Init(HWND hWnd, UINT width, UINT height)
{
	HRESULT hr;
	// DirectX初期化
	hr = InitDirectX(hWnd, width, height, false);
	// 初期化の例
	if (FAILED(hr)) { return hr; }
	g_hWnd = hWnd;

	srand(timeGetTime());

	// Imgui初期化
	CImguiSystem::GetInstance()->Init();

	// オブジェクトのロード
	CObjectLoad::LoadAll();

	// ジオメトリ初期化
	Geometory::Init();

	// スプライト初期化
	Sprite::Init();

	// シェーダーマネージャー初期化
	CShaderManager::GetInstance()->LoadShaders();

	// 入力初期化
	InitInput();

	// カメラ種別初期化
	g_ekind = CameraKind::CAM_TITLE;
	// カメラ初期化
	CCamera::GetInstance()->SetCameraKind(g_ekind);
	// カメラの初期化処理
	CCamera::GetInstance()->Init();

	// 最初のシーンを設定
	g_pScene = new CSceneTitle();
	g_pScene->Init();

	// トランジション初期化
	g_pTransition = new CTransition();
	g_pTransition->Init();

	// 最初はフェードイン
	FadeIn(nullptr);

	// 自動計測モードの場合は、計測を開始して、タイトル画面を飛ばしてゲームを開始する
	if (g_fAutoProfileSeconds > 0.0f)
	{
		CProfiler::GetInstance()->StartAutoRun(g_fAutoProfileSeconds, "profile_report.txt");
		CImguiSystem::GetInstance()->DecideSettingSeed();
		CImguiSystem::GetInstance()->SetGameSpeed(g_nAutoProfileGameSpeed);
		ChangeScene(new CSceneGame());
		SetOnImgui(true);
	}


	return hr;
}

/****************************************//*
	@brief　| 終了処理
*//****************************************/
void Uninit()
{
	// シーンの終了処理
	g_pScene->Uninit();
	delete g_pScene;
	g_pScene = nullptr;

	// トランジションの終了処理
	g_pTransition->Uninit();
	delete g_pTransition;
	g_pTransition = nullptr;

	// 入力の終了処理
	UninitInput();

	// シェーダーの終了処理
	CShaderManager::ReleaseInstance();

	// スプライトの終了処理
	Sprite::Uninit();

	// ジオメトリの終了処理
	Geometory::Uninit();

	// オブジェクトのアンロード
	CObjectLoad::UnLoadAll();

	// Imguiの終了処理
	CImguiSystem::GetInstance()->Uninit();
	CImguiSystem::ReleaseInstance();

	// DirectXの終了処理
	UninitDirectX();
}

/****************************************//*
	@brief　| 更新処理
*//****************************************/
void Update()
{
	PROFILE_SCOPE("Main::Update");

	// 入力の更新
	UpdateInput();

	// シーンの更新
	if (CImguiSystem::GetInstance()->IsUpdate() || !g_bOnImgui)
	{
		if (g_bSceneChanging)
		{
			PROFILE_SCOPE("Update: Scene change (Uninit/Init)");
			g_pScene->Uninit();
			delete g_pScene;
			g_pScene = g_pNextScene;
			g_pScene->Init();
			g_bSceneChanging = false;
		}

		UpdateAutoProfile();

		CCamera* pCamera = CCamera::GetInstance();
		{
			PROFILE_SCOPE("Update: Camera");
			pCamera->Update();
		}

		// ゲームシーンのみ、倍速の設定の回数だけ更新を行う(1フレーム=固定の時間刻みで動いているため、更新回数がそのまま倍率になる)
		// 更新内で押した瞬間の入力を読むタイトルシーンなどは、同じ入力を何度も受け取らないよう倍速にしない
		CImguiSystem* pImguiSystem = CImguiSystem::GetInstance();
		const int nGameSpeed = (dynamic_cast<CSceneGame*>(g_pScene) != nullptr) ? pImguiSystem->GetGameSpeed() : 1;

		int nUpdateCount = 0;
		const auto updateStartTime = std::chrono::steady_clock::now();
		for (int i = 0; i < nGameSpeed; ++i)
		{
			{
				PROFILE_SCOPE("Update: Scene::Update");
				g_pScene->Update();
			}
			nUpdateCount++;

			// 処理に時間がかかり過ぎている場合は、描画が止まらないよう残りの更新を打ち切る
			const float fElapsedMs = std::chrono::duration<float, std::milli>(std::chrono::steady_clock::now() - updateStartTime).count();
			if (fElapsedMs > Max_Update_Time_Per_Frame_Ms) break;
		}
		pImguiSystem->SetAppliedGameSpeed(nUpdateCount);

		{
			PROFILE_SCOPE("Update: Transition");
			g_pTransition->Update();
		}
	}

	// 終了コマンド
	if (IsKeyPress(VK_ESCAPE))
	{
		if(IsKeyTrigger(VK_DELETE))
		{
			AppEnd();
		}
	}

	// Imguiの更新
	{
		PROFILE_SCOPE("Update: ImguiSystem");
		if(g_bOnImgui)CImguiSystem::GetInstance()->Update();
	}

	// 自動計測の時間が終わったら、レポートを出力してアプリケーションを終了する
	if (CProfiler::GetInstance()->UpdateAutoRun()) AppEnd();
}

/****************************************//*
	@brief　| 描画処理
*//****************************************/
void Draw()
{
	{
		PROFILE_SCOPE("Main::Draw");

		{
			PROFILE_SCOPE("Draw: BeginDraw(Clear)");
			BeginDrawDirectX();
		}
		{
			PROFILE_SCOPE("Draw: Scene::Draw");
			g_pScene->Draw();
		}
		{
			PROFILE_SCOPE("Draw: Transition");
			g_pTransition->Draw();
		}
		{
			PROFILE_SCOPE("Draw: ImGui");
			if(g_bOnImgui)CImguiSystem::GetInstance()->Draw();
		}
		{
			PROFILE_SCOPE("Draw: EndDraw(Present)");
			EndDrawDirectX();
		}
	}

	// プロファイラのフレームの区切り
	if (CProfiler::IsEnabled()) CProfiler::GetInstance()->EndFrame();
}

/****************************************//*
	@brief　| 現在のシーン取得
	@return　| 現在のシーンポインタ
*//****************************************/
CScene* GetScene()
{
	return g_pScene;
}

/****************************************//*
	@brief　| シーン変更
	@param　| inScene：変更先のシーンポインタ
*//****************************************/
void ChangeScene(CScene* inScene)
{
	g_pNextScene = inScene;
	g_bSceneChanging = true;
}

/****************************************//*
	@brief　| フェードイン
	@param　| onFadeComplete：フェードイン完了時に呼ばれる関数
*//****************************************/
void FadeIn(std::function<void()> onFadeComplete)
{
	g_pTransition->FadeIn(50, onFadeComplete);
}

/****************************************//*
	@brief　| フェードアウト
	@param　| onFadeComplete：フェードアウト完了時に呼ばれる関数
*//****************************************/
void FadeOut(std::function<void()> onFadeComplete)
{
	g_pTransition->FadeOut(50, onFadeComplete);
}

/****************************************//*
	@brief　| セットImgui有効フラグ
	@param　| bOnImgui：Imgui有効フラグ
*//****************************************/
void SetOnImgui(bool bOnImgui)
{
	g_bOnImgui = bOnImgui;
}