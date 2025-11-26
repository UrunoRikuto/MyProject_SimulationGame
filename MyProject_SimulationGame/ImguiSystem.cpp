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
#include "BuildManager.h"
#include "Human.h"
#include "GameTimeManager.h"

#include <algorithm>
#include <cstdio>

//-- 静的メンバ変数の初期化 --//
CImguiSystem* CImguiSystem::m_pInstance = nullptr;
constexpr float ce_fCharaSize =30.0f;

/****************************************//*
	@brief　	| コンストラクタ
*//****************************************/
CImguiSystem::CImguiSystem()
	: m_pGameObject(nullptr)
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
	ImGuiIO& io = ImGui::GetIO();

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	DrawHierarchy();
	DrawCameraParam();
	DrawInspecter();

	if (m_bDebug[static_cast<int>(DebugSystemFlag::Update)])	DrawUpdateTick();
	if (m_bDebug[static_cast<int>(DebugSystemFlag::FPS)])		DrawFPS();
	if (m_bDebug[static_cast<int>(DebugSystemFlag::Log)])		DrawDebugLog();
	if (m_bDebug[static_cast<int>(DebugSystemFlag::Generate)])	DrawCreateObjectButton();
	if (m_bDebug[static_cast<int>(DebugSystemFlag::CellsDraw)])	DrawCellsDebug();
	if (m_bDebug[static_cast<int>(DebugSystemFlag::BuildRequest)])	DrawBuildRequestList();
	if (m_bDebug[static_cast<int>(DebugSystemFlag::DebugTemplate)])	DrawDebugTemplateCreate();
	if (m_bDebug[static_cast<int>(DebugSystemFlag::GameTime)])		DrawGameTime();
#ifdef _DEBUG
	DrawDebugSystem();
#endif

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

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
	@brief　	| 階層表示
*//****************************************/
void CImguiSystem::DrawHierarchy()
{
	ImGui::SetNextWindowPos(ImVec2(20,20));
	ImGui::SetNextWindowSize(ImVec2(280,300));
	ImGui::Begin("Hierarchy");
	if (ImGui::Button("Select Item Clear"))
	{
		m_pGameObject = nullptr;
	}
	ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(250,260), ImGuiWindowFlags_NoTitleBar);

	ImGui::Checkbox("Human", &m_bOnlyHuman);
	CScene* pScene = GetScene();
	if (m_bOnlyHuman)
	{
		// Only humans: iterate directly
		auto Objects = pScene->GetGameObjects<CHuman>();
		for (auto obj : Objects)
		{
			ObjectID id = obj->GetID();
			std::string name = id.m_sName;
			name += std::to_string(id.m_nSameCount +1);

			std::string job = obj->GetHumanJob()->GetJobName();

			name += " [" + job + "]";

			if (ImGui::Button(name.c_str()))
			{
				m_pGameObject = obj;
			}
		}
	}
	else
	{
		// More efficient grouping: get vector, sort by name then samecount, then walk once
		auto Objects = pScene->GetIDVec(); // assume returns std::vector<ObjectID>
		if (!Objects.empty())
		{
			// sort in-place
			std::sort(Objects.begin(), Objects.end(), [](const ObjectID& a, const ObjectID& b)
			{
				if (a.m_sName != b.m_sName) return a.m_sName < b.m_sName;
				return a.m_nSameCount < b.m_nSameCount;
			});

			size_t i =0;
			while (i < Objects.size())
			{
				const std::string& name = Objects[i].m_sName;
				// count how many have this name (contiguous after sort)
				size_t j = i +1;
				while (j < Objects.size() && Objects[j].m_sName == name) ++j;
				int nItrCount = static_cast<int>(j - i);

				// build header once
				char headerBuf[128];
				snprintf(headerBuf, sizeof(headerBuf), "[%s]:%d", name.c_str(), nItrCount);
				if (ImGui::CollapsingHeader(headerBuf))
				{
					for (int k =0; k < nItrCount; ++k)
					{
						std::string sButtonName = name;
						sButtonName += std::to_string(k +1);
						ObjectID id = Objects[i + k];
						if (ImGui::Button(sButtonName.c_str()))
						{
							m_pGameObject = pScene->GetGameObject(id);
						}
					}
				}
				i = j; // advance to next group
			}
		}
	}

	ImGui::EndChild();
	ImGui::End();
}

/****************************************//*
	@brief　	| カメラのパラメータ表示
*//****************************************/
void CImguiSystem::DrawCameraParam()
{
	CCamera* pCamera = CCamera::GetInstance();
	CScene* pScene = GetScene();

	if (!pCamera) return;

	ImGui::SetNextWindowPos(ImVec2(20, SCREEN_HEIGHT -400));
	ImGui::SetNextWindowSize(ImVec2(280,250));
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
}

/****************************************//*
	@brief　	| インスペクター表示
*//****************************************/
void CImguiSystem::DrawInspecter()
{
	// 選択しているゲームオブジェクトが存在する場合
	if (m_pGameObject)
	{
		// インスペクター表示処理を呼び出す
		m_pGameObject->Inspecter();
	}
	// 選択しているゲームオブジェクトが存在しない場合
	else
	{
		ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH - 300, 20));
		ImGui::SetNextWindowSize(ImVec2(280, SCREEN_HEIGHT - 140));
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

}

