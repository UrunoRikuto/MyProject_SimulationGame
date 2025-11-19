/**************************************************//*
    @file	| ModelRenderer.cpp
    @brief	| モデル描画コンポーネントクラスのcppファイル
    @note	| モデル描画を扱う
            | CRendererComponentを継承
*//**************************************************/
#include "ModelRenderer.h"
#include "Camera.h"

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

    // カメラ情報の取得
    CCamera* pCamera = CCamera::GetInstance();

    // 行列計算
    DirectX::XMFLOAT4X4 wvp[3];

    // ワールド行列
    DirectX::XMMATRIX world =
        DirectX::XMMatrixScaling(m_tParam.m_f3Size.x, m_tParam.m_f3Size.y, m_tParam.m_f3Size.z) *
        DirectX::XMMatrixRotationRollPitchYaw(m_tParam.m_f3Rotate.x, m_tParam.m_f3Rotate.y, m_tParam.m_f3Rotate.z) *
        DirectX::XMMatrixTranslation(m_tParam.m_f3Pos.x, m_tParam.m_f3Pos.y, m_tParam.m_f3Pos.z);
    world = DirectX::XMMatrixTranspose(world);
    DirectX::XMStoreFloat4x4(&wvp[0], world);

    // ビュー行列
    wvp[1] = pCamera->GetViewMatrix();

    // プロジェクション行列
    wvp[2] = pCamera->GetProjectionMatrix();

    // シェーダーに行列を渡す
	m_pVS->WriteBuffer(0, wvp);

    // モデルにシェーダーをセット
    Model* pModel = std::get<ModelParam>(m_RendererObjectMap.find(m_sKey.c_str())->second.m_Data).m_pModel;

    pModel->SetVertexShader(m_pVS);

    pModel->SetPixelShader(m_pPS);

    // モデルの描画
    for (unsigned int i = 0; i < pModel->GetMeshNum(); i++)
    {
        Model::Mesh Mesh = *pModel->GetMesh(i);
        Model::Material material = *pModel->GetMaterial(Mesh.materialID);

		m_pVS->SetTexture(0, material.pTexture);
		m_pPS->SetTexture(0, material.pTexture);

        if (pModel) pModel->Draw(i);
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
