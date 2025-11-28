/**************************************************//*
	@file	| Scene.cpp
	@brief	| シーンのベースクラスのcppファイル
	@note	| シーン内のゲームオブジェクト管理、更新、描画等を行う
			| 描画負荷軽減のための視錐台カリング処理も実装
            | 参考サイト
			| ┗ https://zenn.dev/kazukisako/articles/frustum-culling-e77cb9d116c64a91b9e13090261e9399 (視錐台カリングの基礎と実装)
*//**************************************************/
#include "GameObject.h"
#include "Scene.h"
#include "Camera.h"
#include "Geometory.h"
#include <DirectXMath.h>

// @brief カリング距離
constexpr float CULLING_DISTANCE = 100.0f;

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
    @brief　	| 視錐台平面の抽出
    @param[out] | outPlanes：抽出した平面群を格納する配列（6要素）
    @param[in]  | viewProj：ビュー射影行列
*//****************************************/
static void ExtractFrustumPlanes(DirectX::XMFLOAT4 outPlanes[6], const DirectX::XMMATRIX& viewProj)
{
    using namespace DirectX;
    XMFLOAT4X4 m;
    XMStoreFloat4x4(&m, viewProj);
    // 左
    outPlanes[0] = XMFLOAT4(m._14 + m._11, m._24 + m._21, m._34 + m._31, m._44 + m._41);
    // 右
    outPlanes[1] = XMFLOAT4(m._14 - m._11, m._24 - m._21, m._34 - m._31, m._44 - m._41);
    // 上
    outPlanes[2] = XMFLOAT4(m._14 - m._12, m._24 - m._22, m._34 - m._32, m._44 - m._42);
    // 下
    outPlanes[3] = XMFLOAT4(m._14 + m._12, m._24 + m._22, m._34 + m._32, m._44 + m._42);
    // 近 
    outPlanes[4] = XMFLOAT4(m._13, m._23, m._33, m._43);
    // 遠
    outPlanes[5] = XMFLOAT4(m._14 - m._13, m._24 - m._23, m._34 - m._33, m._44 - m._43);

    // 正規化
    for (int i = 0; i < 6; ++i)
    {
        float a = outPlanes[i].x;
        float b = outPlanes[i].y;
        float c = outPlanes[i].z;
        float len = sqrtf(a * a + b * b + c * c);
        if (len > 0.0f)
        {
            outPlanes[i].x /= len;
            outPlanes[i].y /= len;
            outPlanes[i].z /= len;
            outPlanes[i].w /= len;
        }
    }
}

/****************************************//*
    @brief　	| 視錐台内判定（球と平面群の当たり判定）
    @param[in]  | planes：視錐台の平面群（6要素）
    @param[in]  | center：球の中心座標
    @param[in]  | radius：球の半径
    @return     | 判定結果（true：視錐台内、false：視錐台外）
*//****************************************/
static bool SphereInFrustum(const DirectX::XMFLOAT4 planes[6], const DirectX::XMFLOAT3& center, float radius)
{
    for (int i = 0; i < 6; ++i)
    {
        // 平面の法線ベクトルと球の中心との距離を計算
        const DirectX::XMFLOAT4& p = planes[i];

        // 平面方程式に球の中心を代入して距離を求める
        float dist = p.x * center.x + p.y * center.y + p.z * center.z + p.w;

        // 球が平面の裏側に完全にある場合は視錐台外
        if (dist + radius < 0.0f) return false;
    }
    return true;
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

	// 視錐台平面の抽出
    DirectX::XMFLOAT4X4 viewMat = pCamera->GetViewMatrix(false);
    DirectX::XMFLOAT4X4 projMat = pCamera->GetProjectionMatrix(false);
    DirectX::XMMATRIX view = DirectX::XMLoadFloat4x4(&viewMat);
    DirectX::XMMATRIX proj = DirectX::XMLoadFloat4x4(&projMat);
    DirectX::XMMATRIX viewProj = DirectX::XMMatrixMultiply(view, proj);
    DirectX::XMFLOAT4 frustumPlanes[6];
    ExtractFrustumPlanes(frustumPlanes, viewProj);

    // カメラ位置をキャッシュ
    DirectX::XMFLOAT3 camPos = pCamera->GetPos(); // カメラに GetPosition() を用意している前提

    // カリング距離（チューニング可能）
    const float globalCullDistance = CULLING_DISTANCE;
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
            float r = obj->GetBoundingRadius();

			// 視錐台との当たり判定
            if (!SphereInFrustum(frustumPlanes, objPos, r))
            {
                // 完全に視錐外 -> スキップ
                continue;
            }

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
