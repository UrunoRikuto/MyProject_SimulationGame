/**********************************************************************************//*
	@file		|DirectX.h
	@brief		|DirectX初期化、描画関連
*//***********************************************************************************/
#pragma once

#include <d3d11.h>
#include "Defines.h"
#pragma comment(lib, "d3d11.lib")

// 前方宣言
class RenderTarget;
class DepthStencil;

// ブレンドモード
enum BlendMode
{
	BLEND_NONE,
	BLEND_ALPHA,
	BLEND_ADD,
	BLEND_ADDALPHA,
	BLEND_SUB,
	BLEND_SCREEN,
	BLEND_MAX
};

// サンプラーステート
enum SamplerState
{
	SAMPLER_LINEAR,
	SAMPLER_POINT,
	SAMPLER_MAX
};

// デバイスの取得
ID3D11Device* GetDevice();
// デバイスコンテキストの取得
ID3D11DeviceContext* GetContext();
// スワップチェインの取得
IDXGISwapChain* GetSwapChain();
// デフォルトのレンダーターゲットの取得
RenderTarget* GetDefaultRTV();
// デフォルトのデプスステンシルの取得
DepthStencil* GetDefaultDSV();
// DirectXの初期化
HRESULT InitDirectX(HWND hWnd, UINT width, UINT height, bool fullscreen);
// DirectXの終了
void UninitDirectX();
// 描画開始
void BeginDrawDirectX();
// 描画終了
void EndDrawDirectX();
// レンダーターゲット、デプスステンシルの設定
void SetRenderTargets(UINT num, RenderTarget** ppViews, DepthStencil* pView);
// ラスタライザーステートの設定
void SetCullingMode(D3D11_CULL_MODE cull);
// ブレンドモードの設定
void SetBlendMode(BlendMode blend);
// サンプラーステートの設定
void SetSamplerState(SamplerState state);
