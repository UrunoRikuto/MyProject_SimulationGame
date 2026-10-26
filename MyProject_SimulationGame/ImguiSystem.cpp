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
#include "Input.h"
#include "SeasonManager.h"
#include "TutorialManager.h"
#include "Human.h"
#include "Animal.h"
#include "BuildManager.h"
#include "GameTimeManager.h"
#include "CivLevelManager.h"
#include "GeneratorManager.h"
#include "StorageHouse.h"
#include "Defines.h"
#include "Enums.h"

#include <algorithm>
#include <cstdio>
#include <string>

//-- 静的メンバ変数の初期化 --//
CImguiSystem* CImguiSystem::m_pInstance = nullptr;
constexpr float ce_fCharaSize =30.0f;

// @brief 選択できるゲーム速度(倍速)
constexpr int ce_nGameSpeedSteps[] = { 1, 2, 4, 8 };
constexpr int ce_nGameSpeedStepCount = static_cast<int>(sizeof(ce_nGameSpeedSteps) / sizeof(ce_nGameSpeedSteps[0]));

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
	, m_nCellDisplayMode(0)
	, m_pDebugFont(nullptr)
	, m_pReleaseFont(nullptr)
	, m_bOnlyHuman(false)
	, m_nSeed(0)
	, m_bSettingSeed(false)
{
 m_seedInput.clear();
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

	ImGui::StyleColorsDark();

	// 日本語グリフ範囲を指定してフォント追加（NotoSansJP推奨）
	const ImWchar* jpRanges = io.Fonts->GetGlyphRangesJapanese();
	//文字サイズは据え置きでさらに太さを上げる設定
	ImFontConfig cfg;
	cfg.OversampleH =3; // 水平方向のオーバーサンプリング
	cfg.OversampleV =3; // 垂直方向のオーバーサンプリング
	cfg.PixelSnapH = true; // ピクセル境界にスナップ
	cfg.RasterizerMultiply =1.6f; //さらに濃くして太く見せる
	// フォントサイズは既存のまま
	m_pReleaseFont = io.Fonts->AddFontFromFileTTF(FONT_PATH("NotoSansJP-VariableFont_wght.ttf"),30.0f, &cfg, jpRanges);
	m_pDebugFont = io.Fonts->AddFontFromFileTTF(FONT_PATH("NotoSansJP-VariableFont_wght.ttf"),15.0f, &cfg, jpRanges);

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
	// 選択中のオブジェクトが破棄されている場合は、解放済みのポインタを触らないよう選択を解除する
	ValidateSelection();

	// チュートリアルの進行
	CTutorialManager::Update();

	// 更新処理を止めている場合カメラの更新を行う
	if(!m_bUpdate)CCamera::GetInstance()->Update();

	// ゲームシーンでは、数字キー1から4でゲーム速度(倍速)を切り替える(文字入力中は無効)
	if (dynamic_cast<CSceneGame*>(GetScene()) != nullptr && !ImGui::GetIO().WantTextInput)
	{
		for (int i = 0; i < ce_nGameSpeedStepCount; ++i)
		{
			if (IsKeyTrigger(static_cast<BYTE>('1' + i)))
			{
				SetGameSpeed(ce_nGameSpeedSteps[i]);
			}
		}
	}
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
	// ゲーム速度表示
	Release_DrawGameSpeed();
	// 人間の職業設定表示
	Release_DrawHuman();
	// 人間の一覧表示(職業ごとの人数の増減と、人間ごとの職業設定)
	Release_DrawHumanList();
	// 倉庫の資源表示
	Release_DrawBuildObject();

	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();

	// チュートリアル表示(他のウィンドウの上に重ねる)
	CTutorialManager::Draw(m_pReleaseFont);

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

	// ヒストリー表示とインスペクター表示
	if (m_bDebug[static_cast<int>(DebugSystemFlag::History＆Inspecter)]) { DrawHistory(); DrawInspecter(); }
	// カメラのパラメータ表示
	if (m_bDebug[static_cast<int>(DebugSystemFlag::CameraParam)])	DrawCameraParam();
	// 建築依頼リスト
	if (m_bDebug[static_cast<int>(DebugSystemFlag::BuildRequestList)])	DrawBuildRequestList();
	// 生成依頼リスト
	if (m_bDebug[static_cast<int>(DebugSystemFlag::GenerateRequestList)])	DrawGenerateRequestList();
	// 更新を止めるチェックボックス表示
	if (m_bDebug[static_cast<int>(DebugSystemFlag::Update)])	DrawUpdateTick();
	// フィールドセル表示
	if (m_bDebug[static_cast<int>(DebugSystemFlag::FieldCells)])	DrawFieldCells();
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
	@brief　	| シード値入力モードの表示
*//****************************************/
void CImguiSystem::DrawSeedInputMode()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// ウィンドウ背景色の設定
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f,0.1f,0.1f,1.0f));
	// フレーム背景色の設定
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f,0.0f,0.0f,0.0f));
	// フレーム境界線色の設定
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f,0.0f,0.0f,0.0f));

	ImGui::SetNextWindowPos(ImVec2((SCREEN_WIDTH /2) -300,600));
	ImGui::SetNextWindowSize(ImVec2(600,80));

	// フォントの設定
	ImGui::PushFont(m_pReleaseFont);
	ImGui::Begin("Seed Input Mode", nullptr,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar );
	ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(500,60), ImGuiWindowFlags_NoTitleBar);

	// シード値入力欄
	ImGui::Separator();
	ImGui::Text(u8"シード値を入力:");
	ImGui::SameLine();
	// use InputText with std::string via c_str-backed buffer - use InputText with flags to allow long input
	static char buf[512] = {0};
	if (!m_seedInput.empty()) {
		strncpy_s(buf, m_seedInput.c_str(), sizeof(buf)-1);
	}
	if (ImGui::InputText("##seed_input", buf, sizeof(buf))) {
		m_seedInput = std::string(buf);
	}
	ImGui::Separator();

	ImGui::EndChild();
	ImGui::End();
	ImGui::PopFont();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

