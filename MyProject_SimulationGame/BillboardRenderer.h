/**************************************************//*
	@file	| BillboardRenderer.h
	@brief	| ビルボード描画を扱うコンポーネントクラスのhファイル
	@note	| CRendererComponentを継承
*//**************************************************/
#pragma once
#include "RendererComponent.h"

// @brief ビルボード描画を扱うコンポーネントクラス
class CBillboardRenderer : public CRendererComponent
{
public: 
	// コンストラクタの継承
	using CRendererComponent::CRendererComponent;

	// @brief デストラクタ
	~CBillboardRenderer();

	// @brief 描画処理
	void Draw() override;
};

