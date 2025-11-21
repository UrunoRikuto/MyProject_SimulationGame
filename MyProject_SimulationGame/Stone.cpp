/**************************************************//*
	@file	| Stone.cpp
	@brief	| 石オブジェクトクラスのcppファイル
	@note	| CGameObjectを継承
*//**************************************************/
#include "Stone.h"
#include "ModelRenderer.h"
#include "CollisionObb.h"
#include "Oparation.h"

/*****************************************//*
	@brief　	| コンストラクタ
*//*****************************************/
CStone::CStone()
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

	// (仮)
	float fXPos = GetRandOfRange(-20.0f, 20.0f);
	float fZPos = GetRandOfRange(-20.0f, 20.0f);

	m_tParam.m_f3Pos = { fXPos, m_tParam.m_f3Size.y / 2.0f, fZPos };

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


	// OBBコリジョンコンポーネントの設定
	CCollisionObb* pCollisionObb = GetComponent<CCollisionObb>();
	pCollisionObb->SetTag("Stone");
	pCollisionObb->SetCenter(m_tParam.m_f3Pos);
	pCollisionObb->SetSize(m_tParam.m_f3Size);
}