/**********************************************************************************//*
    @file		|Main.cpp
    @brief		|メイン処理
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

const static int DEBUG_GRID_NUM = 20;			// グリッドの数
const static float DEBUG_GRID_MARGIN = 1.0f;	// グリッドの間隔

//-- グローバル変数

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
// デバッグモードフラグ
bool g_bDebugMode = false;
// シーン切り替え中フラグ
bool g_bSceneChanging = false;


/*************************//*
	@brief		|初期化
	@param[in]	|hWnd	ウィンドウハンドル
	@param[in] 	|width 画面幅
	@param[in] 	|height 画面高さ
*//*************************/
HRESULT Init(HWND hWnd, UINT width, UINT height)
{
	HRESULT hr;
	// DirectX初期化
	hr = InitDirectX(hWnd, width, height, false);
	// 初期化の例
	if (FAILED(hr)) { return hr; }
	g_hWnd = hWnd;

	// Imgui初期化
	CImguiSystem::GetInstance()->Init();

	// オブジェクトのロード
	CObjectLoad::LoadAll();

	// ジオメトリ初期化
	Geometory::Init();

	// スプライト初期化
	Sprite::Init();

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

/*************************//*
	@brief 	| 終了
*//*************************/
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

/*************************//*
	@brief 	| 更新
*//*************************/
void Update()
{
	// 入力の更新
	UpdateInput();
	srand(timeGetTime());

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

	if (IsKeyPress('U'))
	{
		if (IsKeyTrigger('I'))
		{
			g_bDebugMode ^= true;
		}
	}

	// Imguiの更新
	CImguiSystem::GetInstance()->Update();
}

/*************************//*
	@brief 	| 描画
*//*************************/
void Draw()
{
	BeginDrawDirectX();

	// デバッグモード時はグリッドと軸を描画
	if (g_bDebugMode)
	{
		// グリッド
		DirectX::XMFLOAT4 lineColor(0.5f, 0.5f, 0.5f, 1.0f);
		float size = DEBUG_GRID_NUM * DEBUG_GRID_MARGIN;
		for (int i = 1; i <= DEBUG_GRID_NUM; ++i)
		{
			float grid = i * DEBUG_GRID_MARGIN;
			DirectX::XMFLOAT3 pos[2] = {
				DirectX::XMFLOAT3(grid, 0.0f, size),
				DirectX::XMFLOAT3(grid, 0.0f,-size),
			};
			Geometory::AddLine(pos[0], pos[1], lineColor);
			pos[0].x = pos[1].x = -grid;
			Geometory::AddLine(pos[0], pos[1], lineColor);
			pos[0].x = size;
			pos[1].x = -size;
			pos[0].z = pos[1].z = grid;
			Geometory::AddLine(pos[0], pos[1], lineColor);
			pos[0].z = pos[1].z = -grid;
			Geometory::AddLine(pos[0], pos[1], lineColor);
		}
		// 軸
		Geometory::AddLine(DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(size, 0, 0), DirectX::XMFLOAT4(1, 0, 0, 1));
		Geometory::AddLine(DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, size, 0), DirectX::XMFLOAT4(0, 1, 0, 1));
		Geometory::AddLine(DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, size), DirectX::XMFLOAT4(0, 0, 1, 1));
		Geometory::AddLine(DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(-size, 0, 0), DirectX::XMFLOAT4(0, 0, 0, 1));
		Geometory::AddLine(DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, -size), DirectX::XMFLOAT4(0, 0, 0, 1));

		Geometory::DrawLines();
	}

	g_pScene->Draw();
	g_pTransition->Draw();
	if (g_bDebugMode)CImguiSystem::GetInstance()->Draw();

	EndDrawDirectX();
}

/*************************//*
	@brief 	| シーン取得
	@return	| シーンポインタ
*//*************************/
CScene* GetScene()
{
	return g_pScene;
}

/*************************//*
	@brief 		| シーン変更
	@param[in]	|inScene：シーンポインタ
*//*************************/
void ChangeScene(CScene* inScene)
{
	g_pNextScene = inScene;
	g_bSceneChanging = true;
}

/*************************//*
	@brief 		| フェードイン
	@param[in]	|onFadeComplete：フェードイン完了時コールバック関数
*//*************************/
void FadeIn(std::function<void()> onFadeComplete)
{
	g_pTransition->FadeIn(50, onFadeComplete);
}

/*************************//*
	@brief 		| フェードアウト
	@param[in]	|onFadeComplete：フェードアウト完了時コールバック関数
*//*************************/
void FadeOut(std::function<void()> onFadeComplete)
{
	g_pTransition->FadeOut(50, onFadeComplete);
}

/*************************//*
	@brief 	| デバックモードかどうか取得
	@return	| true:デバッグモード false:通常モード
*//*************************/
bool IsDebugMode()
{
	return g_bDebugMode;
}
