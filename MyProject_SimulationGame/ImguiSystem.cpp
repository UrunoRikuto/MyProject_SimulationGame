/**************************************************//*
	@file	| ImguiSystem.cpp
	@brief	| Imguiシステムクラスのcppファイル
	@note	| Imguiの初期化、更新、描画等を行う
			| シングルトンパターンで作成
*//**************************************************/
#include "ImguiSystem.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "Main.h"
#include "Camera.h"
#include "Human.h"
#include "BuildManager.h"
#include "GameTimeManager.h"
#include "CivLevelManager.h"
#include "GeneratorManager.h"
#include "StorageHouse.h"
#include "Defines.h"

#include <algorithm>
#include <cstdio>

//-- 静的メンバ変数の初期化 --//
CImguiSystem* CImguiSystem::m_pInstance = nullptr;
constexpr float ce_fCharaSize = 30.0f;

/****************************************//*
	@brief　	| コンストラクタ
*//****************************************/
CImguiSystem::CImguiSystem()
	: m_pGameObject(nullptr)
	, m_pHumanObject(nullptr)
	, m_pBuildObject(nullptr)
	, m_bUpdate(true)
	, m_bDebug{ false }
	, m_bCellsDraw(false)
	, m_bOnlyHuman(false)
{
	// ジェネレーター情報の初期化
	m_pGenerator.push_back({ "Human" , new(std::nothrow) CHumanGenerator() });
	m_pGenerator.push_back({ "Wood"  , new(std::nothrow) CWoodGenerator() });
	m_pGenerator.push_back({ "Stone" , new(std::nothrow) CStoneGenerator() });
}

/****************************************//*
	@brief　	| デストラクタ
*//****************************************/
CImguiSystem::~CImguiSystem()
{

}

/****************************************//*
	@brief　	| インスタンスの取得
	@return		| CImguiSystem型のインスタンスのポインタ
	@note		| インスタンスが生成されていなければ生成し、インスタンスを返す
*//****************************************/
CImguiSystem* CImguiSystem::GetInstance()
{
	// インスタンスが生成されていなければ生成する
	if (m_pInstance == nullptr)
	{
		m_pInstance = new(std::nothrow) CImguiSystem();
	}
	// インスタンスを返す
	return m_pInstance;
}

