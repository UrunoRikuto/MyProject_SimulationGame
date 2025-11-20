/**************************************************//*
	@file	| Human.cpp
	@brief	| 人間オブジェクトクラスのcppファイル
	@note	| 人間オブジェクトに関する処理の実装
			| CGameObjectを継承
*//**************************************************/
#include "Human.h"
#include "ModelRenderer.h"
#include "CollisionObb.h"

/****************************************//*
	@brief　	| コンストラクタ
*//****************************************/
CHuman::CHuman()
{
	// モデルレンダラーコンポーネントの追加
	AddComponent<CModelRenderer>();

	// OBBコリジョンコンポーネントの追加
	AddComponent<CCollisionObb>();
}

/****************************************//*
	@brief　	| デストラクタ
*//****************************************/
CHuman::~CHuman()
{
}

/****************************************//*
	@brief　	| 初期化処理
*//****************************************/
void CHuman::Init()
{
	// 基底クラスの初期化処理
	CGameObject::Init();

	// モデルレンダラーコンポーネントの設定
	CModelRenderer* pModelRenderer = GetComponent<CModelRenderer>();
	pModelRenderer->SetKey("Human");
	pModelRenderer->SetRendererParam(m_tParam);

	// 頂点シェーダーの設定
	VertexShader* pVS = new VertexShader();
	pVS->Load(SHADER_PATH("VS_Object.cso"));
	pModelRenderer->SetVertexShader(pVS);
	// ピクセルシェーダーの設定
	PixelShader* pPS = new PixelShader();
	pPS->Load(SHADER_PATH("PS_TexColor.cso"));
	pModelRenderer->SetPixelShader(pPS);


	// OBBコリジョンコンポーネントの設定
	CCollisionObb* pCollisionObb = GetComponent<CCollisionObb>();
	pCollisionObb->SetTag("Human");
	pCollisionObb->SetCenter(m_tParam.m_f3Pos);
	pCollisionObb->SetSize(m_tParam.m_f3Size);
	pCollisionObb->SetSizeScale({ 1.0f, 2.0f, 1.0f });
}

/****************************************//*
	@brief　	| 終了処理
*//****************************************/
void CHuman::Uninit()
{
	// 基底クラスの終了処理
	CGameObject::Uninit();
}

/****************************************//*
	@brief　	| 更新処理
*//****************************************/
void CHuman::Update()
{
	// 基底クラスの更新処理
	CGameObject::Update();
}

/****************************************//*
	@brief　	| 描画処理
*//****************************************/
void CHuman::Draw()
{
	// 基底クラスの描画処理
	CGameObject::Draw();
}