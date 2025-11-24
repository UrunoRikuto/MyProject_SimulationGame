/**************************************************//*
	@file	| SkyBox.cpp
	@brief	| スカイボックスクラスのcppファイル
	@note	| CGameObjectを継承
*//**************************************************/
#include "SkyBox.h"
#include "ModelRenderer.h"
#include "Camera.h"

/*****************************************//*
	@brief　	| コンストラクタ
*//*****************************************/
CSkyBox::CSkyBox()
	: CGameObject()
{
	// モデルレンダラーコンポーネントの追加
	AddComponent<CModelRenderer>();
}

/*****************************************//*
	@brief　	| デストラクタ
*//*****************************************/
CSkyBox::~CSkyBox()
{
}

/*****************************************//*
	@brief　	| 初期化処理
*//*****************************************/
void CSkyBox::Init()
{
	// 基底クラスの初期化処理
	CGameObject::Init();

	// モデルレンダラーコンポーネントの設定
	CModelRenderer* pModelRenderer = GetComponent<CModelRenderer>();
	pModelRenderer->SetKey("SkyBox");
	pModelRenderer->SetRendererParam(m_tParam);

	// 頂点シェーダーの設定
	VertexShader* pVS = new VertexShader(VSType::Object);
	pModelRenderer->SetVertexShader(pVS);
	// ピクセルシェーダーの設定
	PixelShader* pPS = new PixelShader(PSType::TexColor);
	pModelRenderer->SetPixelShader(pPS);

}

/*****************************************//*
	@brief　	| 更新処理
*//*****************************************/
void CSkyBox::Update()
{
	// 基底クラスの更新処理
	CGameObject::Update();

	// カメラに追従させる
	CCamera* pCamera = CCamera::GetInstance();
	DirectX::XMFLOAT3 cameraPos = pCamera->GetPos();
	m_tParam.m_f3Pos = cameraPos;

	// スカイボックスを徐々に回転させる
	m_tParam.m_f3Rotate.y += 0.0005f;
}