/****************************************//*
	@brief　	| インスタンスの解放
*//****************************************/
void CImguiSystem::ReleaseInstance()
{
	// インスタンスが生成されていれば解放する
	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

/****************************************//*
	@brief　	| 初期化処理
*//****************************************/
void CImguiSystem::Init()
{
	// 選択しているゲームオブジェクトの初期化
	m_pGameObject = nullptr;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	ImGui_ImplWin32_Init(GetMyWindow());
	ImGui_ImplDX11_Init(GetDevice(), GetContext());

	// ImGuiスタイルの設定
	ImGui::StyleColorsDark();

	// フォントの設定
	m_pReleaseFont = io.Fonts->AddFontFromFileTTF(FONT_PATH("Roboto-VariableFont_wdth,wght.ttf"), 30.0f);
	m_pDebugFont   = io.Fonts->AddFontFromFileTTF(FONT_PATH("Roboto-VariableFont_wdth,wght.ttf"), 15.0f);

#ifdef _DEBUG
	// サイズ調整不可
	io.ConfigWindowsResizeFromEdges = true;
	// ウィンドウ移動不可
	io.ConfigWindowsMoveFromTitleBarOnly = false;
#else
	// サイズ調整不可
	io.ConfigWindowsResizeFromEdges = false;
	// ウィンドウ移動不可
	io.ConfigWindowsMoveFromTitleBarOnly = true;
#endif // _DEBUG

	// 表示する文字以外を非表示にする
	io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
}

/****************************************//*
	@brief　	| 終了処理
*//****************************************/
void CImguiSystem::Uninit()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

/****************************************//*
	@brief　	| 更新処理
*//****************************************/
void CImguiSystem::Update()
{
	// 更新処理を止めている場合カメラの更新を行う
	if(!m_bUpdate)CCamera::GetInstance()->Update();
}

/****************************************//*
	@brief　	| 描画処理
*//****************************************/
void CImguiSystem::Draw()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	
	// ウィンドウ背景色の設定
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	// ウィンドウのタイトルバーを非表示
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	// フレーム背景色の設定
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	// フレーム境界線色の設定
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

	// 文明レベル表示
	Release_DrawCivLevel();
	// ゲーム内時間表示
	Release_DrawGameTime();
	// 人間の職業設定表示
	Release_DrawHuman();
	// 倉庫の資源表示
	Release_DrawBuildObject();

	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();

#ifdef _DEBUG

	// ウィンドウ背景色の設定
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
	// ウィンドウのタイトルバーを表示
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
	// フレーム背景色の設定
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
	// フレーム境界線色の設定
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

	// デバッグ用チェックボックス表示
	DrawDebugSystem();

	// カメラのパラメータ表示
	if (m_bDebug[static_cast<int>(DebugSystemFlag::CameraParam)])	DrawCameraParam();
	// インスペクター表示
	if (m_bDebug[static_cast<int>(DebugSystemFlag::Inspecter)])		DrawInspecter();
	// 建築依頼リスト
	if (m_bDebug[static_cast<int>(DebugSystemFlag::BuildRequestList)])	DrawBuildRequestList();
	// 生成依頼リスト
	if (m_bDebug[static_cast<int>(DebugSystemFlag::GenerateRequestList)])	DrawGenerateRequestList();
	// 更新を止めるチェックボックス表示
	if (m_bDebug[static_cast<int>(DebugSystemFlag::Update)])	DrawUpdateTick();
	// フレームレート表示
	if (m_bDebug[static_cast<int>(DebugSystemFlag::FPS)])		DrawFPS();
	// 全オブジェクト数表示
	if (m_bDebug[static_cast<int>(DebugSystemFlag::AllObjectNum)])	DrawAllObjectNum();
	// デバックログ表示
	if (m_bDebug[static_cast<int>(DebugSystemFlag::Log)])		DrawDebugLog();
	// 倉庫にアイテムを収納する
	if (m_bDebug[static_cast<int>(DebugSystemFlag::AddStoragehouseItem)])	DrawStoragehouseItem();

	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();

#endif // _DEBUG

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

/*＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝*/
//	Debugビルド時の例レイアウト関数			//
/*＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝*/

/****************************************//*
	@brief　	| デバックログの登録
	@param　	| log：登録するログ文字列
	@param　	| clear：true:描画後に削除 false:常に残り続ける
*//****************************************/
void CImguiSystem::AddDebugLog(const std::string& log, bool clear)
{
	DebugLogInfo info;
	info.m_sLog = log;
	info.m_bClear = clear;
	m_DebugLog.push_back(info);
}

/****************************************//*
	@brief　	| カメラのパラメータ表示
*//****************************************/
void CImguiSystem::DrawCameraParam()
{
	CCamera* pCamera = CCamera::GetInstance();
	CScene* pScene = GetScene();

	if (!pCamera) return;

	// フォントの設定
	ImGui::PushFont(m_pDebugFont);

	ImGui::Begin("Camera");
	ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(250,200), ImGuiWindowFlags_NoTitleBar);

	if (ImGui::Button("Reset"))
	{
		CCamera::GetInstance()->Init();
	}

	if (ImGui::CollapsingHeader(std::string("[Transform]").c_str()))
	{
		ImGui::Text("Position");
		DirectX::XMFLOAT3 pos = pCamera->GetPos();
		ImGui::Text("PosX: %.3f", pos.x);
		ImGui::Text("PosY: %.3f", pos.y);
		ImGui::Text("PosZ: %.3f", pos.z);
		ImGui::Text("\n");

		ImGui::Text("Look");
		DirectX::XMFLOAT3 look = pCamera->GetLook();
		ImGui::Text("LookX: %.3f", look.x);
		ImGui::Text("LookY: %.3f", look.y);
		ImGui::Text("LookZ: %.3f", look.z);
		ImGui::Text("\n");

		ImGui::Text("UpVector");
		DirectX::XMFLOAT3 up = pCamera->GetUp();
		ImGui::Text("UpX: %.3f", up.x);
		ImGui::Text("UpY: %.3f", up.y);
		ImGui::Text("UpZ: %.3f", up.z);
	}


	ImGui::EndChild();
	ImGui::End();
	ImGui::PopFont();
}

