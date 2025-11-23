/**************************************************//*
	@file	| Wood.cpp
	@brief	| 木オブジェクトクラスのcppファイル
	@note	| CGameObjectを継承
*//**************************************************/
#include "Wood.h"
#include "ModelRenderer.h"
#include "Oparation.h"

constexpr float WOOD_INITIAL_HP = 50.0f; // 木の初期耐久値

/*****************************************//*
	@brief　	| コンストラクタ
	@param　	| In_f3Pos：初期位置
*//*****************************************/
CWood::CWood()
	:CCollectTarget()
{
	// モデルレンダラーコンポーネントの追加
	AddComponent<CModelRenderer>();
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

	// 木の耐久値設定
	m_Status.m_fHp = WOOD_INITIAL_HP;
	m_Status.m_fMaxHp = WOOD_INITIAL_HP;
}