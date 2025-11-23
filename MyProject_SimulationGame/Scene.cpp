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
    std::vector<ObjectID> newIDVec;
    // 各オブジェクトの Update と同時に破棄判定・削除を行う
    for (auto& list : m_pGameObject_List)
    {
        for (auto it = list.begin(); it != list.end(); )
        {
            CGameObject* obj = *it;
            obj->Update();

            if (obj->IsDestroy())
            {
                obj->OnDestroy();
                obj->Uninit();
                delete obj;
                it = list.erase(it); // イテレータ対応で安全に削除
            }
            else
            {
                newIDVec.push_back(obj->GetID());
                ++it;
            }
        }
    }
    // 残存オブジェクトの ID で m_tIDVec を再構築
    m_tIDVec = std::move(newIDVec);
}

/****************************************//*
    @brief　	| 描画処理
*//****************************************/
void CScene::Draw()
{
    CCamera* pCamera = CCamera::GetInstance();
    Geometory::SetView(pCamera->GetViewMatrix());
    Geometory::SetProjection(pCamera->GetProjectionMatrix());

    // カメラ位置をキャッシュ
    DirectX::XMFLOAT3 camPos = pCamera->GetPos(); // カメラに GetPosition() を用意している前提

    // カリング距離（チューニング可能）
    const float globalCullDistance = 100.0f;
    const float globalCullDistSq = globalCullDistance * globalCullDistance;

    for (auto& list : m_pGameObject_List)
    {
        for (auto obj : list)
        {
            // タグやフラグで無条件描画が必要かチェック
            if (obj->GetTag() == Tag::UI || obj->GetTag() == Tag::SkyBox)
            {
                obj->Draw();
                continue;
            }

            // オブジェクト位置と半径取得
            DirectX::XMFLOAT3 objPos = obj->GetPos();
            float r = obj->GetBoundingRadius(); // 実装を追加しておくこと

            float dx = objPos.x - camPos.x;
            float dy = objPos.y - camPos.y;
            float dz = objPos.z - camPos.z;
            float distSq = dx*dx + dy*dy + dz*dz;

            // 半径を考慮した閾値（二乗）
            float thresh = globalCullDistSq + 2.0f * globalCullDistance * r + r*r; // (D + r)^2
            if (distSq > thresh)
            {
                // カリング：描画をスキップ
                continue;
            }

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
