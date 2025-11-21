/**************************************************//*
	@file	| Wood.cpp
	@brief	| 木オブジェクトクラスのcppファイル
	@note	| CGameObjectを継承
*//**************************************************/
#include "Wood.h"
#include "ModelRenderer.h"
#include "CollisionObb.h"
#include "Oparation.h"

/*****************************************//*
	@brief　	| コンストラクタ
	@param　	| In_f3Pos：初期位置
*//*****************************************/
CWood::CWood()
	:CCollectTarget()
{
	// モデルレンダラーコンポーネントの追加
	AddComponent<CModelRenderer>();

	// OBBコリジョンコンポーネントの追加
	AddComponent<CCollisionObb>();
}

/*****************************************//*
	@brief　	| デストラクタ
*//*****************************************/
CWood::~CWood()
{
}

/*****************************************//*
	@brief　	| 初期化処理
*//*****************************************/
void CWood::Init()
{
	// 基底クラスの初期化処理
	CGameObject::Init();

	// モデルレンダラーコンポーネントの設定
	CModelRenderer* pModelRenderer = GetComponent<CModelRenderer>();
	pModelRenderer->SetKey("Wood");
	pModelRenderer->SetRendererParam(m_tParam);

	// 頂点シェーダーの設定
	VertexShader* pVS = new VertexShader(VSType::Object);
	pModelRenderer->SetVertexShader(pVS);

	// ピクセルシェーダーの設定
	PixelShader* pPS = new PixelShader(PSType::TexColor);
	pModelRenderer->SetPixelShader(pPS);


	// OBBコリジョンコンポーネントの設定
	CCollisionObb* pCollisionObb = GetComponent<CCollisionObb>();
	pCollisionObb->SetTag("Wood");
	pCollisionObb->SetCenter(m_tParam.m_f3Pos);
	pCollisionObb->SetSize(m_tParam.m_f3Size);
	pCollisionObb->SetSizeScale({ 1.0f, 2.0f, 1.0f });
}