/**************************************************//*
	@file	| TestObject.cpp
	@brief	| テスト用ゲームオブジェクトクラスのcppファイル
	@note	| CGameObjectを継承
*//**************************************************/
#include "TestObject.h"
#include "ModelRenderer.h"
#include "CollisionOBB.h"

/*****************************************//*
	@brief　	| コンストラクタ
*//****************************************/
CTestObject::CTestObject()
	: CGameObject()
{
	// モデルレンダラーコンポーネントの追加
	AddComponent<CModelRenderer>();

	// CollisionOBBコンポーネントの追加
	AddComponent<CCollisionObb>();
}

/*****************************************//*
	@brief　	| デストラクタ
*//****************************************/
CTestObject::~CTestObject()
{
}

/*****************************************//*
	@brief　	| 初期化処理
*//****************************************/
void CTestObject::Init()
{

	// モデルレンダラーコンポーネントの設定
	CModelRenderer* pModel = GetComponent<CModelRenderer>();
	// モデルのキーを設定
	pModel->SetKey("TestModel");
	// 描画用パラメータの設定
	pModel->SetRendererParam(m_tParam);

	// 頂点シェーダーの設定
	VertexShader* pVS = new VertexShader(VSType::Object);
	pModel->SetVertexShader(pVS);

	// ピクセルシェーダーの設定
	PixelShader* pPS = new PixelShader(PSType::TexColor);
	pModel->SetPixelShader(pPS);

	// CollisionOBBコンポーネントの設定
	CCollisionObb* pCollision = GetComponent<CCollisionObb>();
	// コリジョン情報の設定
	pCollision->SetCenter(m_tParam.m_f3Pos);
	pCollision->SetSize(m_tParam.m_f3Size);
}

/*****************************************//*
	@brief　	| 更新処理
*//****************************************/
void CTestObject::Update()
{
}