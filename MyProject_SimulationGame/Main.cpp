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

// @brief 倍速時に、1フレームでゲームの更新に使ってよい時間(ミリ秒)
// @note これを超えた場合は、描画が止まらないよう残りの更新を打ち切る
constexpr float Max_Update_Time_Per_Frame_Ms = 12.0f;


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
void SetAutoProfile(float fSeconds, int nGameSpeed, int nExtraHumans)
{
	g_fAutoProfileSeconds = fSeconds;
	g_nAutoProfileGameSpeed = nGameSpeed;
	g_nAutoProfileExtraHumans = nExtraHumans;
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

	// 一定間隔で、無職の人間に職業を割り当てる
	if (++g_nAutoProfileJobTimer < 60) return;
	g_nAutoProfileJobTimer = 0;

	const std::string jobNames[] = {
		JobName::WoodGatherer, JobName::StoneGatherer, JobName::Builder, JobName::Pioneer, JobName::Cook,
		JobName::Hunter, JobName::GrassGatherer, JobName::Farmer, JobName::Smith
	};
	const int nJobCount = static_cast<int>(sizeof(jobNames) / sizeof(jobNames[0]));
	for (CHuman* pHuman : g_pScene->GetGameObjects<CHuman>())
	{
		if (pHuman->IsDestroy()) continue;
		if (pHuman->GetHumanJob()->GetJobName() != JobName::Neet) continue;

		pHuman->SetHumanJob(CreateJobByName(jobNames[g_nAutoProfileJobIndex % nJobCount], *pHuman));
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