/****************************************//*
	@brief　	| インスペクター表示
*//****************************************/
void CImguiSystem::DrawInspecter()
{
	// フォントの設定
	ImGui::PushFont(m_pDebugFont);

	// 選択しているゲームオブジェクトが存在する場合
	if (m_pGameObject)
	{
		// インスペクター表示処理を呼び出す
		m_pGameObject->Inspecter();
	}
	// 選択しているゲームオブジェクトが存在しない場合
	else
	{
		ImGui::Begin("Inspecter");

		/**** 名前表示 ****/
		ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(250, 30), ImGuiWindowFlags_NoTitleBar);

		// 名前の表示
		std::string name = "No Selected Object";
		ImGui::Text(name.c_str());
		// 子要素の終了
		ImGui::EndChild();
		/***** 名前表示 *****/

		ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(250, 500), ImGuiWindowFlags_NoTitleBar);

		ImGui::EndChild();
		ImGui::End();
	}

	ImGui::PopFont();
}

/****************************************//*
	@brief　	| デバッグ用チェックボックス表示
*//****************************************/
void CImguiSystem::DrawDebugSystem()
{
	// フォントの設定
	ImGui::PushFont(m_pDebugFont);

	ImGui::Begin("DebugSystem");

	ImGui::Checkbox("CameraParam", &m_bDebug[static_cast<int>(DebugSystemFlag::CameraParam)]);
	ImGui::Checkbox("Inspecter", &m_bDebug[static_cast<int>(DebugSystemFlag::Inspecter)]);
	ImGui::Checkbox("BuildRequestList", &m_bDebug[static_cast<int>(DebugSystemFlag::BuildRequestList)]);
	ImGui::Checkbox("GenerateRequestList", &m_bDebug[static_cast<int>(DebugSystemFlag::GenerateRequestList)]);
	ImGui::Checkbox("Update",		&m_bDebug[static_cast<int>(DebugSystemFlag::Update)]);
	ImGui::Checkbox("FPS",			&m_bDebug[static_cast<int>(DebugSystemFlag::FPS)]);
	ImGui::Checkbox("AllObjectNum", &m_bDebug[static_cast<int>(DebugSystemFlag::AllObjectNum)]);
	ImGui::Checkbox("Log",			&m_bDebug[static_cast<int>(DebugSystemFlag::Log)]);
	ImGui::Checkbox("AddStoragehouseItem", &m_bDebug[static_cast<int>(DebugSystemFlag::AddStoragehouseItem)]);

	ImGui::End();
	ImGui::PopFont();
}

/****************************************//*
	@brief　	| 更新を止めるチェックボックス表示
*//****************************************/
void CImguiSystem::DrawUpdateTick()
{
	// フォントの設定
	ImGui::PushFont(m_pDebugFont);

	ImGui::Begin("UpdateTick");

	ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(ce_f2InspecterSize), ImGuiWindowFlags_NoTitleBar);
	ImGui::Checkbox("Use Update", &m_bUpdate);
	ImGui::EndChild();

	if (!m_bUpdate)
	{
		ImGui::BeginChild(ImGui::GetID((void*)1), ImVec2(ce_f2InspecterSize), ImGuiWindowFlags_NoTitleBar);
		if (ImGui::Button("Step"))
		{
			GetScene()->Update();
		}
		ImGui::EndChild();
	}

	ImGui::End();
	ImGui::PopFont();
}

