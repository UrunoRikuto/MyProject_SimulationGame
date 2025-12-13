/**************************************************//*
	@file	| Animal.cpp
	@brief	| 動物基底クラスのcppファイル
	@note	| 動物の基底クラスを実装
			| CEntityを継承
*//**************************************************/
#include "Animal.h"
#include "ShaderManager.h"

/*****************************************//*
	@brief　	| コンストラクタ
*//*****************************************/
CAnimal::CAnimal()
	: CEntity()
	, m_f3Velocity({ 0.0f, 0.0f, 0.0f })
	, m_pActionAI(nullptr)
{
	// モデルレンダラーコンポーネントの追加
	CModelRenderer* pModelRenderer = AddComponent<CModelRenderer>();
	// 頂点シェーダーの設定
	VertexShader* pVS = CShaderManager::GetInstance()->GetVertexShader(VSType::Object);
	pModelRenderer->SetVertexShader(pVS);

	// ピクセルシェーダーの設定
	PixelShader* pPS = CShaderManager::GetInstance()->GetPixelShader(PSType::TexColor);
	pModelRenderer->SetPixelShader(pPS);

}

/*****************************************//*
	@brief　	| デストラクタ
*//*****************************************/
CAnimal::~CAnimal()
{
}