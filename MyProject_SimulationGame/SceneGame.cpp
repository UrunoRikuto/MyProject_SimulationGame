/**************************************************//*
	@file	| SceneGame.cpp
	@brief	| ゲームシーンクラスのcppファイル
	@note	| ゲームシーン内のゲームオブジェクト管理、更新、描画等を行う
			| CSceneを継承
*//**************************************************/
#include "SceneGame.h"
#include "Camera.h"
#include "GeneratorManager.h"

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
	// CGeneratorManager::GetInstance()->AddObserver(*(new CHumanGenerator()));

	CGeneratorManager::GetInstance()->NotifyObservers();
}

/****************************************//*
	@brief　	| 更新処理
*//****************************************/
void CSceneGame::Update()
{
	// 基底クラスの更新処理
	CScene::Update();

	// カメラの更新処理
	CCamera::GetInstance()->Update();
}

/****************************************//*
	@brief　	| 描画処理
*//****************************************/
void CSceneGame::Draw()
{
	// 基底クラスの描画処理
	CScene::Draw();
}