/****************************************//*
	@brief　	| シード値設定の決定
*//****************************************/
void CImguiSystem::DecideSettingSeed()
{
	// 入力された文字がなかった場合
	if (m_seedInput.empty()) {
		m_nSeed =0u;
	} else {
		// try to hash string to unsigned int if it is not numeric or too long
		try {
			// attempt to parse as unsigned long long, then narrow
			unsigned long long val = std::stoull(m_seedInput);
			m_nSeed = static_cast<unsigned int>(val &0xFFFFFFFFu);
		} catch (...) {
			// fallback: use std::hash to convert arbitrary string to seed
			std::hash<std::string> hasher;
			size_t h = hasher(m_seedInput);
			m_nSeed = static_cast<unsigned int>(h &0xFFFFFFFFu);
		}
	}

	// シード値設定完了フラグを立てる
	m_bSettingSeed = true;
}

/****************************************//*
	@brief　	| 履歴表示
*//****************************************/
void CImguiSystem::DrawHistory()
{
	// フォントの設定
	ImGui::PushFont(m_pDebugFont);

	ImGui::Begin("Hierarchy");
	ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(250, 260), ImGuiWindowFlags_NoTitleBar);

	auto Objects = GetScene()->GetIDVec();

	std::list<ObjectID> objectIDList{};
	for (auto Id : Objects)
	{
		objectIDList.push_back(Id);
	}

	objectIDList.sort([](ObjectID a, ObjectID b)
		{
			return a.m_nSameCount < b.m_nSameCount;
		});

	objectIDList.sort([](ObjectID a, ObjectID b)
		{
			return a.m_sName < b.m_sName;
		});

	for (auto itr = objectIDList.begin(); itr != objectIDList.end();)
	{
		std::string name = itr->m_sName;

		int nItrCount = 0;
		for (auto idItr : objectIDList)
		{
			if (idItr.m_sName == name)
			{
				nItrCount++;
			}
		}
		ObjectID id;
		id.m_sName = name;

		// 折りたたみヘッダー
		std::string sHeaderName = ("[%s]",name);
		sHeaderName += std::to_string(nItrCount);

		if (ImGui::CollapsingHeader(sHeaderName.c_str()))
		{
			for (int i = 0; i < nItrCount; i++)
			{
				std::string sButtonName = name;
				if (i != 0) sButtonName += std::to_string(i);
				id.m_nSameCount = i;
				if (ImGui::Button(sButtonName.c_str()))
				{
					m_pGameObject = GetScene()->GetGameObject(id);
				}

			}

		}


		std::advance(itr, nItrCount);
	}

	ImGui::EndChild();
	ImGui::End();
	ImGui::PopFont();
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

	ImGui::Begin(u8"カメラ");
	ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(250,200), ImGuiWindowFlags_NoTitleBar);

	if (ImGui::Button(u8"リセット"))
	{
		CCamera::GetInstance()->Init();
	}

	if (ImGui::CollapsingHeader(std::string(u8"[トランスフォーム]").c_str()))
	{
		ImGui::Text(u8"位置");
		DirectX::XMFLOAT3 pos = pCamera->GetPos();
		ImGui::Text(u8"X座標: %.3f", pos.x);
		ImGui::Text(u8"Y座標: %.3f", pos.y);
		ImGui::Text(u8"Z座標: %.3f", pos.z);
		ImGui::Text("\n");

		ImGui::Text(u8"視点位置");
		DirectX::XMFLOAT3 look = pCamera->GetLook();
		ImGui::Text(u8"X座標: %.3f", look.x);
		ImGui::Text(u8"Y座標: %.3f", look.y);
		ImGui::Text(u8"Z座標: %.3f", look.z);
		ImGui::Text("\n");

		ImGui::Text(u8"上方向ベクトル");
		DirectX::XMFLOAT3 up = pCamera->GetUp();
		ImGui::Text(u8"X座標: %.3f", up.x);
		ImGui::Text(u8"Y座標: %.3f", up.y);
		ImGui::Text(u8"Z座標: %.3f", up.z);
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
		ImGui::Begin(u8"インスペクター");

		/**** 名前表示 ****/
		ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(250, 30), ImGuiWindowFlags_NoTitleBar);

		// 名前の表示
		std::string name = u8"選択しているオブジェクトがありません";
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

	ImGui::Begin(u8"デバックシステム");

	ImGui::Checkbox(u8"ヒストリー＆インスペクター", &m_bDebug[static_cast<int>(DebugSystemFlag::History＆Inspecter)]);
	ImGui::Checkbox(u8"カメラ情報", &m_bDebug[static_cast<int>(DebugSystemFlag::CameraParam)]);
	ImGui::Checkbox(u8"建築リクエスト", &m_bDebug[static_cast<int>(DebugSystemFlag::BuildRequestList)]);
	ImGui::Checkbox(u8"生成リクエスト", &m_bDebug[static_cast<int>(DebugSystemFlag::GenerateRequestList)]);
	ImGui::Checkbox(u8"更新判定",		&m_bDebug[static_cast<int>(DebugSystemFlag::Update)]);
	ImGui::Checkbox(u8"フィールドセル描画", &m_bDebug[static_cast<int>(DebugSystemFlag::FieldCells)]);
	ImGui::Checkbox(u8"FPS",			&m_bDebug[static_cast<int>(DebugSystemFlag::FPS)]);
	ImGui::Checkbox(u8"全オブジェクト数", &m_bDebug[static_cast<int>(DebugSystemFlag::AllObjectNum)]);
	ImGui::Checkbox(u8"ログ",			&m_bDebug[static_cast<int>(DebugSystemFlag::Log)]);
	ImGui::Checkbox(u8"倉庫にアイテムを追加", &m_bDebug[static_cast<int>(DebugSystemFlag::AddStoragehouseItem)]);

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
	ImGui::Checkbox(u8"更新処理を行う", &m_bUpdate);
	ImGui::EndChild();

	// 群れ(狼・鹿)の近隣情報を毎フレーム更新するか(オフにすると従来の動作で見比べられる)
	ImGui::Checkbox(u8"群れ情報を毎フレーム更新", &CAnimal::s_bRefreshFlockInfo);

	if (!m_bUpdate)
	{
		ImGui::BeginChild(ImGui::GetID((void*)1), ImVec2(ce_f2InspecterSize), ImGuiWindowFlags_NoTitleBar);
		if (ImGui::Button(u8"一フレーム進める"))
		{
			GetScene()->Update();
		}
		ImGui::EndChild();
	}

	ImGui::End();
	ImGui::PopFont();
}

