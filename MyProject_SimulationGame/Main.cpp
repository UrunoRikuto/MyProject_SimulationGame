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

	// 最初のシーンを設定
	g_pScene = new CSceneGame();
	g_pScene->Init();

	// トランジション初期化
	g_pTransition = new CTransition();
	g_pTransition->Init();

	// 最初はフェードイン
	FadeIn(nullptr);


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
	// 入力の更新
	UpdateInput();

	// シーンの更新
	if (CImguiSystem::GetInstance()->IsUpdate())
	{
		if (g_bSceneChanging)
		{
			g_pScene->Uninit();
			delete g_pScene;
			g_pScene = g_pNextScene;
			g_pScene->Init();
			g_bSceneChanging = false;
		}

		CCamera* pCamera = CCamera::GetInstance();
		pCamera->Update();
		g_pScene->Update();
		g_pTransition->Update();
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
	CImguiSystem::GetInstance()->Update();
}

/****************************************//*
	@brief　| 描画処理
*//****************************************/
void Draw()
{
	BeginDrawDirectX();

	g_pScene->Draw();
	g_pTransition->Draw();
	CImguiSystem::GetInstance()->Draw();

	EndDrawDirectX();
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
