/**********************************************************************************//*
	@file		| Texture.cpp
	@brief		| テクスチャ
*//***********************************************************************************/
#include "Texture.h"
#include "DirectXTex/TextureLoad.h"

/*************************//*
@brief  | コンストラクタ
*//*************************/
Texture::Texture()
	: m_width(0), m_height(0)
	, m_pTex(nullptr)
	, m_pSRV(nullptr)
{
}

/*************************//*
@brief  | デストラクタ
*//*************************/
Texture::~Texture()
{
	SAFE_RELEASE(m_pSRV);
	SAFE_RELEASE(m_pTex);
}

/*************************//*
@brief		| ファイルからテクスチャを生成
@param[in]	| fileName：ファイル名
@return		| 成功したらS_OK
*//*************************/
HRESULT Texture::Create(const char* fileName)
{
	HRESULT hr = S_OK;

	// 文字変換
	wchar_t wPath[MAX_PATH];
	size_t wLen = 0;
	MultiByteToWideChar(0, 0, fileName, -1, wPath, MAX_PATH);

	// ファイル別読み込み
	DirectX::TexMetadata mdata;
	DirectX::ScratchImage image;
	if (strstr(fileName, ".tga"))
		hr = DirectX::LoadFromTGAFile(wPath, &mdata, image);
	else
		hr = DirectX::LoadFromWICFile(wPath, DirectX::WIC_FLAGS::WIC_FLAGS_NONE, &mdata, image);
	if (FAILED(hr)) {
		return E_FAIL;
	}

	// シェーダリソース生成
	hr = CreateShaderResourceView(GetDevice(), image.GetImages(), image.GetImageCount(), mdata, &m_pSRV);
	if (SUCCEEDED(hr))
	{
		m_width = (UINT)mdata.width;
		m_height = (UINT)mdata.height;
	}
	return hr;
}

/*************************//*
@brief		| 空のテクスチャを生成
@param[in]	| format：ピクセルフォーマット
@param[in]	| width：横幅
@param[in]	| height：縦幅
@param[in]	| pData：初期データ
@return		| 成功したらS_OK
*//*************************/
HRESULT Texture::Create(DXGI_FORMAT format, UINT width, UINT height, const void* pData)
{
	D3D11_TEXTURE2D_DESC desc = MakeTexDesc(format, width, height);
	return CreateResource(desc, pData);
}

/*************************//*
@brief  | 横幅を取得
@return | 横幅
*//*************************/
UINT Texture::GetWidth() const
{
	return m_width;
}

/*************************//*
@brief  | 縦幅を取得
@return | 縦幅
*//*************************/
UINT Texture::GetHeight() const
{
	return m_height;
}

/*************************//*
@brief  | シェーダリソースビューを取得
@return | シェーダリソースビュー
*//*************************/
ID3D11ShaderResourceView* Texture::GetResource() const
{
	return m_pSRV;
}

/*************************//*
@brief		| テクスチャ記述子を生成
@param[in]	| format：ピクセルフォーマット
@param[in]	| width：横幅
@param[in]	| height：縦幅
@return		| テクスチャ記述子
*//*************************/
D3D11_TEXTURE2D_DESC Texture::MakeTexDesc(DXGI_FORMAT format, UINT width, UINT height)
{
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.Format = format;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	return desc;
}

/*************************//*
@brief		| テクスチャリソースを生成
@param[in]	| desc：テクスチャ記述子
@param[in]	| pData：初期データ
@return		| 成功したらS_OK
*//*************************/
HRESULT Texture::CreateResource(D3D11_TEXTURE2D_DESC& desc, const void* pData)
{
	HRESULT hr = E_FAIL;

	// テクスチャ作成
	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = pData;
	data.SysMemPitch = desc.Width * 4;
	hr = GetDevice()->CreateTexture2D(&desc, pData ? &data : nullptr, &m_pTex);
	if (FAILED(hr)) { return hr; }

	// 設定
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	switch (desc.Format)
	{
	default:						srvDesc.Format = desc.Format;			break;
	case DXGI_FORMAT_R32_TYPELESS: 	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;	break;
	}
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	// 生成
	hr = GetDevice()->CreateShaderResourceView(m_pTex, &srvDesc, &m_pSRV);
	if (SUCCEEDED(hr))
	{
		m_width = desc.Width;
		m_height = desc.Height;
	}
	return hr;
}

/*************************//*
@brief	| コンストラクタ
*//*************************/
RenderTarget::RenderTarget()
	: m_pRTV(nullptr)
{
}

/*************************//*
@brief	| デストラクタ
*//*************************/
RenderTarget::~RenderTarget()
{
	SAFE_RELEASE(m_pRTV);
}

/*************************//*
@brief  | クリア
*//*************************/
void RenderTarget::Clear()
{
	static float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	Clear(color);
}

