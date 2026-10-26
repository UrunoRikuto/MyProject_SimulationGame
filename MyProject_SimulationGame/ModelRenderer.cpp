/**************************************************//*
    @file	| ModelRenderer.cpp
    @brief	| モデル描画コンポーネントクラスのcppファイル
    @note	| モデル描画を扱う
            | CRendererComponentを継承
*//**************************************************/
#include "ModelRenderer.h"
#include "Camera.h"
#include "Profiler.h"

// static batch map
std::unordered_map<std::string, CModelRenderer::Batch> CModelRenderer::s_batches;
std::unordered_map<PixelShader*, DirectX::XMFLOAT4> CModelRenderer::s_pendingPSColors;

/*****************************************//*
    @brief　	| デストラクタ
*//****************************************/
CModelRenderer::~CModelRenderer()
{

}

/*****************************************//*
    @brief　	| 初期化処理
*//****************************************/
void CModelRenderer::Init()
{
    // デフォルトでは深度バッファを有効にする
    m_bIsDepth = true;
}

/*****************************************//*
    @brief　	| 描画処理
*//****************************************/
void CModelRenderer::Draw()
{
    // キーが設定されていない時は描画しない
    if (m_sKey.empty()) return;

    // フラグによって深度バッファを使用するか決める
    RenderTarget* pRTV = GetDefaultRTV();
    DepthStencil* pDSV = GetDefaultDSV();

	// レンダーターゲットとデプスステンシルのセット
    if (m_bIsDepth) SetRenderTargets(1, &pRTV, pDSV);
    else SetRenderTargets(1, &pRTV, nullptr);

    // カリングのセットアップ用
    SetCullingMode(m_tParam.m_eCulling);

    // バッチの取得(モデルのポインタは最初の1回だけキーから探す)
    CModelRenderer::Batch& batch = s_batches[m_sKey];
    if (batch.pModel == nullptr)
    {
        const RendererObject* pObj = CRendererComponent::FindRendererObject(m_sKey);
        if (!pObj) return;
        batch.pModel = std::get<ModelParam>(pObj->m_Data).m_pModel;
        if (batch.pModel == nullptr) return;
    }

    // ワールド行列
    DirectX::XMMATRIX world =
        DirectX::XMMatrixScaling(m_tParam.m_f3Size.x, m_tParam.m_f3Size.y, m_tParam.m_f3Size.z) *
        DirectX::XMMatrixRotationRollPitchYaw(m_tParam.m_f3Rotate.x, m_tParam.m_f3Rotate.y, m_tParam.m_f3Rotate.z) *
        DirectX::XMMatrixTranslation(m_tParam.m_f3Pos.x, m_tParam.m_f3Pos.y, m_tParam.m_f3Pos.z);

    // ピクセルシェーダーの色は、描画のたびに書き込まず、最後に設定された色をフラッシュ時に書き込む
    // (頂点シェーダーの行列は、フラッシュ時にバッチごとに書き込むためここでは不要)
    switch (m_pPS->m_eType)
    {
    case PSType::TexColor:
        s_pendingPSColors[m_pPS] = m_tParam.m_f4Color;
        break;
    }

    //ここでバッチに追加（ワールド行列のみインスタンス化）
    batch.vs = m_pVS;
    batch.ps = m_pPS;
    DirectX::XMFLOAT4X4 worldMat;
    DirectX::XMStoreFloat4x4(&worldMat, world);
    batch.instances.push_back(worldMat);

    // 注意: 実際のレンダリングはフレーム終端で一括して行うため外部から FlushBatches() を呼んでください
}

// バッチをフラッシュしてインスタンシングで描画
void CModelRenderer::FlushBatches()
{
    PROFILE_SCOPE("Draw: ModelRenderer::FlushBatches");
    CCamera* pCamera = CCamera::GetInstance();

    // フレーム内で最後に設定されたピクセルシェーダーの色を書き込む
    for (auto& kv : s_pendingPSColors)
    {
        kv.first->WriteBuffer(0, &kv.second);
    }
    s_pendingPSColors.clear();
    for (auto& kv : s_batches)
    {
        const std::string& key = kv.first;
        Batch& batch = kv.second;
        if (batch.instances.empty()) continue;

        const RendererObject* pObj = CRendererComponent::FindRendererObject(key);
        if (!pObj) continue;
        Model* pModel = std::get<ModelParam>(pObj->m_Data).m_pModel;
        if (!pModel) continue;

        // 各メッシュごとにインスタンスデータを渡して描画
        // インスタンスデータはワールド行列の配列（float[16]）で、そのまま渡せる
        const float* pInstanceData = reinterpret_cast<const float*>(batch.instances.data());

        // モデルにシェーダーをセット
        if (batch.vs) pModel->SetVertexShader(batch.vs);
        if (batch.ps) pModel->SetPixelShader(batch.ps);

        // シェーダーをセット
        if (batch.vs) batch.vs->Bind();
        if (batch.ps) batch.ps->Bind();

        // Prepare constant buffer with view/proj and useInstance flag
        struct CBWVP
        {
            DirectX::XMFLOAT4X4 world;
            DirectX::XMFLOAT4X4 view;
            DirectX::XMFLOAT4X4 proj;
            float useInstance;
            float pad[3];
        } cb;
        // world can be identity when using per-instance matrices
        DirectX::XMStoreFloat4x4(&cb.world, DirectX::XMMatrixIdentity());
        cb.view = pCamera->GetViewMatrix();
        cb.proj = pCamera->GetProjectionMatrix();
        cb.useInstance = 1.0f;

        if (batch.vs) batch.vs->WriteBuffer(0, &cb);

        for (unsigned int i = 0; i < pModel->GetMeshNum(); ++i)
        {
            Model::Mesh Mesh = *pModel->GetMesh(i);
            Model::Material material = *pModel->GetMaterial(Mesh.materialID);

            if (batch.vs) batch.vs->SetTexture(0, material.pTexture);
            if (batch.ps) batch.ps->SetTexture(0, material.pTexture);

            // インスタンスバッファを作成して描画
            pModel->DrawInstanced(pInstanceData, sizeof(DirectX::XMFLOAT4X4), static_cast<UINT>(batch.instances.size()), i);
        }

        batch.instances.clear();
    }
}

    /*****************************************//*
    @brief　	| Mesh情報を取得
    @return	| 読み込まれているモデルのメッシュ情報ベクター
*//****************************************/
std::vector<Model::Mesh> CModelRenderer::GetMesh()
{
    return std::get<ModelParam>(m_RendererObjectMap.find(m_sKey)->second.m_Data).m_tMeshVec;
}