/****************************************//*
	@brief　	| フィールドセル描画
*//****************************************/
void CImguiSystem::DrawFieldCells()
{
	// フォントの設定
	ImGui::PushFont(m_pDebugFont);
	ImGui::Begin("FieldCells");
	ImGui::Checkbox(u8"フィールドセル描画", &m_bCellsDraw);
	ImGui::Text(u8"中心位置");
	
	std::string ButtonLabel = (m_nCellDisplayMode == 0) ? u8"セルタイプ" : u8"縄張りタイプ";
	if (ImGui::Button(ButtonLabel.c_str()))
	{
		m_nCellDisplayMode ^= 1;
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

	ImGui::Text(u8"全オブジェクト数: %d", nObjectNum);

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

	ImGui::Begin(u8"デバックログ");
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
	@brief　	| 倉庫にアイテムを収納する
*//****************************************/
void CImguiSystem::DrawStoragehouseItem()
{
	// フォントの設定
	ImGui::PushFont(m_pDebugFont);
	ImGui::Begin(u8"倉庫に資源追加");
	CStorageHouse* pStorageHouse = GetScene()->GetGameObject<CStorageHouse>();

	if (pStorageHouse)
	{
		// 全アイテムタイプの追加ボタン作成
		for(int i = 0; i < static_cast<int>(CItem::ITEM_TYPE::MAX); ++i)
		{
			// アイテムタイプの取得
			CItem::ITEM_TYPE type = static_cast<CItem::ITEM_TYPE>(i);
			// アイテムタイプの名前取得
			std::string itemName = CItem::ITEM_TYPE_TO_STRING(type);

			if (ImGui::Button((u8"追加:" + itemName).c_str()))
			{
				CItem* pItem = new CItem(type);
				pStorageHouse->StoreItem(pItem);
			}
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

	ImGui::Begin(u8"建築リクエスト");

	CBuildManager* pBuildManager = CBuildManager::GetInstance();

	for (auto& request : pBuildManager->GetBuildRequestList())
	{
		ImGui::BeginChild(ImGui::GetID((void*)&request), ImVec2(280,80), ImGuiWindowFlags_NoTitleBar);
		const char* RequestType = "";
		switch (request.eRequestType)
		{
		case CBuildManager::RequestType::Build:
			RequestType = u8"建築";
			break;
		case CBuildManager::RequestType::Upgrade:
			RequestType = u8"強化";
			break;
		case CBuildManager::RequestType::Demolition:
			RequestType = u8"解体";
			break;
		}

		const char* BuildType = "";
		switch (request.eBuildType)
		{
		case CBuildManager::BuildType::RefreshFacility:
			BuildType = u8"休憩施設";
			break;
		case CBuildManager::BuildType::HumanHouse:
			BuildType = u8"人間の家";
			break;
		case CBuildManager::BuildType::BlackSmith:
			BuildType = u8"鍛冶屋";
			break;
		case CBuildManager::BuildType::FoodFactory:
			BuildType = u8"食料加工施設";
			break;
		case CBuildManager::BuildType::FarmFacility:
			BuildType = u8"農作施設";
			break;
		}

		const char* RequestState = "";
		switch (request.eRequestState)
		{
		case REQUEST_STATE::Unprocessed:
			RequestState = u8"未受注";
			break;
		case REQUEST_STATE::InProcess:
			RequestState = u8"受注済み";
			break;
		}

		ImGui::Text(u8"タイプ: %s", RequestType);
		ImGui::Text(u8"建築タイプ: %s", BuildType);
		ImGui::Text(u8"状態: %s", RequestState);

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

	ImGui::Begin(u8"生成リクエスト");
	
	// 生成マネージャーのインスタンス取得
	CGeneratorManager* pGeneratorManager = CGeneratorManager::GetInstance();

	// 件数が多い場合は先頭の一部だけ表示する
	constexpr int Max_Display_Count = 30;
	int nDisplayCount = 0;

	for(auto& request : pGeneratorManager->GetGenerateRequestList())
	{
		if (nDisplayCount >= Max_Display_Count)
		{
			ImGui::Text(u8"他 %d 件", static_cast<int>(pGeneratorManager->GetGenerateRequestList().size()) - nDisplayCount);
			break;
		}
		nDisplayCount++;

		ImGui::BeginChild(ImGui::GetID((void*)&request), ImVec2(280, 80), ImGuiWindowFlags_NoTitleBar);

		std::string strType = "タイプ:";
		switch (request.m_GenerateType)
		{
		case CGeneratorManager::GenerateType::Wood:
			strType += "木";
			break;
		case CGeneratorManager::GenerateType::Stone:
			strType += "石";
			break;
		case CGeneratorManager::GenerateType::Grass:
			strType += "草";
			break;
		case CGeneratorManager::GenerateType::Human:
			strType += "人間";
			break;
		}
		ImGui::Text(strType.c_str());

		ImGui::Text(u8"生成するまでの時間: %.2f", request.m_fGenerateTime);
		if (request.m_n2CellIndex.x >= 0)
		{
			ImGui::Text(u8"生成先セル: (%d, %d)", request.m_n2CellIndex.x, request.m_n2CellIndex.y);
		}

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
	ImGui::SetNextWindowPos(ImVec2(0.0f,0.0f));
	// サイズ変更ハンドルを非表示に設定
	ImGui::SetNextWindowSizeConstraints(ImVec2(250,100), ImVec2(250,100));
	// フォントの設定
	ImGui::PushFont(m_pReleaseFont);
	// ウィンドウの開始
	ImGui::Begin("Civ Level", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
	// インスタンスの取得
	CCivLevelManager* pCivLevelManager = CCivLevelManager::GetInstance();
#ifndef _DEBUG
	// 文明レベルの表示（視認性を高めるため白色）
	std::string sLevel = std::to_string(pCivLevelManager->GetCivLevel());
	ImGui::TextColored(
		ImVec4(1.0f,1.0f,1.0f,1.0f),
		u8"文明レベル: %s",
		sLevel.c_str()
	);
#else
	// 編集可能な文明レベルの表示
	int level = pCivLevelManager->GetCivLevel();
	ImGui::InputInt("Civ Level", &level);
	pCivLevelManager->SetCivLevel(level);

#endif // !_DEBUG

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
	ImGui::Begin(u8"ゲーム内時間", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

	// インスタンスの取得
	CGameTimeManager* pGameTimeManager = CGameTimeManager::GetInstance();

	// ゲーム内日数の表示
	int day = pGameTimeManager->GetGameDays();
	// ゲーム内時間の表示
	float time = pGameTimeManager->GetGameTime();

	// ゲーム内時間と日数の表示
	ImGui::Text(u8"ゲーム内時間");

	// 区切り線の表示
	ImGui::Separator();

	// 現在の日数の表示
	ImGui::Text(std::string(u8"経過日数:" + std::to_string(day)).c_str());

	// 現在の季節の表示
	ImGui::Text(u8"季節: %s (%d/%d日目)", CSeasonManager::GetSeasonName(), CSeasonManager::GetDayInSeason() + 1, CSeasonManager::Season_Length_Days);

	// 現在の時間の表示
	ImGui::Text(u8"現在時刻: %.2f", time);

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

	std::string dayTimeFormat = std::string(u8"[" + currendDayTime + "]");
#ifdef _DEBUG
	// 時刻を次にの時間帯に変更するボタンの表示
	if (ImGui::Button(u8"時間帯を進める"))
	{
		float newTime = 0.0f;
		switch (dayTime)
		{
		case CGameTimeManager::DAY_TIME::MORNING:
			newTime = 30.0f;
			break;
		case CGameTimeManager::DAY_TIME::NOON:
			newTime = 60.0f;
			break;
		case CGameTimeManager::DAY_TIME::EVENING:
			newTime = 90.0f;
			break;
		case CGameTimeManager::DAY_TIME::NIGHT:
			newTime = 0.0f;
			break;
		}
		pGameTimeManager->SetGameTime(newTime);
	}
	
#endif // _DEBUG


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
		std::string name = std::string(u8"[名前]:" + id.m_sName).c_str();
		// 同名オブジェクトの区別のために番号を付与
		name += std::to_string(id.m_nSameCount + 1);
		// 職業名を表示名に追加
		ImGui::Text(name.c_str());
	}
	else
	{
		ImGui::Text(u8"未設定");
	}

	// コンボボックスの表示
	static int currentHumanIndex = 0;
	if (ImGui::BeginCombo("##SelectHuman", Humans.empty() ? u8"人間が見つからない" : u8"人間選択"))
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
	std::string SelectButtonLabel = u8"カメラ:";
	SelectButtonLabel += (pFocusHuman) ? u8"注視をやめる" : u8"注視する";

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
	ImGui::Text(u8"[ステータス]");

	// 体力値の表示
	const float maxHealth = m_pHumanObject->GetMaxHealth();
	const float currentHealth = m_pHumanObject->GetHealth();
	std::string healthText = u8"体力: " + std::to_string(static_cast<int>(currentHealth)) + " / " + std::to_string(static_cast<int>(maxHealth));
	ImGui::ProgressBar(currentHealth / maxHealth, ImVec2(200.0f, 30.0f), healthText.c_str());

	// 空腹値の表示
	const float maxHunger = Max_Hunger;
	float currentHunger = m_pHumanObject->GetHunger();
	std::string hungerText = u8"空腹値: " + std::to_string(static_cast<int>(currentHunger)) + " / " + std::to_string(static_cast<int>(maxHunger));
	ImGui::ProgressBar(currentHunger / maxHunger, ImVec2(200.0f, 30.0f), hungerText.c_str());

	// スタミナ値の表示
	const float maxStamina = m_pHumanObject->GetMaxStamina();
	const float currentStamina = m_pHumanObject->GetStamina();
	std::string staminaText = u8"スタミナ: " + std::to_string(static_cast<int>(currentStamina)) + " / " + std::to_string(static_cast<int>(maxStamina));
	ImGui::ProgressBar(currentStamina / maxStamina, ImVec2(200.0f, 30.0f), staminaText.c_str());

	// 所持しているツールの表示
	CItem* pTool = m_pHumanObject->GetToolItem();
	std::string toolName = pTool ? CItem::ITEM_TYPE_TO_STRING(pTool->GetItemType()) : u8"なし";
	ImGui::TextColored(ImVec4(0.5f, 0.5f, 1.0f, 1.0f), u8"道具: %s", toolName.c_str());

	// 現在の職業名の表示
	ImGui::Text(u8"[職業]:%s", items[currentJobIndex]);

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

	// スキル表示
	m_pHumanObject->GetSkill()->DrawSkillImGui();

	// 職業別のステータス表示
	m_pHumanObject->GetHumanJob()->DrawJobStatusImGui();

	// アイテムリストの表示
	ImGui::Separator();
	ImGui::Text(u8"[所持アイテム]");
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
	@brief　	| 人間の一覧表示
	@note		| 職業ごとの人数の増減と、人間ごとの職業設定をまとめて行える(人間が増えても選びやすいようにするため)
*//****************************************/
void CImguiSystem::Release_DrawHumanList()
{
	// 一覧ウィンドウの大きさと、人間情報ウィンドウとの間隔
	const float List_Width = 440.0f;
	const float List_Max_Height = 560.0f;
	const float Table_Max_Height = 220.0f;
	const float Human_Info_Width = 400.0f;
	const float Margin = 10.0f;
	const float Top = 45.0f;

	ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH - Human_Info_Width - List_Width - Margin, Top));
	// 高さは内容(人数)に合わせて決まる
	ImGui::SetNextWindowSizeConstraints(ImVec2(List_Width, 0.0f), ImVec2(List_Width, List_Max_Height));

	ImGui::PushFont(m_pReleaseFont);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.05f, 0.07f, 0.12f, 0.88f));
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.15f, 0.20f, 0.40f, 0.95f));
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.20f, 0.28f, 0.55f, 0.95f));
	ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, ImVec4(0.15f, 0.20f, 0.40f, 0.95f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.45f, 0.55f, 0.90f, 0.90f));
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.18f, 0.22f, 0.35f, 0.90f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);

	if (ImGui::Begin(u8"人間一覧", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::SetWindowFontScale(0.7f);

		// 破棄されていない人間を集める
		std::vector<CHuman*> Humans;
		for (CHuman* pHuman : GetScene()->GetGameObjects<CHuman>())
		{
			if (!pHuman->IsDestroy()) Humans.push_back(pHuman);
		}

		// 解放済みの職業名と、職業ごとの人数
		const std::vector<std::string> JobNames = CCivLevelManager::GetInstance()->GetUnlockJobNames();
		std::unordered_map<std::string, int> JobCounts;
		for (CHuman* pHuman : Humans) JobCounts[pHuman->GetHumanJob()->GetJobName()]++;
		const int nNeetCount = JobCounts[JobName::Neet];

		ImGui::Text(u8"人数: %d人 (無職: %d人)", static_cast<int>(Humans.size()), nNeetCount);

		// 職業ごとの人数の増減(+は無職の人間を1人その職業に、-はその職業の人間を1人無職にする)
		// 職業が増えても場所を取らないよう、2列に並べて表示する
		ImGui::Separator();
		ImGui::Text(u8"[職業ごとの人数]");
		if (ImGui::BeginTable("JobTable", 8, ImGuiTableFlags_SizingFixedFit))
		{
			int nIndex = 0;
			for (const std::string& jobName : JobNames)
			{
				if (jobName == JobName::Neet) continue;

				const int nColumn = (nIndex % 2) * 4;
				if (nIndex % 2 == 0) ImGui::TableNextRow();
				++nIndex;

				ImGui::PushID(jobName.c_str());
				const int nCount = JobCounts[jobName];

				ImGui::TableSetColumnIndex(nColumn);
				ImGui::AlignTextToFramePadding();
				ImGui::Text(u8"%s", jobName.c_str());

				ImGui::TableSetColumnIndex(nColumn + 1);
				ImGui::AlignTextToFramePadding();
				ImGui::Text(u8"%d人", nCount);

				ImGui::TableSetColumnIndex(nColumn + 2);
				ImGui::BeginDisabled(nCount == 0);
				if (ImGui::Button("-##Dec", ImVec2(26.0f, 0.0f)))
				{
					// 一覧の後ろの人間から無職に戻す
					for (auto it = Humans.rbegin(); it != Humans.rend(); ++it)
					{
						if ((*it)->GetHumanJob()->GetJobName() != jobName) continue;
						(*it)->SetHumanJob(CreateJobByName(JobName::Neet, **it));
						break;
					}
				}
				ImGui::EndDisabled();

				ImGui::TableSetColumnIndex(nColumn + 3);
				ImGui::BeginDisabled(nNeetCount == 0);
				if (ImGui::Button("+##Inc", ImVec2(26.0f, 0.0f)))
				{
					// 一覧の前の無職の人間から割り当てる
					for (CHuman* pHuman : Humans)
					{
						if (pHuman->GetHumanJob()->GetJobName() != JobName::Neet) continue;
						pHuman->SetHumanJob(CreateJobByName(jobName, *pHuman));
						break;
					}
				}
				ImGui::EndDisabled();
				ImGui::PopID();
			}
			ImGui::EndTable();
		}

		// 人間ごとの一覧(名前を押すと選択、職業欄から直接職業を変更できる)
		ImGui::Separator();
		static bool bNeetOnly = false;
		ImGui::Checkbox(u8"無職のみ表示", &bNeetOnly);

		const ImGuiTableFlags TableFlags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit;
		// 表示する人数に合わせた高さにする(人数が少ないときは小さく、多いときはスクロールさせる)
		const int nRowCount = bNeetOnly ? nNeetCount : static_cast<int>(Humans.size());
		const float fRowHeight = ImGui::GetFrameHeight() + ImGui::GetStyle().CellPadding.y * 2.0f;
		const float fTableHeight = std::min(Table_Max_Height, fRowHeight * (nRowCount + 1) + 4.0f);
		if (ImGui::BeginTable("HumanTable", 3, TableFlags, ImVec2(0.0f, fTableHeight)))
		{
			ImGui::TableSetupScrollFreeze(0, 1);
			ImGui::TableSetupColumn(u8"名前", ImGuiTableColumnFlags_WidthFixed, 70.0f);
			ImGui::TableSetupColumn(u8"職業", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn(u8"空腹", ImGuiTableColumnFlags_WidthFixed, 45.0f);
			ImGui::TableHeadersRow();

			for (CHuman* pHuman : Humans)
			{
				const std::string currentJob = pHuman->GetHumanJob()->GetJobName();
				if (bNeetOnly && currentJob != JobName::Neet) continue;

				const bool isSelected = (pHuman == m_pHumanObject);
				ObjectID id = pHuman->GetID();
				const std::string name = id.m_sName + std::to_string(id.m_nSameCount + 1);

				ImGui::TableNextRow();
				ImGui::PushID(pHuman);
				if (isSelected) ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImGui::GetColorU32(ImVec4(0.30f, 0.40f, 0.80f, 0.55f)));

				// 名前: 押すと、その人間を選択してカメラで注視する
				ImGui::TableSetColumnIndex(0);
				if (ImGui::Selectable(name.c_str(), isSelected))
				{
					m_pHumanObject = pHuman;
					m_pGameObject = m_pHumanObject;
				}

				// 職業: 直接変更する(変更した人間は選択状態にするが、カメラは動かさない)
				ImGui::TableSetColumnIndex(1);
				ImGui::SetNextItemWidth(-FLT_MIN);
				if (ImGui::BeginCombo("##Job", currentJob.c_str()))
				{
					for (const std::string& jobName : JobNames)
					{
						if (ImGui::Selectable(jobName.c_str(), jobName == currentJob))
						{
							pHuman->SetHumanJob(CreateJobByName(jobName, *pHuman));
							m_pHumanObject = pHuman;
						}
					}
					ImGui::EndCombo();
				}

				// 空腹値: 少ないほど目立つ色にする
				ImGui::TableSetColumnIndex(2);
				const float fHungerRate = pHuman->GetHunger() / Max_Hunger;
				const ImVec4 hungerColor = fHungerRate < 0.25f ? ImVec4(1.0f, 0.4f, 0.4f, 1.0f)
					: fHungerRate < 0.5f ? ImVec4(1.0f, 0.85f, 0.3f, 1.0f)
					: ImVec4(0.8f, 1.0f, 0.8f, 1.0f);
				ImGui::TextColored(hungerColor, "%d", static_cast<int>(pHuman->GetHunger()));

				ImGui::PopID();
			}
			ImGui::EndTable();
		}
	}
	ImGui::End();

	ImGui::PopStyleVar();
	ImGui::PopStyleColor(6);
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
	ImGui::Begin(u8"建築物", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
	
	// コンボボックスで選択
	static int currentBuildObjectIndex = 0;
	// インスタンスの取得
	CScene* pScene = GetScene();
	// 建築物オブジェクトの取得
	auto buildObjects = pScene->GetGameObjects<CBuildObject>();
	// vector型に変換
	std::vector<CBuildObject*> buildObjectVec(buildObjects.begin(), buildObjects.end());

	if (ImGui::BeginCombo("##SelectBuild", buildObjects.empty() ? u8"建築物が見つかりません" : u8"建築物選択"))
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
	std::string SelectButtonLabel = u8"カメラ:";
	SelectButtonLabel += (pFocusBuildObject) ? u8"注視をやめる" : u8"注視する";

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
	std::string selectedObjectName = u8"建築物を選択していません";
	// 選択されている場合
	if (m_pBuildObject != nullptr)
	{
		ObjectID id = m_pBuildObject->GetID();
		selectedObjectName = id.m_sName + std::to_string(id.m_nSameCount + 1);
	}

	ImGui::Text(selectedObjectName.c_str());

	// 名前の横に、施設の説明を開閉するボタンを表示
	static bool s_bShowDescription = false;
	if (m_pBuildObject != nullptr)
	{
		ImGui::SameLine();
		if (ImGui::SmallButton(s_bShowDescription ? u8"説明を閉じる" : u8"説明"))
		{
			s_bShowDescription = !s_bShowDescription;
		}
	}

	ImGui::Separator();

	if (m_pBuildObject)m_pBuildObject->Inspecter();

	ImGui::End();
	ImGui::PopFont();

	// 施設の説明ウィンドウ(建築物ウィンドウの右隣)
	if (m_pBuildObject != nullptr && s_bShowDescription)
	{
		const float Description_Width = 380.0f;
		const float Build_Window_Width = 400.0f;

		ImGui::SetNextWindowPos(ImVec2(Build_Window_Width + 10.0f, SCREEN_HEIGHT - 400.0f));
		ImGui::SetNextWindowSizeConstraints(ImVec2(Description_Width, 0.0f), ImVec2(Description_Width, 10000.0f));

		ImGui::PushFont(m_pReleaseFont);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.05f, 0.07f, 0.12f, 0.92f));
		ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.15f, 0.20f, 0.40f, 0.95f));
		ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.20f, 0.28f, 0.55f, 0.95f));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.45f, 0.55f, 0.90f, 0.90f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);

		ObjectID id = m_pBuildObject->GetID();
		const std::string title = id.m_sName + u8"の説明";
		if (ImGui::Begin(title.c_str(), &s_bShowDescription, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar))
		{
			ImGui::SetWindowFontScale(0.75f);
			ImGui::PushTextWrapPos(Description_Width - 20.0f);
			ImGui::TextUnformatted(m_pBuildObject->GetDescription().c_str());

			ImGui::Separator();
			ImGui::Text(u8"現在のレベル: %d / %d", m_pBuildObject->GetBuildLevel(), CBuildObject::MAX_BUILD_LEVEL);
			ImGui::TextUnformatted(u8"建築職の人間が、建築と強化(レベルアップ)を行います。");
			ImGui::PopTextWrapPos();
		}
		ImGui::End();

		ImGui::PopStyleVar();
		ImGui::PopStyleColor(4);
		ImGui::PopFont();
	}
}