/****************************************//*
	@brief　	| デバッグ用チェックボックス表示
*//****************************************/
void CImguiSystem::DrawDebugSystem()
{
	ImGui::SetNextWindowSize(ImVec2(200, 130));

	ImGui::Begin("DebugSystem");
	ImGui::Checkbox("Update",		&m_bDebug[static_cast<int>(DebugSystemFlag::Update)]);
	ImGui::Checkbox("FPS",			&m_bDebug[static_cast<int>(DebugSystemFlag::FPS)]);
	ImGui::Checkbox("Log",			&m_bDebug[static_cast<int>(DebugSystemFlag::Log)]);
	ImGui::Checkbox("Generate",		&m_bDebug[static_cast<int>(DebugSystemFlag::Generate)]);
	ImGui::Checkbox("CellsDraw", &m_bDebug[static_cast<int>(DebugSystemFlag::CellsDraw)]);
	ImGui::Checkbox("BuildRequest", &m_bDebug[static_cast<int>(DebugSystemFlag::BuildRequest)]);
	ImGui::Checkbox("DebugTemplate", &m_bDebug[static_cast<int>(DebugSystemFlag::DebugTemplate)]);
	ImGui::Checkbox("GameTime", &m_bDebug[static_cast<int>(DebugSystemFlag::GameTime)]);

	ImGui::End();
}

/****************************************//*
	@brief　	| 更新を止めるチェックボックス表示
*//****************************************/
void CImguiSystem::DrawUpdateTick()
{
	ImGui::SetNextWindowSize(ImVec2(280, 100));
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
}

/****************************************//*
	@brief　	| フレームレート表示
*//****************************************/
void CImguiSystem::DrawFPS()
{
	ImGui::SetNextWindowSize(ImVec2(140,70));
	ImGui::Begin("FPS");
	ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(120.0f,30.0f), ImGuiWindowFlags_NoTitleBar);

	int fps = GetFPS();
	ImGui::Text("FPS: %d", fps);

	ImGui::EndChild();
	ImGui::End();
}

/****************************************//*
	@brief　	| デバッグログ表示
*//****************************************/
void CImguiSystem::DrawDebugLog()
{
	ImGui::SetNextWindowSize(ImVec2(400,150));
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
}

/****************************************//*
	@brief　	| オブジェクト作成ボタン表示
*//****************************************/
void CImguiSystem::DrawCreateObjectButton()
{
	ImGui::SetNextWindowSize(ImVec2(200, 100));
	ImGui::Begin("CreateObject");

	for(int i = 0; i < m_pGenerator.size(); i++)
	{
		if (ImGui::Button(m_pGenerator[i].m_sName.c_str()))
		{
			m_pGenerator[i].m_pGenerator->Notify();
		}
	}

	ImGui::End();
}

/****************************************//*
	@brief　	| セルデバッグ表示
*//****************************************/
void CImguiSystem::DrawCellsDebug()
{
	ImGui::SetNextWindowSize(ImVec2(200, 100));
	ImGui::Begin("CellsDebug");
	ImGui::Checkbox("Cells Draw", &m_bCellsDraw);
	ImGui::End();
}

/****************************************//*
	@brief　	| 建築依頼リスト
*//****************************************/
void CImguiSystem::DrawBuildRequestList()
{
	ImGui::SetNextWindowSize(ImVec2(300,200));
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
}

/****************************************//*
	@brief　	| デバッグテンプレート作成
*//****************************************/
void CImguiSystem::DrawDebugTemplateCreate()
{
	ImGui::SetNextWindowSize(ImVec2(300, 150));
	ImGui::Begin("DebugTemplateCreate");

	ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(280, 100), ImGuiWindowFlags_NoTitleBar);
	ImGui::Text("Create Human Template");

	// 生成する数指定
	// 木材収集職
	static int nWoodGatherCount = 0;
	ImGui::InputInt("WoodGatherCount", &nWoodGatherCount);
	// 石材収集職
	static int nStoneGatherCount = 0;
	ImGui::InputInt("StoneGatherCount", &nStoneGatherCount);
	// 建築職
	static int nBuilderCount = 0;
	ImGui::InputInt("BuilderCount", &nBuilderCount);

	// テンプレート作成ボタン

	if (ImGui::Button("Create Human"))
	{
		CScene* pScene = GetScene();

		for(int i = 0; i < nWoodGatherCount; i++)
		{
			CHuman* human = pScene->AddGameObject<CHuman>(Tag::GameObject, "Human");
			human->SetHumanJob(CreateJobByName(JobName::WoodGatherer, *human));
		}
		for (int i = 0; i < nStoneGatherCount; i++)
		{
			CHuman* human = pScene->AddGameObject<CHuman>(Tag::GameObject, "Human");
			human->SetHumanJob(CreateJobByName(JobName::StoneGatherer, *human));
		}
		for (int i = 0; i < nBuilderCount; i++)
		{
			CHuman* human = pScene->AddGameObject<CHuman>(Tag::GameObject, "Human");
			human->SetHumanJob(CreateJobByName(JobName::Builder, *human));
		}
	}

	ImGui::EndChild();

	ImGui::End();
}

/****************************************//*
	@brief　	| ゲームタイマー表示
*//****************************************/
void CImguiSystem::DrawGameTime()
{
	ImGui::SetNextWindowSize(ImVec2(200, 100));

	ImGui::Begin("GameTime");
	ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(180.0f, 60.0f), ImGuiWindowFlags_NoTitleBar);

	float time = CGameTimeManager::GetInstance()->GetGameTime();
	ImGui::Text(std::string("GameTime:" + std::to_string(time)).c_str());

	int day = CGameTimeManager::GetInstance()->GetGameDays();
	ImGui::Text(std::string("GameDays:" + std::to_string(day)).c_str());

	ImGui::EndChild();
	ImGui::End();
}