/****************************************//*
	@brief　	| フレームレート表示
*//****************************************/
void CImguiSystem::DrawFPS()
{
	// フォントの設定
	ImGui::PushFont(m_pDebugFont);

	ImGui::Begin("FPS");
	ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(120.0f,30.0f), ImGuiWindowFlags_NoTitleBar);

	int fps = GetFPS();
	ImGui::Text("FPS: %d", fps);

	ImGui::EndChild();
	ImGui::End();
	ImGui::PopFont();
}

/****************************************//*
	@brief　	| 全オブジェクト数表示
*//****************************************/
void CImguiSystem::DrawAllObjectNum()
{
	// フォントの設定
	ImGui::PushFont(m_pDebugFont);

	ImGui::Begin("AllObjectNum");

	// シーンのインスタンス取得
	CScene* pScene = GetScene();

	if (pScene == nullptr)return;

	// 全オブジェクト数の取得
	int nObjectNum = pScene->GetIDVec().size();

	ImGui::Text("All Object Num: %d", nObjectNum);

	ImGui::End();
	ImGui::PopFont();
}

/****************************************//*
	@brief　	| デバッグログ表示
*//****************************************/
void CImguiSystem::DrawDebugLog()
{
	// フォントの設定
	ImGui::PushFont(m_pDebugFont);

	ImGui::Begin("DebugLog");
	ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(380,120), ImGuiWindowFlags_NoTitleBar);
	for (const auto& log : m_DebugLog)
	{
		ImGui::Text("%s", log.m_sLog.c_str());
	}
	ImGui::EndChild();
	ImGui::End();
	// remove cleared logs efficiently
	m_DebugLog.erase(
		std::remove_if(
			m_DebugLog.begin(),
			m_DebugLog.end(),
			[](const DebugLogInfo& log) { return log.m_bClear; }
		),
		m_DebugLog.end()
	);
	ImGui::PopFont();
}

/****************************************//*
	@brief　	| 倉庫の資源表示
*//****************************************/
void CImguiSystem::DrawStoragehouseItem()
{
	// フォントの設定
	ImGui::PushFont(m_pDebugFont);
	ImGui::Begin("AddStoragehouseItem");
	CStorageHouse* pStorageHouse = GetScene()->GetGameObject<CStorageHouse>();

	if (pStorageHouse)
	{
		if (ImGui::Button("Add Wood"))
		{
			CItem* pItem = new CItem(CItem::ITEM_TYPE::Wood);
			pStorageHouse->StoreItem(pItem);
		}
		if (ImGui::Button("Add Stone"))
		{
			CItem* pItem = new CItem(CItem::ITEM_TYPE::Stone);
			pStorageHouse->StoreItem(pItem);
		}
		if (ImGui::Button("Add Iron"))
		{
			CItem* pItem = new CItem(CItem::ITEM_TYPE::Iron);
			pStorageHouse->StoreItem(pItem);
		}

	}
	ImGui::End();
	ImGui::PopFont();

}

/****************************************//*
	@brief　	| 建築依頼リスト
*//****************************************/
void CImguiSystem::DrawBuildRequestList()
{
	// フォントの設定
	ImGui::PushFont(m_pDebugFont);

	ImGui::Begin("BuildRequestList");

	CBuildManager* pBuildManager = CBuildManager::GetInstance();

	for (auto& request : pBuildManager->GetBuildRequestList())
	{
		ImGui::BeginChild(ImGui::GetID((void*)&request), ImVec2(280,80), ImGuiWindowFlags_NoTitleBar);
		const char* RequestType = "";
		switch (request.eRequestType)
		{
		case CBuildManager::RequestType::Build:
			RequestType = "Build";
			break;
		case CBuildManager::RequestType::Upgrade:
			RequestType = "Upgrade";
			break;
		case CBuildManager::RequestType::Demolition:
			RequestType = "Demolition";
			break;
		}

		const char* BuildType = "";
		switch (request.eBuildType)
		{
		case CBuildManager::BuildType::RefreshFacility:
			BuildType = "RefreshFacility";
			break;
		}

		const char* RequestState = "";
		switch (request.eRequestState)
		{
		case CBuildManager::RequestState::Unprocessed:
			RequestState = "Unprocessed";
			break;
		case CBuildManager::RequestState::InProcess:
			RequestState = "InProcess";
			break;
		}

		ImGui::Text("Type: %s", RequestType);
		ImGui::Text("BuildType: %s", BuildType);
		ImGui::Text("State: %s", RequestState);

		ImGui::EndChild();
	}

	ImGui::End();
	ImGui::PopFont();
}

