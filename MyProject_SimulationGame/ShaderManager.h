/**************************************************//*
	@file	| ShaderManager.h
	@brief	| シェーダーマネージャークラスのhファイル
	@note	| シェーダーの管理を行うクラスを定義
			| シングルトンパターンで作成
*//**************************************************/
#pragma once
#include "Singleton.h"
#include "Shader.h"

// @brief シェーダーマネージャークラス
class CShaderManager : public ISingleton<CShaderManager>
{
private:
	// @brief コンストラクタ
	CShaderManager();

	friend class ISingleton<CShaderManager>;
public:
	// @brief デストラクタ
	~CShaderManager();
	
	// @brief シェーダーの読み込み
	void LoadShaders();

	// @brief 頂点シェーダーの取得
	// @param[in] type：頂点シェーダーの種類
	VertexShader* GetVertexShader(VSType type);

	// @brief ピクセルシェーダーの取得
	// @param[in] type：ピクセルシェーダーの種類
	PixelShader* GetPixelShader(PSType type);


private:

	// @brief 頂点シェーダー格納用マップ
	std::map<VSType, VertexShader*> m_VSMap;
	// @brief ピクセルシェーダー格納用マップ
	std::map<PSType, PixelShader*> m_PSMap;
};

