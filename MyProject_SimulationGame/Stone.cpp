/**************************************************//*
	@file	| Stone.cpp
	@brief	| 石オブジェクトクラスのcppファイル
	@note	| CGameObjectを継承
*//**************************************************/
#include "Stone.h"
#include "ModelRenderer.h"
#include "Oparation.h"

/*****************************************//*
	@brief　	| コンストラクタ
*//*****************************************/
CStone::CStone()
	:CCollectTarget()
{
	// モデルレンダラーコンポーネントの追加
	AddComponent<CModelRenderer>();
}

/*****************************************//*
	@brief　	| デストラクタ
*//*****************************************/
CStone::~CStone()
{
}

/*****************************************//*
	@brief　	| 初期化処理
*//*****************************************/
void CStone::Init()
{
	// 基底クラスの初期化処理
	CGameObject::Init();

	// モデルレンダラーコンポーネントの設定
	CModelRenderer* pModelRenderer = GetComponent<CModelRenderer>();
	pModelRenderer->SetKey("Stone");
	pModelRenderer->SetRendererParam(m_tParam);

	// 頂点シェーダーの設定
	VertexShader* pVS = new VertexShader(VSType::Object);
	pModelRenderer->SetVertexShader(pVS);

	// ピクセルシェーダーの設定
	PixelShader* pPS = new PixelShader(PSType::TexColor);
	pModelRenderer->SetPixelShader(pPS);
}