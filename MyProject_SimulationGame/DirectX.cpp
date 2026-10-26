/**********************************************************************************//*
	@file		|DirectX.cpp
	@brief		|DirectX初期化、描画関連
*//***********************************************************************************/
#include "DirectX.h"
#include "Texture.h"
#include "Profiler.h"
#include <algorithm>

//--- グローバル変数
ID3D11Device* g_pDevice;
ID3D11DeviceContext* g_pContext;
IDXGISwapChain* g_pSwapChain;
RenderTarget* g_pRTV;
DepthStencil* g_pDSV;
ID3D11RasterizerState* g_pRasterizerState[3];
ID3D11BlendState* g_pBlendState[BLEND_MAX];
ID3D11SamplerState* g_pSamplerState[SAMPLER_MAX];

//--- GPUの処理時間の計測用(プロファイラが有効な間だけ使用する)
namespace
{
	// @brief 直前に設定したレンダーターゲット・ビューポートの記録(同じ設定の再呼び出しを省くため)
	struct RenderTargetState
	{
		bool m_bValid = false;
		UINT m_nNum = 0;
		ID3D11RenderTargetView* m_pRtvs[4] = {};
		ID3D11DepthStencilView* m_pDsv = nullptr;
		float m_fWidth = 0.0f;
		float m_fHeight = 0.0f;
	};
	RenderTargetState g_tRenderTargetState;

	// @brief 直前に設定したカリングモード(未設定の場合は-1)
	int g_nCullingState = -1;

	// @brief 計測用クエリのリングバッファのサイズ
	constexpr int GPU_QUERY_RING_SIZE = 4;

	ID3D11Query* g_pQueryDisjoint[GPU_QUERY_RING_SIZE] = {};
	ID3D11Query* g_pQueryStart[GPU_QUERY_RING_SIZE] = {};
	ID3D11Query* g_pQueryEnd[GPU_QUERY_RING_SIZE] = {};
	int g_nQueryWrite = 0;
	int g_nQueryRead = 0;
	int g_nQueryPending = 0;
	bool g_bQueryInFrame = false;
	bool g_bQueryFailed = false;

	// @brief 計測用クエリの解放
	void ReleaseGpuQueries()
	{
		for (int i = 0; i < GPU_QUERY_RING_SIZE; ++i)
		{
			SAFE_RELEASE(g_pQueryDisjoint[i]);
			SAFE_RELEASE(g_pQueryStart[i]);
			SAFE_RELEASE(g_pQueryEnd[i]);
		}
	}

	// @brief 計測用クエリの作成
	// @return true:使用できる false:作成できなかった
	bool CreateGpuQueries()
	{
		if (g_bQueryFailed) return false;
		if (g_pQueryDisjoint[0] != nullptr) return true;

		D3D11_QUERY_DESC desc = {};
		for (int i = 0; i < GPU_QUERY_RING_SIZE; ++i)
		{
			desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
			HRESULT hr1 = g_pDevice->CreateQuery(&desc, &g_pQueryDisjoint[i]);
			desc.Query = D3D11_QUERY_TIMESTAMP;
			HRESULT hr2 = g_pDevice->CreateQuery(&desc, &g_pQueryStart[i]);
			HRESULT hr3 = g_pDevice->CreateQuery(&desc, &g_pQueryEnd[i]);
			if (FAILED(hr1) || FAILED(hr2) || FAILED(hr3))
			{
				ReleaseGpuQueries();
				g_bQueryFailed = true;
				return false;
			}
		}
		return true;
	}

	// @brief GPUの計測の開始(フレームの描画の最初に呼ぶ)
	void BeginGpuTimer()
	{
		g_bQueryInFrame = false;
		if (!CProfiler::IsEnabled() || !CreateGpuQueries()) return;

		g_pContext->Begin(g_pQueryDisjoint[g_nQueryWrite]);
		g_pContext->End(g_pQueryStart[g_nQueryWrite]);
		g_bQueryInFrame = true;
	}

