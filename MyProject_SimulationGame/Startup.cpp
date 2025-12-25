/**********************************************************************************//*
    @file		|Startup.cpp
    @brief		|起動時処理
*//***********************************************************************************/
#pragma warning(disable: 4819)
#include <windows.h>
#include "Main.h"
#include <stdio.h>
#include <crtdbg.h>
#include "Defines.h"
#include "imgui_impl_win32.h"

// timeGetTime周りの使用
#pragma comment(lib, "winmm.lib")

//--- プロトタイプ宣言
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//--- グローバル変数
// アプリケーション終了フラグ
bool g_bEnd = false;
// ウィンドウハンドル
HWND myHWnd;
// FPS値
DWORD myFPS = 0;
// 1フレーム内でホイールが動いたか
static int g_MouseWheelDelta = 0;

/****************************************//*
	@brief		|エントリーポイント
	@param		|hInstance：インスタンスハンドル
	@param		|hPrevInstance：前のインスタンスハンドル
	@param		|lpCmdLine：コマンドライン
	@param		|nCmdShow：ウィンドウの表示方法
	@return		| 成功したら0
*//****************************************/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // メモリリーク検出
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(235);

	//--- 変数宣言
	WNDCLASSEX wcex;
	MSG message;

	// ウィンドクラス情報の設定
	ZeroMemory(&wcex, sizeof(wcex));
	wcex.hInstance = hInstance;
	wcex.lpszClassName = "Class Name";
	wcex.lpfnWndProc = WndProc;
	wcex.style = CS_CLASSDC | CS_DBLCLKS;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hIconSm = wcex.hIcon;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

	// ウィンドウクラス情報の登録
	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL, "Failed to RegisterClassEx", "Error", MB_OK);
		return 0;
	}

	// ウィンドウの作成
	RECT rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	DWORD style = WS_CAPTION | WS_SYSMENU;
	DWORD exStyle = WS_EX_OVERLAPPEDWINDOW;
	AdjustWindowRectEx(&rect, style, false, exStyle);
	myHWnd = CreateWindowEx(
		exStyle, wcex.lpszClassName,
		NULL, style,
		CW_USEDEFAULT, CW_USEDEFAULT,
		rect.right - rect.left, rect.bottom - rect.top,
		HWND_DESKTOP,
		NULL, hInstance, NULL
	);

	// ウィンドウの表示
	ShowWindow(myHWnd, nCmdShow);
	UpdateWindow(myHWnd);
	SetWindowText(myHWnd, APP_TITLE);

	// 初期化処理
	if (FAILED(Init(myHWnd, SCREEN_WIDTH, SCREEN_HEIGHT)))
	{
		Uninit();
		UnregisterClass(wcex.lpszClassName, hInstance);
		return 0;
	}

	//--- FPS制御
	timeBeginPeriod(1);
	DWORD countStartTime = timeGetTime();
	DWORD preExecTime = countStartTime;
	DWORD time = timeGetTime();	//現在の処理時間
	DWORD oldTime = time;		//以前に実行した時間
	DWORD fpsTime = time;		//fpsの計測し始め
	DWORD fpsCount = 0;			//FPS値計測カウンタ

	//--- ウィンドウの管理
	// フレーム時間（ミリ秒）
	const DWORD frameDuration = 1000 / FPS;
	const DWORD spinWindow = 2; // 最後に短時間スピンするウィンドウ（ms）

	while (!g_bEnd)
	{
		// 1) 保留中のメッセージをすべて処理
		while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			if (message.message == WM_QUIT) { g_bEnd = true; break; }
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		if (g_bEnd) break;

		// 2) 現在時刻は1回だけ取得
		DWORD now = timeGetTime();
		DWORD elapsed = now - oldTime;
		if (elapsed < frameDuration)
		{
			DWORD remaining = frameDuration - elapsed;
			if (remaining > spinWindow + 1)
			{
				// 長めの待ち：他スレッドに譲るが短すぎない
				Sleep(remaining - spinWindow);
			}
			else
			{
				// 非ブロッキングで軽く譲る（ほかのスレッドへすぐ戻す）
				Sleep(0); // または SwitchToThread();
			}

			// 最後に、残りのごく短い時間をスピン（高応答性確保）
			while ((timeGetTime() - oldTime) < frameDuration) { /* tight wait */ }
		}

		// 3) フレーム更新/描画
		now = timeGetTime();
		if (now - oldTime >= frameDuration)
		{
			Update();
			Draw();
			oldTime = now;

			// FPS カウント等（既存ロジックを使う）
			fpsCount++;
			if (now - fpsTime >= 1000) { myFPS = fpsCount; fpsCount = 0; fpsTime = now; }
		}
	}


	// 終了時
	timeEndPeriod(1);
	Uninit();
	UnregisterClass(wcex.lpszClassName, hInstance);

	return 0;
}

/****************************************//*
	@brief		| ウインドウプロシージャ
	@param		| hWnd：ウィンドウハンドル
	@param		| message：メッセージ
	@param		| wParam：メッセージの追加情報1
	@param		| lParam：メッセージの追加情報2
	@return		| 処理結果
*//****************************************/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;  // ImGui が入力を処理した場合は OS 側の処理をスキップ

	switch (message)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
	}
	break;
	case WM_MOUSEWHEEL:
	{
		int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		g_MouseWheelDelta += delta;
	}
	break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

/****************************************//*
	@brief 	|アプリケーション終了処理
*//****************************************/
void AppEnd()
{
	g_bEnd = true;
}

/****************************************//*
	@brief		| ウインドウハンドル取得
	@return		| ウインドウハンドル
*//****************************************/
HWND GetMyWindow()
{
	return myHWnd;
}

/****************************************//*
	@brief		| フレームレート取得
	@return		| フレームレート
*//****************************************/
int GetFPS()
{
	return myFPS;
}

/****************************************//*
	@brief		| マウスホイールの回転量取得
	@return		| 回転量
*//****************************************/
int GetMouseWheelDelta()
{
	return g_MouseWheelDelta;
}

/****************************************//*
	@brief		| リセットマウスホイールの回転量
*//****************************************/
void ResetMouseWheelDelta()
{
	g_MouseWheelDelta = 0;
}