/**************************************************//*
	@file	| SceneTitle.cpp
	@brief	| タイトルシーンクラスのcppファイル
	@note	| タイトルシーン内のゲームオブジェクト管理、更新、描画等を行う
			| CSceneを継承
*//**************************************************/
#include "SceneTitle.h"
#include "Input.h"
#include "Main.h"
#include "Sprite3DRenderer.h"
#include "ImguiSystem.h"
#include "SpriteObject.h"

/*****************************************//*
	@brief　	| コンストラクタ
*//*****************************************/
CSceneTitle::CSceneTitle()
	:CScene()
	, m_nSelectIndex(0)
	, m_pMenuObjectList()
{
}

/*****************************************//*
	@brief　	| デストラクタ
*//*****************************************/
CSceneTitle::~CSceneTitle()
{
}

/*****************************************//*
	@brief　	| 初期化処理
*//*****************************************/
void CSceneTitle::Init()
{
	// 基底クラスの初期化処理
	CScene::Init();

	// 背景の生成
	CSpriteObject* pBackground = AddGameObject<CSpriteObject>(Tag::GameObject, "TitleBackground");
	pBackground->GetComponent<CSprite3DRenderer>()->SetKey("TitleBackGround");
	pBackground->SetPos(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	pBackground->SetSize(DirectX::XMFLOAT3(80.0f, 50.0f, 10.0f));
	pBackground->SetColor(DirectX::XMFLOAT4(3.0f, 3.0f, 3.0f, 0.4f));


	// ゲーム開始ボタンの生成
	CTitleObject* pStartButton = AddGameObject<CTitleObject>(Tag::UI, "StartGameButton");
	pStartButton->GetComponent<CSprite3DRenderer>()->SetKey("GameStartButton");
	pStartButton->SetPos(DirectX::XMFLOAT3(0.0f, -10.0f, 0.0f));
	pStartButton->SetSize(DirectX::XMFLOAT3(10.0f, 5.0f, 10.0f));
	pStartButton->SetFunction([this]()
		{
			Mode_SeedInputMode();
		});
	m_pMenuObjectList.push_back(pStartButton);

	// ゲーム終了ボタンの生成
	CTitleObject* pExitButton = AddGameObject<CTitleObject>(Tag::UI, "ExitGameButton");
	pExitButton->GetComponent<CSprite3DRenderer>()->SetKey("ExitGameButton");
	pExitButton->SetPos(DirectX::XMFLOAT3(0.0f, -15.0f, 0.0f));
	pExitButton->SetSize(DirectX::XMFLOAT3(10.0f, 5.0f, 10.0f));
	pExitButton->SetFunction([]()
		{
			// ゲームを終了
			AppEnd();
		});
	m_pMenuObjectList.push_back(pExitButton);
}

/*****************************************//*
	@brief　	| 終了処理
*//*****************************************/
void CSceneTitle::Uninit()
{
	// メニュー選択肢オブジェクトの解放
	for (auto button : m_pMenuObjectList)
	{
		button = nullptr;
	}
	m_pMenuObjectList.clear();

	// 基底クラスの終了処理
	CScene::Uninit();
}

/*****************************************//*
	@brief　	| 更新処理
*//*****************************************/
void CSceneTitle::Update()
{
	// 基底クラスの更新処理
	CScene::Update();

	// シード値入力モードでない場合
	if (!m_bSeedInputMode)
	{

		// メニュー選択処理
		if (IsKeyTrigger('W') || IsKeyTrigger(VK_UP))
		{
			m_nSelectIndex--;
			if (m_nSelectIndex < 0)
			{
				m_nSelectIndex = 0;
			}
		}
		if (IsKeyTrigger('S') || IsKeyTrigger(VK_DOWN))
		{
			m_nSelectIndex++;
			if (m_nSelectIndex >= m_pMenuObjectList.size())
			{
				m_nSelectIndex = (m_pMenuObjectList.size() - 1);
			}
		}
		if (IsKeyTrigger(VK_RETURN))
		{
			// 選択肢に応じた処理を実行
			m_pMenuObjectList[m_nSelectIndex]->Execute();
		}

		// メニュー選択肢の拡大・縮小処理
		for (auto button : m_pMenuObjectList)
		{
			// 選択されているボタンの拡大処理
			if (button == m_pMenuObjectList[m_nSelectIndex])
			{
				DirectX::XMFLOAT3 scale = button->GetSize();
				scale.x += 0.1f;
				scale.y += 0.06f;
				if (scale.x > 15.0f)scale.x = 15.0f;
				if (scale.y > 8.0f)scale.y = 8.0f;
				button->SetSize(scale);
			}
			else
			{
				// 選択されていないボタンの縮小処理
				DirectX::XMFLOAT3 scale = button->GetSize();
				scale.x -= 0.1f;
				scale.y -= 0.06f;
				if (scale.x < 10.0f)scale.x = 10.0f;
				if (scale.y < 5.0f)scale.y = 5.0f;
				button->SetSize(scale);
			}
		}
	}
	// シード値入力モードの場合
	else
	{
		if (IsKeyTrigger(VK_RETURN))
		{
			// 選択肢に応じた処理を実行
			m_pMenuObjectList[0]->Execute();
		}
	}

	// 削除可能なオブジェクトの削除処理
	for (auto& Objectlist : m_pGameObject_List)
	{
		Objectlist.remove_if([](CGameObject* pObj)
			{
				if (pObj->IsDestroy())
				{
					pObj->Uninit();
					pObj->OnDestroy();
					SAFE_DELETE(pObj);
					return true;
				}
				return false;
			});
	}
}

/*****************************************//*
	@brief　	| 描画処理
*//*****************************************/
void CSceneTitle::Draw()
{
	// 基底クラスの描画処理
	CScene::Draw();

	// シード値入力モードの場合シード値入力システムの描画処理を実行
	if (m_bSeedInputMode)CImguiSystem::GetInstance()->DrawSeedInputMode();
}

/*****************************************//*
	@brief　	| シード値入力モードの移行
*//*****************************************/
void CSceneTitle::Mode_SeedInputMode()
{
	// メニュー選択肢オブジェクトの解放
	for (auto button : m_pMenuObjectList)
	{
		button->Destroy();
		button = nullptr;
	}
	m_pMenuObjectList.clear();

	// シード値入力モードに移行
	m_bSeedInputMode = true;

	// ゲーム開始ボタンの生成
	CTitleObject* pStartButton = AddGameObject<CTitleObject>(Tag::UI, "StartGameButton");
	pStartButton->GetComponent<CSprite3DRenderer>()->SetKey("GameStartButton");
	pStartButton->SetPos(DirectX::XMFLOAT3(0.0f, -10.0f, 0.0f));
	pStartButton->SetSize(DirectX::XMFLOAT3(10.0f, 5.0f, 10.0f));
	pStartButton->SetFunction([]()
		{
			// シード値の決定
			CImguiSystem::GetInstance()->DecideSettingSeed();
			// シーンをゲームシーンに変更
			ChangeScene(new CSceneGame());
			SetOnImgui(true);
		});
	m_pMenuObjectList.push_back(pStartButton);
}