	// @brief GPUの計測の終了と、結果の読み取り(フレームの描画の最後に呼ぶ)
	void EndGpuTimer()
	{
		if (!g_bQueryInFrame) return;

		g_pContext->End(g_pQueryEnd[g_nQueryWrite]);
		g_pContext->End(g_pQueryDisjoint[g_nQueryWrite]);
		g_nQueryWrite = (g_nQueryWrite + 1) % GPU_QUERY_RING_SIZE;
		g_nQueryPending++;

		// リングバッファが一杯の場合は、最も古い結果を捨てる
		if (g_nQueryPending > GPU_QUERY_RING_SIZE)
		{
			g_nQueryRead = (g_nQueryRead + 1) % GPU_QUERY_RING_SIZE;
			g_nQueryPending--;
		}

		// 結果が出ているものを読み取る(まだの場合は待たずに、次のフレームで読む)
		while (g_nQueryPending > 0)
		{
			D3D11_QUERY_DATA_TIMESTAMP_DISJOINT disjoint = {};
			if (g_pContext->GetData(g_pQueryDisjoint[g_nQueryRead], &disjoint, sizeof(disjoint), D3D11_ASYNC_GETDATA_DONOTFLUSH) != S_OK) break;

			UINT64 start = 0;
			UINT64 end = 0;
			if (g_pContext->GetData(g_pQueryStart[g_nQueryRead], &start, sizeof(start), D3D11_ASYNC_GETDATA_DONOTFLUSH) != S_OK) break;
			if (g_pContext->GetData(g_pQueryEnd[g_nQueryRead], &end, sizeof(end), D3D11_ASYNC_GETDATA_DONOTFLUSH) != S_OK) break;
			if (!disjoint.Disjoint && disjoint.Frequency > 0)
			{
				CProfiler::GetInstance()->SetGpuFrameTime(static_cast<double>(end - start) * 1000.0 / static_cast<double>(disjoint.Frequency));
			}

			g_nQueryRead = (g_nQueryRead + 1) % GPU_QUERY_RING_SIZE;
			g_nQueryPending--;
		}
	}
}

/*************************//*
@brief  | デバイスの取得
*//*************************/
ID3D11Device* GetDevice()
{
	return g_pDevice;
}

/*************************//*
@brief  | デバイスコンテキストの取得
*//*************************/
ID3D11DeviceContext* GetContext()
{
	return g_pContext;
}

/*************************//*
@brief  | スワップチェインの取得
*//*************************/
IDXGISwapChain* GetSwapChain()
{
	return g_pSwapChain;
}

/*************************//*
@brief  | デフォルトのレンダーターゲットの取得
*//*************************/
RenderTarget* GetDefaultRTV()
{
	return g_pRTV;
}

/*************************//*
@brief  | デフォルトのデプスステンシルの取得
*//*************************/
DepthStencil* GetDefaultDSV()
{
	return g_pDSV;
}

