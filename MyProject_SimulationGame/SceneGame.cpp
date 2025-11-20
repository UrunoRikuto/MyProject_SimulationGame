/**************************************************//*
	@file	| SceneGame.cpp
	@brief	| ゲームシーンクラスのcppファイル
	@note	| ゲームシーン内のゲームオブジェクト管理、更新、描画等を行う
			| CSceneを継承
*//**************************************************/
#include "SceneGame.h"
#include "Camera.h"
#include "TestObject.h"
#include "Human.h"
#include "Wood.h"

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

	// 人間オブジェクトの追加
	AddGameObject<CHuman>(Tag::GameObject, "Human");

	// 木オブジェクトの追加
	AddGameObject<CWood>(Tag::GameObject, "Wood");
	AddGameObject<CWood>(Tag::GameObject, "Wood");
	AddGameObject<CWood>(Tag::GameObject, "Wood");
	AddGameObject<CWood>(Tag::GameObject, "Wood");
	AddGameObject<CWood>(Tag::GameObject, "Wood");
}

/****************************************//*
	@brief　	| 更新処理
*//****************************************/
void CSceneGame::Update()
{
	// 基底クラスの更新処理
	CScene::Update();
}

/****************************************//*
	@brief　	| 描画処理
*//****************************************/
void CSceneGame::Draw()
{
	// 基底クラスの描画処理
	CScene::Draw();
}