/****************************************//*
	@brief　	| 生成依頼リスト
*//****************************************/
void CImguiSystem::DrawGenerateRequestList()
{
	// フォントの設定
	ImGui::PushFont(m_pDebugFont);

	ImGui::Begin("GenerateRequestList");
	
	// 生成マネージャーのインスタンス取得
	CGeneratorManager* pGeneratorManager = CGeneratorManager::GetInstance();

	for(auto& request : pGeneratorManager->GetGenerateRequestList())
	{
		ImGui::BeginChild(ImGui::GetID((void*)&request), ImVec2(280, 80), ImGuiWindowFlags_NoTitleBar);

		std::string strType = "Type:";
		switch (request.m_GenerateType)
		{
		case CGeneratorManager::GenerateType::Wood:
			strType += "Wood";
			break;
		case CGeneratorManager::GenerateType::Stone:
			strType += "Stone";
			break;
		case CGeneratorManager::GenerateType::Human:
			strType += "Human";
			break;
		}
		ImGui::Text(strType.c_str());

		ImGui::Text("Timer: %.2f", request.m_fGenerateTime);

		ImGui::EndChild();
	}


	ImGui::End();
	ImGui::PopFont();
}

/*＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝*/
//	Releaseビルド時の例レイアウト関数		//
/*＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝*/