/*************************//*
@brief		| DirectXの初期化
@param[in]	| hWnd：ウインドウハンドル
@param[in]	| width：画面幅
@param[in]	| height：画面高さ
@param[in]	| fullscreen：フルスクリーンモードにするかどうか
@return		| HRESULT
*//*************************/
HRESULT InitDirectX(HWND hWnd, UINT width, UINT height, bool fullscreen)
{
	HRESULT	hr = E_FAIL;
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));						// ゼロクリア
	sd.BufferDesc.Width = width;						// バックバッファの幅
	sd.BufferDesc.Height = height;						// バックバッファの高さ
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// バックバッファフォーマット(R,G,B,A)
	sd.SampleDesc.Count = 1;							// マルチサンプルの数
	sd.BufferDesc.RefreshRate.Numerator = 1000;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// バックバッファの使用方法
	sd.BufferCount = 1;									// バックバッファの数
	sd.OutputWindow = hWnd;								// 関連付けるウインドウ
	sd.Windowed = fullscreen ? FALSE : TRUE;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// ドライバの種類
	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,	// GPUで描画
		D3D_DRIVER_TYPE_WARP,		// 高精度(低速
		D3D_DRIVER_TYPE_REFERENCE,	// CPUで描画
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	UINT createDeviceFlags = 0;
#if defined(_DEBUG)
	// デバッグレイヤーは描画の呼び出しが重くなるため、デバッグビルドのみ有効にする
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// 機能レベル
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,		// DirectX11.1対応GPUレベル
		D3D_FEATURE_LEVEL_11_0,		// DirectX11対応GPUレベル
		D3D_FEATURE_LEVEL_10_1,		// DirectX10.1対応GPUレベル
		D3D_FEATURE_LEVEL_10_0,		// DirectX10対応GPUレベル
		D3D_FEATURE_LEVEL_9_3,		// DirectX9.3対応GPUレベル
		D3D_FEATURE_LEVEL_9_2,		// DirectX9.2対応GPUレベル
		D3D_FEATURE_LEVEL_9_1		// Direct9.1対応GPUレベル
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	D3D_DRIVER_TYPE driverType;
	D3D_FEATURE_LEVEL featureLevel;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; ++driverTypeIndex)
	{
		driverType = driverTypes[driverTypeIndex];
		// デバッグレイヤーが使えない環境(グラフィックスツール未導入など)では、デバッグレイヤーなしで作り直す
		UINT deviceFlags = createDeviceFlags;
		for (int nTry = 0; nTry < 2; ++nTry)
		{
			hr = D3D11CreateDeviceAndSwapChain(
				NULL,					// ディスプレイデバイスのアダプタ（NULLの場合最初に見つかったアダプタ）
				driverType,				// デバイスドライバのタイプ
				NULL,					// ソフトウェアラスタライザを使用する場合に指定する
				deviceFlags,		// デバイスフラグ
				featureLevels,			// 機能レベル
				numFeatureLevels,		// 機能レベル数
				D3D11_SDK_VERSION,		// 
				&sd,					// スワップチェインの設定
				&g_pSwapChain,			// IDXGIDwapChainインタフェース	
				&g_pDevice,				// ID3D11Deviceインタフェース
				&featureLevel,		// サポートされている機能レベル
				&g_pContext);		// デバイスコンテキスト
			if (SUCCEEDED(hr) || !(deviceFlags & D3D11_CREATE_DEVICE_DEBUG)) break;
			deviceFlags &= ~static_cast<UINT>(D3D11_CREATE_DEVICE_DEBUG);
		}
		if (SUCCEEDED(hr)) {
			break;
		}
	}
	if (FAILED(hr)) {
		return hr;
	}

	//--- レンダーターゲット設定
	g_pRTV = new RenderTarget();
	if (FAILED(hr = g_pRTV->CreateFromScreen()))
		return hr;
	g_pDSV = new DepthStencil();
	if (FAILED(hr = g_pDSV->Create(g_pRTV->GetWidth(), g_pRTV->GetHeight(), false)))
		return hr;
	SetRenderTargets(1, &g_pRTV, nullptr);


	//--- カリング設定
	D3D11_RASTERIZER_DESC rasterizer = {};
	D3D11_CULL_MODE cull[] = {
		D3D11_CULL_NONE,
		D3D11_CULL_FRONT,
		D3D11_CULL_BACK
	};
	rasterizer.FillMode = D3D11_FILL_SOLID;
	rasterizer.FrontCounterClockwise = false;
	for (int i = 0; i < 3; ++i)
	{
		rasterizer.CullMode = cull[i];
		hr = g_pDevice->CreateRasterizerState(&rasterizer, &g_pRasterizerState[i]);
		if (FAILED(hr)) { return hr; }
	}
	SetCullingMode(D3D11_CULL_BACK);

	//--- 深度テスト


	//--- アルファブレンディング
	// https://pgming-ctrl.com/directx11/blend/
	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	D3D11_BLEND blend[BLEND_MAX][2] = {
		{D3D11_BLEND_ONE, D3D11_BLEND_ZERO},
		{D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA},
		{D3D11_BLEND_ONE, D3D11_BLEND_ONE},
		{D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_ONE},
		{D3D11_BLEND_ZERO, D3D11_BLEND_INV_SRC_COLOR},
		{D3D11_BLEND_INV_DEST_COLOR, D3D11_BLEND_ONE},
	};
	for (int i = 0; i < BLEND_MAX; ++i)
	{
		blendDesc.RenderTarget[0].SrcBlend = blend[i][0];
		blendDesc.RenderTarget[0].DestBlend = blend[i][1];
		hr = g_pDevice->CreateBlendState(&blendDesc, &g_pBlendState[i]);
		if (FAILED(hr)) { return hr; }
	}
	SetBlendMode(BLEND_ALPHA);

	// サンプラー
	D3D11_SAMPLER_DESC samplerDesc = {};
	D3D11_FILTER filter[] = {
		D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_FILTER_MIN_MAG_MIP_POINT,
	};
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	for (int i = 0; i < SAMPLER_MAX; ++i)
	{
		samplerDesc.Filter = filter[i];
		hr = g_pDevice->CreateSamplerState(&samplerDesc, &g_pSamplerState[i]);
		if (FAILED(hr)) { return hr; }
	}
	SetSamplerState(SAMPLER_LINEAR);

	return S_OK;
}

