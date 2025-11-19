/**************************************************//*
	@file	| SpriteRenderer.h
	@brief	| 2Dスプライト描画を扱うコンポーネントクラスのhファイル
	@note	| CRendererComponentを継承
*//**************************************************/
#pragma once
#include "RendererComponent.h"

// @brief 2Dスプライト描画を扱うコンポーネントクラス
class CSpriteRenderer : public CRendererComponent
{
public:
	// コンストラクタの継承
	using CRendererComponent::CRendererComponent;

	// @brief デストラクタ
	virtual ~CSpriteRenderer();

	// @brief 描画処理
	void Draw() override;
};