/*************************//*
@brief		| クリア
@param[in]	| color：クリアカラー
*//*************************/
void RenderTarget::Clear(const float* color)
{
	GetContext()->ClearRenderTargetView(m_pRTV, color);
}

/*************************//*
@brief		| 空のレンダーターゲットを生成
@param[in]	| format：ピクセルフォーマット
@param[in]	| width：横幅
@param[in]	| height：縦幅
@return		| 成功したらS_OK
*//*************************/
HRESULT RenderTarget::Create(DXGI_FORMAT format, UINT width, UINT height)
{
	D3D11_TEXTURE2D_DESC desc = MakeTexDesc(format, width, height);
	desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
	return CreateResource(desc);
}

/*************************//*
@brief	| スクリーンからレンダーターゲットを生成
@return	| 成功したらS_OK
*//*************************/
HRESULT RenderTarget::CreateFromScreen()
{
	HRESULT hr;

	// バックバッファのポインタを取得
	ID3D11Texture2D* pBackBuffer = NULL;
	hr = GetSwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&m_pTex);
	if (FAILED(hr)) { return hr; }

	// バックバッファへのポインタを指定してレンダーターゲットビューを作成
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvDesc.Texture2D.MipSlice = 0;
	hr = GetDevice()->CreateRenderTargetView(m_pTex, &rtvDesc, &m_pRTV);
	if (SUCCEEDED(hr))
	{
		D3D11_TEXTURE2D_DESC desc;
		m_pTex->GetDesc(&desc);
		m_width = desc.Width;
		m_height = desc.Height;
	}
	return hr;
}

/*************************//*
@brief  | レンダーターゲットビューを取得
@return | レンダーターゲットビュー
*//*************************/
ID3D11RenderTargetView* RenderTarget::GetView() const
{
	return m_pRTV;
}

/*************************//*
@brief		| テクスチャリソースを生成
@param[in]	| desc：テクスチャ記述子
@param[in]	| pData：初期データ
@return		| 成功したらS_OK
*//*************************/
HRESULT RenderTarget::CreateResource(D3D11_TEXTURE2D_DESC& desc, const void* pData)
{
	// テクスチャリソース作成
	HRESULT hr = Texture::CreateResource(desc, nullptr);
	if (FAILED(hr)) { return hr; }

	// 設定
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = desc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	// 生成
	return GetDevice()->CreateRenderTargetView(m_pTex, &rtvDesc, &m_pRTV);
}

/*************************//*
@brief  | コンストラクタ
*//*************************/
DepthStencil::DepthStencil()
	: m_pDSV(nullptr)
{
}

/*************************//*
@brief  | デストラクタ
*//*************************/
DepthStencil::~DepthStencil()
{
	SAFE_RELEASE(m_pDSV);
}

/*************************//*
@brief  | クリア
*//*************************/
void DepthStencil::Clear()
{
	GetContext()->ClearDepthStencilView(m_pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

/*************************//*
@brief		| 空のデプスステンシルを生成
@param[in]	| width：横幅
@param[in]	| height：縦幅
@param[in]	| useStencil：ステンシルを使用するか
@return		| 成功したらS_OK
*//*************************/
HRESULT DepthStencil::Create(UINT width, UINT height, bool useStencil)
{
	// https://docs.microsoft.com/ja-jp/windows/win32/direct3d11/d3d10-graphics-programming-guide-depth-stencil#compositing
	D3D11_TEXTURE2D_DESC desc = MakeTexDesc(useStencil ? DXGI_FORMAT_R24G8_TYPELESS : DXGI_FORMAT_R32_TYPELESS, width, height);
	desc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
	return CreateResource(desc);
}

/*************************//*
@brief  | デプスステンシルビューを取得
@return | デプスステンシルビュー
*//*************************/
ID3D11DepthStencilView* DepthStencil::GetView() const
{
	return m_pDSV;
}

/*************************//*
@brief		| テクスチャリソースを生成
@param[in]	| desc：テクスチャ記述子
@param[in]	| pData：初期データ
@return		| 成功したらS_OK
*//*************************/
HRESULT DepthStencil::CreateResource(D3D11_TEXTURE2D_DESC& desc, const void* pData)
{
	// ステンシル使用判定
	bool useStencil = (desc.Format == DXGI_FORMAT_R24G8_TYPELESS);

	// リソース生成
	desc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
	HRESULT hr = Texture::CreateResource(desc, nullptr);
	if (FAILED(hr)) { return hr; }

	// 設定
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = useStencil ? DXGI_FORMAT_D24_UNORM_S8_UINT : DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	// 生成
	return GetDevice()->CreateDepthStencilView(m_pTex, &dsvDesc, &m_pDSV);
}