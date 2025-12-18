/**************************************************//*
	@file	| SceneGame.cpp
	@brief	| ゲームシーンクラスのcppファイル
	@note	| ゲームシーン内のゲームオブジェクト管理、更新、描画等を行う
			| CSceneを継承
*//**************************************************/
#include "SceneGame.h"
#include "Camera.h"
#include "GeneratorManager.h"
#include "FieldManager.h"
#include "GameTimeManager.h"
#include "BuildManager.h"
#include "FieldGround.h"
#include "SkyBox.h"
#include "ImguiSystem.h"

#include "TestObject.h"
#include "Wolf_Animal.h"
#include "Oparation.h"
#include "Deer_Animal.h"

/****************************************//*
	@brief　	| コンストラクタ
*//****************************************/
CSceneGame::CSceneGame()
{

}

/****************************************//*
	@brief　	| デストラクタ
*//****************************************/
CSceneGame::~CSceneGame()
{
	CGameTimeManager::ReleaseInstance();
	CFieldManager::ReleaseInstance();
	CGeneratorManager::ReleaseInstance();
	CBuildManager::ReleaseInstance();
}

/****************************************//*
	@brief　	| 初期化処理
*//****************************************/
void CSceneGame::Init()
{
	// 基底クラスの初期化処理
	CScene::Init();

	// カメラの種類をゲーム用カメラに設定
	CCamera::GetInstance()->SetCameraKind(CameraKind::CAM_GAME);

	// 生成管理システムの通知処理を実行
	CGeneratorManager::GetInstance()->AddObserver(*(new CWoodGenerator()));
	CGeneratorManager::GetInstance()->AddObserver(*(new CStoneGenerator()));
	CGeneratorManager::GetInstance()->AddObserver(*(new CGrassGenerator()));
	CGeneratorManager::GetInstance()->AddObserver(*(new CHumanGenerator()));

	// 狼の生成
	DirectX::XMFLOAT3 wolfBasePos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	std::vector<CWolf_Animal*> wolfList;
	for (int i = 0; i < 5; ++i)
	{
		CWolf_Animal* pWolf = AddGameObject<CWolf_Animal>(Tag::GameObject, "Wolf_Animal");
		wolfBasePos.x += GetRandOfRange(-5, 5);
		wolfBasePos.z += GetRandOfRange(-5, 5);
		pWolf->SetPos(wolfBasePos);
		wolfList.push_back(pWolf);
	}
	// 群れの形成
	for (CWolf_Animal* wolf : wolfList)
	{
		wolf->RegisterToFlock(wolfList);
	}


	// シカの生成
	DirectX::XMFLOAT3 deerBasePos = DirectX::XMFLOAT3(20.0f, 0.0f, 20.0f);
	std::vector<CDeer_Animal*> deerList;
	for (int i = 0; i < 8; ++i)
	{
		CDeer_Animal* pDeer = AddGameObject<CDeer_Animal>(Tag::GameObject, "Deer_Animal");
		deerBasePos.x += GetRandOfRange(-5, 5);
		deerBasePos.z += GetRandOfRange(-5, 5);
		pDeer->SetPos(deerBasePos);
		deerList.push_back(pDeer);
	}
	for (CDeer_Animal* deer : deerList)
	{
		deer->RegisterToFlock(deerList);
	}


	// フィールド管理システムの初期化
	//CFieldManager::GetInstance()->AssignFieldCellType();

	// フィールド地面オブジェクトの生成
	AddGameObject<CFieldGround>(Tag::Field, "FieldGround");

	// スカイボックスオブジェクトの生成
	AddGameObject<CSkyBox>(Tag::SkyBox, "SkyBox");
}

/****************************************//*
	@brief　	| 更新処理
*//****************************************/
void CSceneGame::Update()
{
	// 基底クラスの更新処理
	CScene::Update();

	// 生成管理システムの更新処理
	CGeneratorManager::GetInstance()->Update();

	// ゲーム内時間の更新
	CGameTimeManager::GetInstance()->UpdateGameTime();

	// クールタイム処理
	CBuildManager::GetInstance()->CoolTimeUpdate();

	// ------------------------------------------------------------
	// シカの群れ情報（近傍）の更新
	// ※脅威検知/共有のロジック自体は各DeerのUpdate()->SetThreat()に任せる
	// ------------------------------------------------------------
	{
		auto deerList = GetGameObjects<CDeer_Animal>();
		if (!deerList.empty())
		{
			std::vector<CDeer_Animal*> deerVec(deerList.begin(), deerList.end());
			for (CDeer_Animal* deer : deerList)
			{
				if (!deer) continue;
				deer->RegisterToFlock(deerVec);
			}
		}
	}
}

/****************************************//*
	@brief　	| 描画処理
*//****************************************/
void CSceneGame::Draw()
{
	//// 基底クラスの描画処理
	CScene::Draw();
}
