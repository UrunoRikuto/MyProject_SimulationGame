/**************************************************//*
	@file	| RefreshFacility.cpp
	@brief	| 休憩施設クラスのcppファイル
	@note	| 休憩施設の処理を実装
			| CBuildObjectを継承
*//**************************************************/
#include "RefreshFacility.h"
#include "ModelRenderer.h"

/*****************************************//*
	@brief　	| コンストラクタ
*//*****************************************/
CRefreshFacility::CRefreshFacility()
	:CBuildObject()
{
	// モデルレンダラーコンポーネントの追加
	AddComponent<CModelRenderer>();
}

/*****************************************//*
	@brief　	| デストラクタ
*//*****************************************/
CRefreshFacility::~CRefreshFacility()
{
}

/*****************************************//*
	@brief　	| 初期化処理
*//*****************************************/
void CRefreshFacility::Init()
{
	// 基底クラスの初期化処理
	CBuildObject::Init();

	// モデルレンダラーコンポーネントの設定
	CModelRenderer* pModelRenderer = GetComponent<CModelRenderer>();
	pModelRenderer->SetKey("RefreshFacility");
	pModelRenderer->SetRendererParam(m_tParam);

	// 頂点シェーダーの設定
	VertexShader* pVS = new VertexShader(VSType::Object);
	pModelRenderer->SetVertexShader(pVS);

	// ピクセルシェーダーの設定
	PixelShader* pPS = new PixelShader(PSType::TexColor);
	pModelRenderer->SetPixelShader(pPS);
}