/*************************//*
@brief  | DirectXの終了
*//*************************/
void UninitDirectX()
{
	SAFE_DELETE(g_pDSV);
	SAFE_DELETE(g_pRTV);

	for (int i = 0; i < SAMPLER_MAX; ++i)
		SAFE_RELEASE(g_pSamplerState[i]);
	for (int i = 0; i < BLEND_MAX; ++i)
		SAFE_RELEASE(g_pBlendState[i]);
	for (int i = 0; i < 3; ++i)
		SAFE_RELEASE(g_pRasterizerState[i]);
	ReleaseGpuQueries();
	if (g_pContext)
		g_pContext->ClearState();
	SAFE_RELEASE(g_pContext);
	if (g_pSwapChain)
		g_pSwapChain->SetFullscreenState(false, NULL);
	SAFE_RELEASE(g_pSwapChain);
	SAFE_RELEASE(g_pDevice);
}

/*************************//*
@brief  | 描画開始
*//*************************/
void BeginDrawDirectX()
{
	BeginGpuTimer();

	// 他の処理で変更されている可能性があるため、フレームの最初は必ず設定し直す
	g_tRenderTargetState.m_bValid = false;
	g_nCullingState = -1;

	float color[4] = { 0.8f, 0.9f, 1.0f, 1.0f };
	g_pRTV->Clear(color);
	g_pDSV->Clear();
}

/*************************//*
@brief  | 描画終了
*//*************************/
void EndDrawDirectX()
{
	EndGpuTimer();
	// 垂直同期を待って表示する(待たないと、画面の更新タイミングとずれて、コマ落ちやティアリングが起きる)
	g_pSwapChain->Present(1, 0);
}

/*************************//*
@brief		| レンダーターゲット、デプスステンシルの設定
@param[in]	| num：レンダーターゲットの数
@param[in]	| ppViews：レンダーターゲットの配列
@param[in]	| pView：デプスステンシル
*//*************************/
void SetRenderTargets(UINT num, RenderTarget** ppViews, DepthStencil* pView)
{
	static ID3D11RenderTargetView* rtvs[4];

	if (num > 4) num = 4;
	for (UINT i = 0; i < num; ++i)
		rtvs[i] = ppViews[i]->GetView();
	ID3D11DepthStencilView* pDsv = pView ? pView->GetView() : nullptr;
	const float fWidth = (float)ppViews[0]->GetWidth();
	const float fHeight = (float)ppViews[0]->GetHeight();

	// 直前と同じ設定の場合は、描画のたびに同じ設定を繰り返さないよう省く
	RenderTargetState& state = g_tRenderTargetState;
	if (state.m_bValid && state.m_nNum == num && state.m_pDsv == pDsv &&
		state.m_fWidth == fWidth && state.m_fHeight == fHeight &&
		std::equal(rtvs, rtvs + num, state.m_pRtvs))
	{
		return;
	}
	state.m_bValid = true;
	state.m_nNum = num;
	std::copy(rtvs, rtvs + num, state.m_pRtvs);
	state.m_pDsv = pDsv;
	state.m_fWidth = fWidth;
	state.m_fHeight = fHeight;

	g_pContext->OMSetRenderTargets(num, rtvs, pDsv);

	// ビューポートの設定
	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.Width = fWidth;
	vp.Height = fHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	g_pContext->RSSetViewports(1, &vp);
}

/*************************//*
@brief		| カリングモードの設定
@param[in]	| cull：カリングモード
*//*************************/
void SetCullingMode(D3D11_CULL_MODE cull)
{
	// 直前と同じ設定の場合は省く
	if (g_nCullingState == (int)cull) return;
	g_nCullingState = (int)cull;

	switch (cull)
	{
	case D3D11_CULL_NONE: g_pContext->RSSetState(g_pRasterizerState[0]); break;
	case D3D11_CULL_FRONT: g_pContext->RSSetState(g_pRasterizerState[1]); break;
	case D3D11_CULL_BACK: g_pContext->RSSetState(g_pRasterizerState[2]); break;
	}
}

/*************************//*
@brief		| 深度テストの設定
@param[in]	| enable：深度テストを有効にするかどうか
*//*************************/
void SetBlendMode(BlendMode blend)
{
	if (blend < 0 || blend >= BLEND_MAX) return;
	FLOAT blendFactor[4] = { D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO };
	g_pContext->OMSetBlendState(g_pBlendState[blend], blendFactor, 0xffffffff);
}

/*************************//*
@brief		| サンプラーステートの設定
@param[in]	| state：サンプラーステート
*//*************************/
void SetSamplerState(SamplerState state)
{
	if (state < 0 || state >= SAMPLER_MAX) return;
	g_pContext->PSSetSamplers(0, 1, &g_pSamplerState[state]);
}