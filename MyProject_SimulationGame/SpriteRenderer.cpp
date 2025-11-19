/**************************************************//*
	@file	| SpriteRenderer.cpp
	@brief	| 2Dスプライト描画を扱うコンポーネントクラスのcppファイル
	@note   | CRendererComponentを継承
*//**************************************************/
#include "SpriteRenderer.h"
#include "Sprite.h"
#include "DirectX.h"

/****************************************//*
	@brief　	| デストラクタ
*//****************************************/
CSpriteRenderer::~CSpriteRenderer()
{

}

/****************************************//*
    @brief　	| 描画処理
*//****************************************/
void CSpriteRenderer::Draw()
{
    // キーが設定されていない時は描画しない
    if (m_sKey.empty()) return;

    // 深度バッファを無効にする 
    RenderTarget* pRTV = GetDefaultRTV();
    SetRenderTargets(1, &pRTV, nullptr);

    // カリングのセット
    SetCullingMode(m_tParam.m_eCulling);

    // 描画用パラメータのセット
    Sprite::SetParam(m_tParam, SpriteKind::Screen);

    // テクスチャのセット
    Sprite::SetTexture(std::get<Texture*>(m_RendererObjectMap.find(m_sKey.c_str())->second.m_Data));

    // 描画
    Sprite::Draw();
}
