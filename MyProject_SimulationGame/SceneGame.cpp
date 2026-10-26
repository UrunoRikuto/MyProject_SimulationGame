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
#include "Animal.h"
#include "Profiler.h"

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
	CGeneratorManager* pGeneratorManager = CGeneratorManager::GetInstance();
	pGeneratorManager->AddObserver(*(new CWoodGenerator()));
	pGeneratorManager->AddObserver(*(new CStoneGenerator()));
	pGeneratorManager->AddObserver(*(new CGrassGenerator()));
	pGeneratorManager->AddObserver(*(new CHumanGenerator()));
	pGeneratorManager->AddObserver(*(new CAnimalGenerator()));

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
	// 動物の群れの近隣検索用の空間分割を、このステップの最初に作り直せるようにする
	CAnimal::BeginUpdateStep();

	{
		PROFILE_SCOPE("Update: Scene objects");
		CScene::Update();
	}

	// 生成管理システムの更新処理
	{
		PROFILE_SCOPE("Update: GeneratorManager");
		CGeneratorManager::GetInstance()->Update();
	}

	// ゲーム内時間の更新
	{
		PROFILE_SCOPE("Update: GameTime");
		CGameTimeManager::GetInstance()->UpdateGameTime();
	}

	// クールタイム処理
	{
		PROFILE_SCOPE("Update: BuildManager");
		CBuildManager::GetInstance()->CoolTimeUpdate();
	}

	// 削除可能なオブジェクトの削除処理
	PROFILE_SCOPE("Update: RemoveDestroyed");
	// 破棄が要求されたオブジェクトがない場合は、全オブジェクトを調べる必要がない
	if (CGameObject::GetDestroyRequestCount() == 0) return;
	CGameObject::ResetDestroyRequestCount();

	for (auto& Objectlist : m_pGameObject_List)
	{
		Objectlist.remove_if([this](CGameObject* pObj)
		{
			if (pObj->IsDestroy())
			{
				pObj->Uninit();
				pObj->OnDestroy();
				NotifyObjectRemoving(pObj);
				SAFE_DELETE(pObj);
				return true;
			}
			return false;
		});
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
