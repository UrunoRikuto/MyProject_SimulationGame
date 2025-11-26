/**************************************************//*
	@file	| ShaderManager.h
	@brief	| シェーダーマネージャークラスのhファイル
	@note	| シェーダーの管理を行うクラスを定義
			| シングルトンパターンで作成
*//**************************************************/
#pragma once
#include "Shader.h"

// @brief シェーダーマネージャークラス
class CShaderManager
{
private:
	// @brief コンストラクタ
	CShaderManager();
	// コピーコンストラクタ・代入演算子の禁止
	CShaderManager(const CShaderManager&) = delete;
	CShaderManager& operator=(const CShaderManager&) = delete;
public:
	// @brief デストラクタ
	~CShaderManager();
	// @brief シングルトンインスタンスを取得する関数
	static CShaderManager* GetInstance();
	// @brief シングルトンインスタンスを解放する関数
	static void ReleaseInstance();
	
	// @brief シェーダーの読み込み
	void LoadShaders();

	// @brief 頂点シェーダーの取得
	// @param[in] type：頂点シェーダーの種類
	VertexShader* GetVertexShader(VSType type);

	// @brief ピクセルシェーダーの取得
	// @param[in] type：ピクセルシェーダーの種類
	PixelShader* GetPixelShader(PSType type);


private:
	// @brief シングルトンインスタンス
	static CShaderManager* m_Instance;

	// @brief 頂点シェーダー格納用マップ
	std::map<VSType, VertexShader*> m_VSMap;
	// @brief ピクセルシェーダー格納用マップ
	std::map<PSType, PixelShader*> m_PSMap;
};

