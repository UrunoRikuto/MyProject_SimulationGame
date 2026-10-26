/**********************************************************************************//*
    @file		|Startup.cpp
    @brief		|起動時処理
*//***********************************************************************************/
#pragma warning(disable: 4819)
#include <windows.h>
#include "GoalManager.h"
#include "Main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <crtdbg.h>
#include "Defines.h"
#include "imgui_impl_win32.h"
#include "Profiler.h"

// timeGetTime周りの使用
#pragma comment(lib, "winmm.lib")

/****************************************//*
	@brief	| 高精度タイマーによる現在時刻(ミリ秒)の取得
	@return	| 現在時刻(ミリ秒、小数部あり)
	@note	| timeGetTimeは、整数のミリ秒でしか測れない上に、OSの省電力機能などでタイマーの分解能が粗く(約15.6ms)なると、
			| フレームの間隔が不安定になってカクつくため、フレームの時間管理には使用しない
*//****************************************/
static double HiResNowMs()
{
	static LARGE_INTEGER s_Frequency = []()
	{
		LARGE_INTEGER f;
		QueryPerformanceFrequency(&f);
		return f;
	}();

	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return static_cast<double>(counter.QuadPart) * 1000.0 / static_cast<double>(s_Frequency.QuadPart);
}

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
	// 起動引数による自動計測モード(例: --profile=60 --speed=8 --humans=20)
	{
		float fSeconds = 0.0f;
		int nSpeed = 1;
		int nHumans = 0;
		int nStartDay = -1;
		char jobKey[32] = "";
		if (lpCmdLine != nullptr)
		{
			const char* pArg = nullptr;
			if ((pArg = strstr(lpCmdLine, "--profile=")) != nullptr) fSeconds = static_cast<float>(atof(pArg + 10));
			if ((pArg = strstr(lpCmdLine, "--speed=")) != nullptr) nSpeed = atoi(pArg + 8);
			if ((pArg = strstr(lpCmdLine, "--humans=")) != nullptr) nHumans = atoi(pArg + 9);
			if ((pArg = strstr(lpCmdLine, "--day=")) != nullptr) nStartDay = atoi(pArg + 6);
			if ((pArg = strstr(lpCmdLine, "--job=")) != nullptr) sscanf_s(pArg + 6, "%31[a-z]", jobKey, (unsigned)sizeof(jobKey));
		}
		if (fSeconds > 0.0f) SetAutoProfile(fSeconds, nSpeed, nHumans, jobKey, nStartDay);

		// 自動計測中でも、目標の表示と結果の画面を出す(目標の画面の確認用。例: --profile=600 --day=65 --goalui)
		if (fSeconds > 0.0f && lpCmdLine != nullptr && strstr(lpCmdLine, "--goalui") != nullptr) CGoalManager::SetHeadless(false);
	}

	if (FAILED(Init(myHWnd, SCREEN_WIDTH, SCREEN_HEIGHT)))
	{
		Uninit();
		UnregisterClass(wcex.lpszClassName, hInstance);
		return 0;
	}

	//--- FPS制御
	timeBeginPeriod(1);

	// 省電力機能(EcoQoS)による、実行速度やタイマーの分解能の制限を受けないようにする
	{
		PROCESS_POWER_THROTTLING_STATE throttling = {};
		throttling.Version = PROCESS_POWER_THROTTLING_CURRENT_VERSION;
		throttling.ControlMask = PROCESS_POWER_THROTTLING_EXECUTION_SPEED;
#ifdef PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION
		throttling.ControlMask |= PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION;
#endif
		throttling.StateMask = 0;
		SetProcessInformation(GetCurrentProcess(), ProcessPowerThrottling, &throttling, sizeof(throttling));
	}

	// フレーム間の待ち時間用の、高分解能の待機タイマー(使えない環境ではnullptrになり、Sleepで待つ)
	HANDLE hFrameTimer = CreateWaitableTimerExW(nullptr, nullptr, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);

	//--- ウィンドウの管理
	// フレーム時間（ミリ秒）
	// (整数のミリ秒(16ms)にすると約62.5FPSになり、60Hzの画面とのずれで一定間隔ごとにコマ落ちするため、小数のまま扱う)
	const double frameDuration = 1000.0 / FPS;
	const double spinWindow = 2.0; // 最後に短時間スピンするウィンドウ（ms）

	// 次のフレームを始める時刻(毎回「今の時刻+フレーム時間」にせず、決まった間隔で進めることで、処理時間のばらつきが積み重ならないようにする)
	double nextFrameTime = HiResNowMs();
	DWORD fpsTime = static_cast<DWORD>(nextFrameTime);	//fpsの計測し始め
	DWORD fpsCount = 0;									//FPS値計測カウンタ

	while (!g_bEnd)
	{
		// 1) 保留中のメッセージをすべて処理
		{
			PROFILE_SCOPE("Loop: Messages");
			while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
			{
				if (message.message == WM_QUIT) { g_bEnd = true; break; }
				TranslateMessage(&message);
				DispatchMessage(&message);
			}
		}
		if (g_bEnd) break;

		// 2) 次のフレームの時刻まで待つ
		double now = HiResNowMs();
		if (now < nextFrameTime)
		{
			PROFILE_SCOPE("Loop: Wait");
			const double remaining = nextFrameTime - now;
			if (remaining > spinWindow + 1.0)
			{
				// 長めの待ち：他スレッドに譲るが短すぎない(最後のスピン分を除いた時間だけ待つ)
				const double waitMs = remaining - spinWindow;
				if (hFrameTimer != nullptr)
				{
					// 高分解能タイマーで待つ(Sleepはタイマーの分解能が粗いと、大きく待ち過ぎてカクつく)
					LARGE_INTEGER dueTime;
					dueTime.QuadPart = -static_cast<LONGLONG>(waitMs * 10000.0);
					if (SetWaitableTimer(hFrameTimer, &dueTime, 0, nullptr, nullptr, FALSE))
					{
						WaitForSingleObject(hFrameTimer, INFINITE);
					}
				}
				else
				{
					Sleep(static_cast<DWORD>(waitMs));
				}
			}
			else
			{
				// 非ブロッキングで軽く譲る（ほかのスレッドへすぐ戻す）
				Sleep(0); // または SwitchToThread();
			}

			// 最後に、残りのごく短い時間をスピン（高応答性確保）
			while (HiResNowMs() < nextFrameTime) { /* tight wait */ }
		}

		// 3) フレーム更新/描画
		Update();
		Draw();

		// 次のフレームの時刻を進める(大きく遅れた場合は、遅れを取り戻そうとせず立て直す)
		nextFrameTime += frameDuration;
		now = HiResNowMs();
		if (now - nextFrameTime > frameDuration * 2.0)
		{
			nextFrameTime = now;
		}

		// FPS カウント等
		fpsCount++;
		if (now - fpsTime >= 1000.0) { myFPS = fpsCount; fpsCount = 0; fpsTime = static_cast<DWORD>(now); }
	}


	// 終了時
	if (hFrameTimer != nullptr) CloseHandle(hFrameTimer);
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