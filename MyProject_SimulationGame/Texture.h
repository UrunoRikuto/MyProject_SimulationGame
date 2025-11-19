/**********************************************************************************//*
	@file		| Texture.h
	@brief		| テクスチャ
*//***********************************************************************************/
#pragma once
#include "DirectX.h"

// @brief テクスチャクラス
class Texture
{
public:
	// @brief コンストラクタ
	Texture();

	// @brief デストラクタ
	virtual ~Texture();

	// @brief ファイルからテクスチャを生成
	// @param[in] fileName ファイル名
	// @return 成功したらS_OK
	HRESULT Create(const char* fileName);

	// @brief 空のテクスチャを生成
	// @param[in] format ピクセルフォーマット
	// @param[in] width 横幅
	// @param[in] height 縦幅
	// @param[in] pData 初期データ
	// @return 成功したらS_OK
	HRESULT Create(DXGI_FORMAT format, UINT width, UINT height, const void* pData = nullptr);

	// @brief 横幅を取得
	// @return 横幅
	UINT GetWidth() const;

	// @brief 縦幅を取得
	// @return 縦幅
	UINT GetHeight() const;

	// @brief シェーダリソースビューを取得
	// @return シェーダリソースビュー
	ID3D11ShaderResourceView* GetResource() const;

protected:
	// @brief テクスチャ記述子を生成
	// @param[in] format ピクセルフォーマット
	// @param[in] width 横幅
	// @param[in] height 縦幅
	// @return テクスチャ記述子
	D3D11_TEXTURE2D_DESC MakeTexDesc(DXGI_FORMAT format, UINT width, UINT height);

	// @brief テクスチャリソースを生成
	// @param[in] desc テクスチャ記述子
	// @param[in] pData 初期データ
	// @return 成功したらS_OK
	virtual HRESULT CreateResource(D3D11_TEXTURE2D_DESC &desc, const void* pData);

protected:
	// 2Dテクスチャの横幅
	UINT m_width;
	UINT m_height;
	ID3D11ShaderResourceView *m_pSRV;
	ID3D11Texture2D* m_pTex;
};

// @brief レンダーターゲット
class RenderTarget : public Texture
{
public:
	// @brief コンストラクタ
	RenderTarget();

	// @brief デストラクタ
	~RenderTarget();

	// @brief クリア
	void Clear();

	// @brief クリア
	// @param[in] color クリアカラー
	void Clear(const float* color);

	// @brief 空のレンダーターゲットを生成
	// @param[in] format ピクセルフォーマット
	// @param[in] width 横幅
	// @param[in] height 縦幅
	// @return 成功したらS_OK
	HRESULT Create(DXGI_FORMAT format, UINT width, UINT height);

	// @brief スクリーンからレンダーターゲットを生成
	// @return 成功したらS_OK
	HRESULT CreateFromScreen();

	// @brief レンダーターゲットビューを取得
	// @return レンダーターゲットビュー
	ID3D11RenderTargetView* GetView() const;

protected:
	// @brief テクスチャリソースを生成
	// @param[in] desc テクスチャ記述子
	// @param[in] pData 初期データ
	// @return 成功したらS_OK
	virtual HRESULT CreateResource(D3D11_TEXTURE2D_DESC& desc, const void* pData = nullptr);

private:
	// レンダーターゲットビュー
	ID3D11RenderTargetView* m_pRTV;
};

// @brief デプスステンシル
class DepthStencil : public Texture
{
public:
	// @brief コンストラクタ
	DepthStencil();

	// @brief デストラクタ
	~DepthStencil();

	// @brief クリア
	void Clear();

	// @brief 空のデプスステンシルを生成
	// @param[in] width 横幅
	// @param[in] height 縦幅
	// @param[in] useStencil ステンシルを使用するか
	// @return 成功したらS_OK
	HRESULT Create(UINT width, UINT height, bool useStencil);

	// @brief デプスステンシルビューを取得
	// @return デプスステンシルビュー
	ID3D11DepthStencilView* GetView() const;

protected:
	// @brief テクスチャリソースを生成
	// @param[in] desc テクスチャ記述子
	// @param[in] pData 初期データ
	// @return 成功したらS_OK
	virtual HRESULT CreateResource(D3D11_TEXTURE2D_DESC& desc, const void* pData = nullptr);

private:
	// デプスステンシルビュー
	ID3D11DepthStencilView* m_pDSV;
};