/****************************************//*
	@brief　	| 文明レベル表示
*//****************************************/
void CImguiSystem::Release_DrawCivLevel()
{
	// ウィンドウの位置設定
	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
	// サイズ変更ハンドルを非表示に設定
	ImGui::SetNextWindowSizeConstraints(ImVec2(250, 100), ImVec2(250, 100));
	// フォントの設定
	ImGui::PushFont(m_pReleaseFont);
	// ウィンドウの開始
	ImGui::Begin("Civ Level", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
	// インスタンスの取得
	CCivLevelManager* pCivLevelManager = CCivLevelManager::GetInstance();
	// 文明レベルの表示
	std::string sLevel = std::to_string(pCivLevelManager->GetCivLevel());
	ImGui::TextColored(
		ImVec4(0.5f, 1.0f, 0.0f, 1.0f),
		std::string("Civ Level: " + sLevel).c_str()
	);
	// 現在経験値を取得
	std::string sExp = std::to_string(static_cast<int>(pCivLevelManager->GetExp()));
	// 必要経験値を取得
	std::string sExpThreshold = std::to_string(static_cast<int>(pCivLevelManager->GetExpThreshold()));
	// 経験値の表示
	ImGui::Text(std::string("Exp: " + sExp + " / " + sExpThreshold).c_str());
	ImGui::End();
	ImGui::PopFont();
}

/****************************************//*
	@brief　	| ゲーム内時間表示
*//****************************************/
void CImguiSystem::Release_DrawGameTime()
{
	// ウィンドウの位置設定
	ImGui::SetNextWindowPos(ImVec2(0.0f, 100.0f));
	// サイズ変更ハンドルを非表示に設定
	ImGui::SetNextWindowSizeConstraints(ImVec2(230, 300), ImVec2(230, 300));
	// フォントの設定
	ImGui::PushFont(m_pReleaseFont);
	// ウィンドウの開始
	ImGui::Begin("Game Time", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

	// インスタンスの取得
	CGameTimeManager* pGameTimeManager = CGameTimeManager::GetInstance();

	// ゲーム内日数の表示
	int day = pGameTimeManager->GetGameDays();
	// ゲーム内時間の表示
	float time = pGameTimeManager->GetGameTime();

	// ゲーム内時間と日数の表示
	ImGui::Text("Game Time");

	// 区切り線の表示
	ImGui::Separator();

	// 現在の日数の表示
	ImGui::Text(std::string("Day:" + std::to_string(day)).c_str());

	// 現在の時間の表示
	ImGui::Text("Time: %.2f", time);

	// 現在の時間帯の取得
	CGameTimeManager::DAY_TIME dayTime = pGameTimeManager->GetCurrentDayTime();

	// 時間帯によって色を変更して表示
	ImVec4 color;
	switch (dayTime)
	{
	case CGameTimeManager::DAY_TIME::MORNING:
		color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // 黄色
		break;
	case CGameTimeManager::DAY_TIME::NOON:
		color = ImVec4(1.0f, 0.65f, 0.0f, 1.0f); // オレンジ
		break;
	case CGameTimeManager::DAY_TIME::EVENING:
		color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // 赤
		break;
	case CGameTimeManager::DAY_TIME::NIGHT:
		color = ImVec4(0.0f, 0.0f, 1.0f, 1.0f); // 青
	}

	// 現在の時間帯を文字列で取得
	std::string currendDayTime = pGameTimeManager->GetCurrentDayTimeString();

	std::string dayTimeFormat = std::string("[" + currendDayTime + "]");

	// 区切り線の表示
	ImGui::Separator();

	// 色付きで表示
	ImGui::TextColored(color, dayTimeFormat.c_str());

	// ウィンドウの終了
	ImGui::End();
	ImGui::PopFont();
}

/****************************************//*
	@brief　	| 人間の職業設定表示
*//****************************************/
void CImguiSystem::Release_DrawHuman()
{
	// ウィンドウの位置設定
	ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH - 400, 0.0f));

	// サイズ変更ハンドルを非表示に設定
	ImGui::SetNextWindowSizeConstraints(ImVec2(400, 700), ImVec2(400, 700));
	// フォントの設定
	ImGui::PushFont(m_pReleaseFont);

	// ウィンドウの開始
	ImGui::Begin("Human Info", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

	// 現在のシーンを取得
	CScene* pScene = GetScene();

	// 人間オブジェクトの取得
	auto Objects = pScene->GetGameObjects<CHuman>();
	// vector型に変換
	std::vector<CHuman*> Humans(Objects.begin(), Objects.end());

	if (m_pHumanObject)
	{
		// オブジェクトIDの取得
		ObjectID id = m_pHumanObject->GetID();
		// 表示名の作成
		std::string name = std::string("[Name]:" + id.m_sName).c_str();
		// 同名オブジェクトの区別のために番号を付与
		name += std::to_string(id.m_nSameCount + 1);
		// 職業名を表示名に追加
		ImGui::Text(name.c_str());
	}
	else
	{
		ImGui::Text("No Selected");
	}

	// コンボボックスの表示
	static int currentHumanIndex = 0;
	if (ImGui::BeginCombo("##SelectHuman", Humans.empty() ? "No Humans" : "HumanSelect"))
	{
		// 人間オブジェクトの表示
		for (size_t n = 0; n < Humans.size(); n++)
		{
			// 選択中かどうかの判定
			const bool isSelected = (currentHumanIndex == (int)n);
			// オブジェクトIDの取得
			ObjectID id = Humans[n]->GetID();
			// 表示名の作成
			std::string name = id.m_sName + std::to_string(id.m_nSameCount + 1);
			// 職業名の取得
			std::string job = Humans[n]->GetHumanJob()->GetJobName();
			// 職業名を表示名に追加
			name += " [" + job + "]";

			// 選択肢の表示
			if (ImGui::Selectable(name.c_str(), isSelected))
			{
				// 選択された場合、インデックスと選択中のオブジェクトを更新
				currentHumanIndex = static_cast<int>(n);
				// 選択中の人間オブジェクトを更新
				m_pHumanObject = Humans[n];
				m_pGameObject = m_pHumanObject;
			}
			// 選択中の項目にフォーカスを設定
			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	// 現在注視しているオブジェクトが人間かどうかの判定
	CHuman* pFocusHuman = dynamic_cast<CHuman*>(m_pGameObject);

	// 選択ボタンのラベル作成
	std::string SelectButtonLabel = "Camera:";
	SelectButtonLabel += (pFocusHuman) ? "Focus" : "NoFocus";

	// 選択ボタンの表示
	if (ImGui::Button(SelectButtonLabel.c_str()))
	{
		// 現在注視しているオブジェクトが人間であればフリーモードに変更、そうでなければ選択した人間オブジェクトに注視させる
		if (pFocusHuman)
		{
			// カメラを選択したオブジェクトに注視させる
			m_pGameObject = nullptr;
		}
		else
		{
			// カメラを選択したオブジェクトに注視させる
			m_pGameObject = m_pHumanObject;
		}
	}

	if (m_pHumanObject == nullptr)
	{
		ImGui::End();
		ImGui::PopFont();
		return;
	}

	// 職業名の取得
	std::string job = m_pHumanObject->GetHumanJob()->GetJobName();

	// 現在の職業を文字列で取得
	std::string currentJob = m_pHumanObject->GetHumanJob()->GetJobName();

	// 職業名リストの取得
	const std::vector<std::string> JobNames = CCivLevelManager::GetInstance()->GetUnlockJobNames();

	// Combo 用（string → const char* の配列に変換）
	std::vector<const char*> items;
	for (auto& s : JobNames) items.push_back(s.c_str());

	// 現在のインデックス
	int currentJobIndex = static_cast<int>(std::distance(
		JobNames.begin(),
		std::find(JobNames.begin(), JobNames.end(), currentJob)
	));

	// 区切り線の表示
	ImGui::Separator();
	ImGui::Text("[Status]");

	// スタミナ値の表示
	IJob_Strategy::JobStatus currentJobStatus = m_pHumanObject->GetHumanJob()->GetJobStatus();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Stamina: %.1f / %.1f", currentJobStatus.m_fStamina, currentJobStatus.m_fMaxStamina);


	// 空腹値の表示
	float currentHunger = m_pHumanObject->GetHunger();
	ImVec4 hungerTextColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f); // デフォルトは緑色
	if (currentHunger < Human_Warning_Hunger)
	{
		hungerTextColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // 警告閾値未満は黄色
	}

	ImGui::TextColored(hungerTextColor, "Hunger: %.1f / %.1f", currentHunger, Human_Max_Hunger);

	// 所持しているツールの表示
	CItem* pTool = m_pHumanObject->GetToolItem();
	std::string toolName = pTool ? CItem::ITEM_TYPE_TO_STRING(pTool->GetItemType()) : "None";
	ImGui::TextColored(ImVec4(0.5f, 0.5f, 1.0f, 1.0f), "Tool: %s", toolName.c_str());

	// 現在の職業名の表示
	ImGui::Text("[Job]:%s", items[currentJobIndex]);

	// コンボボックスの表示
	if(ImGui::BeginCombo("##SelectJob", currentJob.c_str()))
	{
		for (size_t n = 0; n < items.size(); n++)
		{
			// 選択中かどうかの判定
			const bool isSelected = (currentJobIndex == (int)n);
			// 選択肢の表示
			if (ImGui::Selectable(items[n], isSelected))
			{
				// 選択された場合、インデックスを更新
				currentJobIndex = static_cast<int>(n);
				// 新しい職業オブジェクトを作成して設定
				m_pHumanObject->SetHumanJob(CreateJobByName(JobNames[currentJobIndex], *m_pHumanObject));
			}
			// 選択中の項目にフォーカスを設定
			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	// 職業別のステータス表示
	m_pHumanObject->GetHumanJob()->DrawJobStatusImGui();

	// アイテムリストの表示
	ImGui::Separator();
	ImGui::Text("[Item List]");
	const auto& itemList = m_pHumanObject->GetItemList();

	ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(580, 300), ImGuiWindowFlags_NoTitleBar);

	// アイテムタイプ別にカウントして表示
	std::unordered_map<std::string, int> itemCountMap;
	for (const auto& item : itemList)
	{
		itemCountMap[CItem::ITEM_TYPE_TO_STRING(item->GetItemType())]++;
	}
	for (const auto& pair : itemCountMap)
	{
		ImGui::Text("%s: %d", pair.first.c_str(), pair.second);
	}

	ImGui::EndChild();

	// ウィンドウの終了
	ImGui::End();
	ImGui::PopFont();
}

/****************************************//*
	@brief　	| 建築物の表示
*//****************************************/
void CImguiSystem::Release_DrawBuildObject()
{
	// ウィンドウの位置設定(左下)
	ImGui::SetNextWindowPos(ImVec2(0.0f, SCREEN_HEIGHT - 400.0f));
	// サイズ変更ハンドルを非表示に設定
	ImGui::SetNextWindowSizeConstraints(ImVec2(400, 400), ImVec2(400, 400));
	// フォントの設定
	ImGui::PushFont(m_pReleaseFont);

	// ウィンドウの開始
	ImGui::Begin("BuildObject", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
	
	// コンボボックスで選択
	static int currentBuildObjectIndex = 0;
	// インスタンスの取得
	CScene* pScene = GetScene();
	// 建築物オブジェクトの取得
	auto buildObjects = pScene->GetGameObjects<CBuildObject>();
	// vector型に変換
	std::vector<CBuildObject*> buildObjectVec(buildObjects.begin(), buildObjects.end());

	if (ImGui::BeginCombo("##SelectBuild", buildObjects.empty() ? "No BuildObjects" : "BuildSelect"))
	{
		for (size_t n = 0; n < buildObjects.size(); n++)
		{
			const bool isSelected = (currentBuildObjectIndex == (int)n);
			ObjectID id = buildObjectVec[n]->GetID();
			std::string name = id.m_sName + std::to_string(id.m_nSameCount + 1);
			if (ImGui::Selectable(name.c_str(), isSelected))
			{
				currentBuildObjectIndex = static_cast<int>(n);
				m_pBuildObject = buildObjectVec[n];
#ifdef _DEBUG
				// 選択した建築物にカメラを注視させる
				m_pGameObject = m_pBuildObject;
#endif // _DEBUG
			}
			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

#ifdef _DEBUG

	// 現在注視しているオブジェクトが建築物かどうかの判定
	CBuildObject* pFocusBuildObject = dynamic_cast<CBuildObject*>(m_pGameObject);

	// 選択ボタンの表示
	std::string SelectButtonLabel = "Camera:";
	SelectButtonLabel += (pFocusBuildObject) ? "Focus" : "NoFocus";

	// 選択ボタンの表示
	if (ImGui::Button(SelectButtonLabel.c_str()))
	{
		if (pFocusBuildObject)
		{
			// カメラを選択したオブジェクトに注視させる
			m_pGameObject = nullptr;
		}
		else
		{
			// カメラを選択したオブジェクトに注視させる
			m_pGameObject = m_pBuildObject;
		}
	}
#endif // _DEBUG

	// 区切り線の表示
	ImGui::Separator();
	// 選択中の建築物の名前表示
	std::string selectedObjectName = "No Select BuildObject";
	// 選択されている場合
	if (m_pBuildObject != nullptr)
	{
		ObjectID id = m_pBuildObject->GetID();
		selectedObjectName = id.m_sName + std::to_string(id.m_nSameCount + 1);
	}

	ImGui::Text(selectedObjectName.c_str());

	ImGui::Separator();

	if (m_pBuildObject)m_pBuildObject->Inspecter();

	ImGui::End();
	ImGui::PopFont();
}
