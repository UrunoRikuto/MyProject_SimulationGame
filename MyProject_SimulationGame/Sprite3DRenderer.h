/**************************************************//*
	@file	| Sprite3DRenderer.h
	@brief	| 3Dスプライト描画を扱うコンポーネントクラスのhファイル
	@note	| CRendererComponentを継承
*//**************************************************/
#pragma once
#include "RendererComponent.h"

// @brief 3Dスプライト描画を扱うコンポーネントクラス
class CSprite3DRenderer : public CRendererComponent
{
public:
	// コンストラクタの継承
	using CRendererComponent::CRendererComponent;

	// @brief デストラクタ
	~CSprite3DRenderer();

	// @brief 初期化処理
	void Init() override;

	// @brief 描画処理
	void Draw() override;
	
	// @brief 深度バッファを使用するかどうかの指定
	// @param isDepth：true:深度バッファ使用 false:深度バッファ不使用
	void SetDepth(bool isDepth) { m_bIsDepth = isDepth; }

private:
	// @brief 深度バッファを使用するかどうか
	bool m_bIsDepth;

};