/****************************************//*
	@brief　	| ゲーム速度(倍速)の設定
	@param　	| nSpeed：1フレームに行うゲームの更新回数
*//****************************************/
void CImguiSystem::SetGameSpeed(int nSpeed)
{
	// 選択できる最大倍率の範囲に収める
	const int nMaxSpeed = ce_nGameSpeedSteps[ce_nGameSpeedStepCount - 1];
	if (nSpeed < 1) nSpeed = 1;
	if (nSpeed > nMaxSpeed) nSpeed = nMaxSpeed;

	m_nGameSpeed = nSpeed;
}

/****************************************//*
	@brief　	| 実際に行えたゲームの更新回数の通知
	@param　	| nUpdateCount：直前のフレームで実際に行った更新回数
*//****************************************/
void CImguiSystem::SetAppliedGameSpeed(int nUpdateCount)
{
	// 値の細かな揺れを抑えるため、移動平均にする
	m_fAppliedGameSpeed += (static_cast<float>(nUpdateCount) - m_fAppliedGameSpeed) * 0.1f;
}

/****************************************//*
	@brief　	| ゲーム速度表示
	@note		| クリックで 等速 → 2倍 → 4倍 → 8倍 → 等速 と切り替わる
*//****************************************/
void CImguiSystem::Release_DrawGameSpeed()
{
	// ゲームシーン以外では表示しない
	if (dynamic_cast<CSceneGame*>(GetScene()) == nullptr) return;

	// ウィンドウの位置設定(画面上部の中央)
	ImGui::SetNextWindowPos(ImVec2((SCREEN_WIDTH / 2) - 150.0f, 0.0f));
	// 幅は固定し、高さは内容に合わせる
	ImGui::SetNextWindowSizeConstraints(ImVec2(300.0f, 0.0f), ImVec2(300.0f, 200.0f));
	// フォントの設定
	ImGui::PushFont(m_pReleaseFont);
	ImGui::Begin(u8"ゲーム速度", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);

	// 現在の速度を表示するボタン(押すと次の速度に切り替わる)
	const std::string label = std::string(u8"ゲーム速度 x") + std::to_string(m_nGameSpeed);
	if (ImGui::Button(label.c_str(), ImVec2(-1.0f, 0.0f)))
	{
		// 現在の速度の位置を探し、次の速度へ切り替える(最後の次は等速に戻る)
		int nIndex = 0;
		for (int i = 0; i < ce_nGameSpeedStepCount; ++i)
		{
			if (ce_nGameSpeedSteps[i] == m_nGameSpeed) nIndex = i;
		}
		SetGameSpeed(ce_nGameSpeedSteps[(nIndex + 1) % ce_nGameSpeedStepCount]);
	}

	// 処理が追いつかず、設定した倍速を実現できていない場合は、実効倍率を表示する
	if (m_nGameSpeed > 1 && m_fAppliedGameSpeed < static_cast<float>(m_nGameSpeed) - 0.2f)
	{
		ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), u8"処理が追いつかず x%.1f", m_fAppliedGameSpeed);
	}

	ImGui::End();
	ImGui::PopFont();
}

/****************************************//*
	@brief　	| 選択しているオブジェクトの確認
	@note		| 選択中のオブジェクトが破棄されて解放済みになっている場合は、選択を解除する
*//****************************************/
void CImguiSystem::ValidateSelection()
{
	CScene* pScene = GetScene();
	if (pScene == nullptr) return;

	if (m_pGameObject && !pScene->Contains(m_pGameObject)) m_pGameObject = nullptr;
	if (m_pHumanObject && !pScene->Contains(m_pHumanObject)) m_pHumanObject = nullptr;
	if (m_pBuildObject && !pScene->Contains(m_pBuildObject)) m_pBuildObject = nullptr;
}
