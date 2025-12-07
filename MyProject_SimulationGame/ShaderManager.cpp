/**************************************************//*
	@file	| ShaderManager.cpp
	@brief	| シェーダーマネージャークラスのcppファイル
	@note	| シェーダーの管理を行うクラスを定義
			| シングルトンパターンで作成
*//**************************************************/
#include "ShaderManager.h"

/*****************************************//*
	@brief　	| コンストラクタ
*//*****************************************/
CShaderManager::CShaderManager()
{
}

/*****************************************//*
	@brief　	| デストラクタ
*//*****************************************/
CShaderManager::~CShaderManager()
{
	// 頂点シェーダーの解放
	for (auto& vsPair : m_VSMap)
	{
		delete vsPair.second;
	}
	m_VSMap.clear();
	// ピクセルシェーダーの解放
	for (auto& psPair : m_PSMap)
	{
		delete psPair.second;
	}
	m_PSMap.clear();
}

/*****************************************//*
	@brief　	| シェーダーの読み込み
*//*****************************************/
void CShaderManager::LoadShaders()
{

	//== 頂点シェーダーの読み込み

	// VS_Object
	VertexShader* pVSObj = new(std::nothrow) VertexShader(VSType::Object);
	if (pVSObj != nullptr)
	{
		if (!pVSObj->Load(SHADER_PATH("VS_Object.cso")))
		{
			m_VSMap[VSType::Object] = pVSObj;
		}
	}

	// VS_Sprite
	VertexShader* pVSSprite = new(std::nothrow) VertexShader(VSType::Sprite);
	if (pVSSprite != nullptr)
	{
		if (!pVSSprite->Load(SHADER_PATH("VS_Sprite.cso")))
		{
			m_VSMap[VSType::Sprite] = pVSSprite;
		}
	}
	
	//== ピクセルシェーダーの読み込み

	// PS_TexColor
	PixelShader* pPSTexColor = new(std::nothrow) PixelShader(PSType::TexColor);
	if (pPSTexColor != nullptr)
	{
		if (!pPSTexColor->Load(SHADER_PATH("PS_TexColor.cso")))
		{
			m_PSMap[PSType::TexColor] = pPSTexColor;
		}
	}

	// PS_Sprite
	PixelShader* pPSSprite = new(std::nothrow) PixelShader(PSType::Sprite);
	if (pPSSprite != nullptr)
	{
		if (!pPSSprite->Load(SHADER_PATH("PS_Sprite.cso")))
		{
			m_PSMap[PSType::Sprite] = pPSSprite;
		}
	}
}


/*****************************************//*
	@brief　	| 頂点シェーダーの取得
	@param[in]	| type：頂点シェーダーの種類
*//*****************************************/
VertexShader* CShaderManager::GetVertexShader(VSType type)
{
	auto it = m_VSMap.find(type);
	if (it != m_VSMap.end())
	{
		return it->second;
	}
	return nullptr;
}

/*****************************************//*
	@brief　	| ピクセルシェーダーの取得
	@param[in]	| type：ピクセルシェーダーの種類
*//*****************************************/
PixelShader* CShaderManager::GetPixelShader(PSType type)
{
	auto it = m_PSMap.find(type);
	if (it != m_PSMap.end())
	{
		return it->second;
	}
	return nullptr;
}