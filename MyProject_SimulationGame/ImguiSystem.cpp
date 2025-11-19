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

//-- 静的メンバ変数の初期化 --//
CImguiSystem* CImguiSystem::m_pInstance = nullptr;
constexpr float ce_fCharaSize = 30.0f;

/****************************************//*
	@brief　	| コンストラクタ
*//****************************************/
CImguiSystem::CImguiSystem()
	: m_pGameObject(nullptr)
	, m_bUpdate(true)
	, m_bCollisionDraw(true)
{
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
	DrawUpdateTick();
	DrawCollision();
	DrawFPS();

	// 選択しているゲームオブジェクトが存在する場合
	// 選択しているオブジェクトのインスペクター表示処理
	if (m_pGameObject)m_pGameObject->Inspecter();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

/****************************************//*
	@brief　	| 階層表示
*//****************************************/
void CImguiSystem::DrawHierarchy()
{
	ImGui::SetNextWindowPos(ImVec2(20, 20));
	ImGui::SetNextWindowSize(ImVec2(280, 300));
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

		if (ImGui::CollapsingHeader(std::string("[" + name + "]").c_str()))
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
}

/****************************************//*
	@brief　	| カメラのパラメータ表示
*//****************************************/
void CImguiSystem::DrawCameraParam()
{
	CCamera* pCamera = CCamera::GetInstance();
	CScene* pScene = GetScene();

	if (dynamic_cast<CSceneGame*>(pScene))pCamera->SetCameraKind(CameraKind::CAM_GAME);

	if (!pCamera) return;
	if (m_bUpdate) return;
	pCamera->SetCameraKind(CameraKind::CAM_DEBUG);
	ImGui::SetNextWindowPos(ImVec2(20, SCREEN_HEIGHT - 400));
	ImGui::SetNextWindowSize(ImVec2(280, 180));
	ImGui::Begin("Camera");
	ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(250, 160), ImGuiWindowFlags_NoTitleBar);


	if (ImGui::CollapsingHeader(std::string("[Transform]").c_str()))
	{
		ImGui::Text(std::string("Position").c_str());
		DirectX::XMFLOAT3 pos = pCamera->GetPos();
		ImGui::Text(std::string("PosX:" + std::to_string(pos.x)).c_str());
		ImGui::Text(std::string("PosY:" + std::to_string(pos.y)).c_str());
		ImGui::Text(std::string("PosZ:" + std::to_string(pos.z)).c_str());
		ImGui::Text("\n");

		ImGui::Text(std::string("Look").c_str());
		DirectX::XMFLOAT3 look = pCamera->GetLook();
		ImGui::Text(std::string("LookX:" + std::to_string(look.x)).c_str());
		ImGui::Text(std::string("LookY:" + std::to_string(look.y)).c_str());
		ImGui::Text(std::string("LookZ:" + std::to_string(look.z)).c_str());
		ImGui::Text("\n");

		ImGui::Text(std::string("UpVector").c_str());
		DirectX::XMFLOAT3 up = pCamera->GetUp();
		ImGui::Text(std::string("UpX:" + std::to_string(up.x)).c_str());
		ImGui::Text(std::string("UpY:" + std::to_string(up.y)).c_str());
		ImGui::Text(std::string("UpZ:" + std::to_string(up.z)).c_str());
	}


	ImGui::EndChild();
	ImGui::End();
}

/****************************************//*
	@brief　	| 更新を止めるチェックボックス表示
*//****************************************/
void CImguiSystem::DrawUpdateTick()
{
	ImGui::SetNextWindowPos(ImVec2(20, SCREEN_HEIGHT - 120));
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
	@brief　	| 当たり判定を表示するチェックボックス表示
*//****************************************/
void CImguiSystem::DrawCollision()
{
	ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH - 300, SCREEN_HEIGHT - 120));
	ImGui::SetNextWindowSize(ImVec2(280, 100));
	ImGui::Begin("Collision");

	ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(ce_f2InspecterSize), ImGuiWindowFlags_NoTitleBar);
	ImGui::Checkbox("DrawCollision", &m_bCollisionDraw);
	ImGui::EndChild();
	ImGui::End();
	if (!m_bCollisionDraw)return;

	auto CollisionVec = GetScene()->GetCollisionVec();
	for (int i = 0; i < CollisionVec.size(); i++)
	{
		CollisionVec[i]->Draw();
	}
}

/****************************************//*
	@brief　	| フレームレート表示
*//****************************************/
void CImguiSystem::DrawFPS()
{
	ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH / 2 + 170, 20.0f));
	ImGui::SetNextWindowSize(ImVec2(140, 70));
	ImGui::Begin("FPS");
	ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(120.0f, 30.0f), ImGuiWindowFlags_NoTitleBar);

	int fps = GetFPS();
	ImGui::Text(std::string("FPS:" + std::to_string(fps)).c_str());

	ImGui::EndChild();
	ImGui::End();
}
