/**************************************************//*
	@file	| ModelRenderer.h
	@brief	| モデル描画コンポーネントクラスのhファイル
	@note	| モデル描画を扱う
			| CRendererComponentを継承
*//**************************************************/
#pragma once
#include "RendererComponent.h"
#include "Model.h"
#include "Shader.h"

// @brief モデル描画コンポーネントクラス
class CModelRenderer : public CRendererComponent
{
public:
	// コンストラクタの継承
	using CRendererComponent::CRendererComponent;

	// デストラクタ
	~CModelRenderer();

	// 初期化処理
    void Init() override;

	// 描画処理
	void Draw() override;

	// @brief Mesh情報を取得
	// @return 読み込まれているモデルのメッシュ情報ベクター
	std::vector<Model::Mesh> GetMesh();

	// @brief 深度バッファを使用するかどうかの指定
	// @param isDepth：true:深度バッファ使用 false:深度バッファ不使用
	void SetDepth(bool isDepth) { m_bIsDepth = isDepth; }

	// @brief 頂点シェーダーの設定
	// @param pVS：頂点シェーダーのポインタ
	void SetVertexShader(VertexShader* pVS) { m_pVS = pVS; }

	// @brief ピクセルシェーダーの設定
	// @param pPS：ピクセルシェーダーのポインタ
	void SetPixelShader(PixelShader* pPS) { m_pPS = pPS; }

private:
	// @brief 深度バッファを使用するかどうか
	bool m_bIsDepth;

	// @brief 頂点シェーダー
	VertexShader* m_pVS;

	// @brief ピクセルシェーダー
	PixelShader* m_pPS;

};
