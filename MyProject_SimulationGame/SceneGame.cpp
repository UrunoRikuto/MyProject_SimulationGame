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
	CGeneratorManager::GetInstance()->AddObserver(*(new CHumanGenerator()));

	// フィールド管理システムの初期化
	CFieldManager::GetInstance()->AssignFieldCellType();

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

	// ゲーム内時間の更新
	CGameTimeManager::GetInstance()->UpdateGameTime();
}

/****************************************//*
	@brief　	| 描画処理
*//****************************************/
void CSceneGame::Draw()
{
	// デバッグ用のフィールドグリッド描画
	if (CImguiSystem::GetInstance()->IsCellsDraw())
	{
		for (auto cell : CFieldManager::GetInstance()->GetFieldGrid()->GetFieldCells())
		{
			for (auto fieldCell : cell)
			{
				fieldCell->DebugDraw();
			}
		}
	}
	else
	{
		// 基底クラスの描画処理
		CScene::Draw();
	}
}
