/**************************************************//*
	@file	| FieldGround.cpp
	@brief	| 地面クラスのcppファイル
	@note	| 地面の処理を定義
			| CGameObjectを継承
*//**************************************************/
#include "FieldGround.h"
#include "ModelRenderer.h"

/*****************************************//*
	@brief　	| コンストラクタ
*//*****************************************/
CFieldGround::CFieldGround()
	: CGameObject()
{
	// モデルレンダラーコンポーネントの追加
	AddComponent<CModelRenderer>();
}

/*****************************************//*
	@brief　	| デストラクタ
*//*****************************************/
CFieldGround::~CFieldGround()
{
}

/*****************************************//*
	@brief　	| 初期化処理
*//*****************************************/
void CFieldGround::Init()
{
	// 基底クラスの初期化処理
	CGameObject::Init();

	// サイズの設定
	m_tParam.m_f3Size = DirectX::XMFLOAT3(1000.0f, 1.0f, 1000.0f);
	m_tParam.m_f3Pos.y -= m_tParam.m_f3Size.y;

	// モデルレンダラーコンポーネントの設定
	CModelRenderer* pModelRenderer = GetComponent<CModelRenderer>();
	pModelRenderer->SetKey("FieldGround");
	pModelRenderer->SetRendererParam(m_tParam);

	// 頂点シェーダーの設定
	VertexShader* pVS = new VertexShader(VSType::Object);
	pModelRenderer->SetVertexShader(pVS);

	// ピクセルシェーダーの設定
	PixelShader* pPS = new PixelShader(PSType::TexColor);
	pModelRenderer->SetPixelShader(pPS);

}
