/**************************************************//*
	@file	| BillboardRenderer.cpp
	@brief	| ビルボード描画を扱うコンポーネントクラスのcppファイル
	@note	| CRendererComponentを継承
*//**************************************************/
#include "BillboardRenderer.h"
#include "Sprite.h"
#include "DirectX.h"

/*****************************************//*
	@brief　	| デストラクタ
*//*****************************************/
CBillboardRenderer::~CBillboardRenderer()
{

}

/*****************************************//*
	@brief　	| 描画処理
*//*****************************************/
void CBillboardRenderer::Draw()
{
	// キーが設定されていない時は描画しない
	if (m_sKey.empty()) return;

	// 深度バッファを無効にする 
	RenderTarget* pRTV = GetDefaultRTV();
	SetRenderTargets(1, &pRTV, nullptr);

	// カリングのセット
	SetCullingMode(m_tParam.m_eCulling);

	// 描画用パラメータのセット
	Sprite::SetParam(m_tParam, SpriteKind::Billboard);

	// テクスチャのセット
	Sprite::SetTexture(std::get<Texture*>(m_RendererObjectMap.find(m_sKey.c_str())->second.m_Data));

	// 描画
	Sprite::Draw();
}
