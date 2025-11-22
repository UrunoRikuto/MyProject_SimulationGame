/**************************************************//*
	@file	| Scene.cpp
	@brief	| シーンのベースクラスのcppファイル
	@note	| シーン内のゲームオブジェクト管理、更新、描画等を行う
*//**************************************************/
#include "GameObject.h"
#include "Scene.h"
#include "Camera.h"
#include "Geometory.h"


/****************************************//*
	@brief　	| コンストラクタ
*//****************************************/
CScene::CScene()
{

}

/****************************************//*
    @brief　	| デストラクタ
*//****************************************/
CScene::~CScene()
{

}

/****************************************//*
    @brief　	| 初期化処理
*//****************************************/
void CScene::Init()
{

}

/****************************************//*
    @brief　	| 終了処理
*//****************************************/
void CScene::Uninit()
{
	// ゲームオブジェクトの解放
	for (auto list : m_pGameObject_List)
	{
		// リスト内の全てのゲームオブジェクトを解放
        for (auto obj : list)
        {
            obj->Uninit();
            delete obj;
        }
        list.clear();
	}

	// リストのクリア
    m_tIDVec.clear();
}

/****************************************//*
    @brief　	| 更新処理
*//****************************************/
void CScene::Update()
{
	// ゲームオブジェクトの更新
    for (auto& list : m_pGameObject_List)
    {
		// リスト内の全てのゲームオブジェクトを更新
        for (auto obj : list)
        {
			// アクティブ状態のオブジェクトのみ更新
            obj->Update();
        }
    }

	// オブジェクトIDリストから破棄予定のオブジェクトのIDを削除
    for (auto itr = m_tIDVec.begin(); itr != m_tIDVec.end();)
    {
		// もしIDに対応するゲームオブジェクトが破棄予定なら
        if (GetGameObject(*itr)->IsDestroy())
        {
			// リストから削除
            itr = m_tIDVec.erase(itr);
        }
        else
        {
			// 次の要素へ進む
            itr++;
        }
    }

	// ゲームオブジェクトリストから破棄予定のオブジェクトを削除
    for (auto& list : m_pGameObject_List)
    {
		// 破棄予定のオブジェクトをリストから削除
        list.remove_if([](CGameObject* pObj)
            {
                if (pObj->IsDestroy())
                {
                    pObj->OnDestroy();
                    pObj->Uninit();
                    delete pObj;
                    pObj = nullptr;

                    return true;
                }
                return false;
            });

    }
}

/****************************************//*
    @brief　	| 描画処理
*//****************************************/
void CScene::Draw()
{
	// カメラのビュー行列、プロジェクション行列をジオメトリに設定
    CCamera* pCamera = CCamera::GetInstance();
    Geometory::SetView(pCamera->GetViewMatrix());
    Geometory::SetProjection(pCamera->GetProjectionMatrix());

	// ゲームオブジェクトの描画
    for (auto& list : m_pGameObject_List)
    {
		// リスト内の全てのゲームオブジェクトを描画
        for (auto obj : list)
        {
            obj->Draw();
        }
    }
}

/****************************************//*
    @brief　	| オブジェクトIDリストの取得
    @return     | オブジェクトIDリストの参照
*//****************************************/
std::vector<ObjectID> CScene::GetIDVec()
{
    return m_tIDVec;
}

/****************************************//*
    @brief　	| ゲームオブジェクトリストの取得
    @return     | ゲームオブジェクトリストの参照
*//****************************************/
std::array<std::list<CGameObject*>, (int)Tag::Max> CScene::GetGameObjectList()
{
    return m_pGameObject_List;
}
