/**************************************************//*
	@file	| TestObject.cpp
	@brief	| テスト用ゲームオブジェクトクラスのcppファイル
	@note	| CGameObjectを継承
*//**************************************************/
#include "TestObject.h"
#include "ModelRenderer.h"

/*****************************************//*
	@brief　	| コンストラクタ
*//****************************************/
CTestObject::CTestObject()
	: CGameObject()
{
	// モデルレンダラーコンポーネントの追加
	AddComponent<CModelRenderer>();
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
}

/*****************************************//*
	@brief　	| 更新処理
*//****************************************/
void CTestObject::Update()
{
}