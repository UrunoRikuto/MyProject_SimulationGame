/**********************************************************************************//*
	@file		| MeshBuffer.cpp
	@brief		| メッシュバッファのcpp
*//***********************************************************************************/
#include "MeshBuffer.h"

/*************************//*
@brief  | コンストラクタ
*//*************************/
MeshBuffer::MeshBuffer()
	: m_pVtxBuffer(NULL)
	, m_pIdxBuffer(NULL)
	, m_desc{}
{
}

/*************************//*
@brief  | デストラクタ
*//*************************/
MeshBuffer::~MeshBuffer()
{
	SAFE_DELETE_ARRAY(m_desc.pIdx);
	SAFE_DELETE_ARRAY(m_desc.pVtx);
	SAFE_RELEASE(m_pIdxBuffer);
	SAFE_RELEASE(m_pVtxBuffer);
}

/*************************//*
@brief		| メッシュバッファの作成
@param[in]	| desc：メッシュバッファの情報
@return		| HRESULT
*//*************************/
HRESULT MeshBuffer::Create(const Description& desc)
{
	HRESULT hr = E_FAIL;

	// 頂点バッファ作成
	hr = CreateVertexBuffer(desc.pVtx, desc.vtxSize, desc.vtxCount, desc.isWrite);
	if (FAILED(hr)) { return hr; }

	// インデックスバッファ作成
	if (desc.pIdx) {
		hr = CreateIndexBuffer(desc.pIdx, desc.idxSize, desc.idxCount);
		if (FAILED(hr)) { return hr; }
	}

	// バッファ情報のコピー
	m_desc = desc;

	// 頂点、インデックスの情報をコピー
	rsize_t vtxMemSize = desc.vtxSize * desc.vtxCount;
	void* pVtx = new char[vtxMemSize];
	memcpy_s(pVtx, vtxMemSize, desc.pVtx, vtxMemSize);
	m_desc.pVtx = pVtx;
	if (m_desc.pIdx) {
		rsize_t idxMemSize = desc.idxSize * desc.idxCount;
		void* pIdx = new char[idxMemSize];
		memcpy_s(pIdx, idxMemSize, desc.pIdx, idxMemSize);
		m_desc.pIdx = pIdx;
	}

	return hr;
}

/*************************//*
@brief		| メッシュバッファの描画
@param[in]	| count：描画する頂点数、インデックス数(0を指定した場合はバッファの数)
*//*************************/
void MeshBuffer::Draw(int count)
{
	ID3D11DeviceContext* pContext = GetContext();
	UINT stride = m_desc.vtxSize;
	UINT offset = 0;

	pContext->IASetPrimitiveTopology(m_desc.topology);
	pContext->IASetVertexBuffers(0, 1, &m_pVtxBuffer, &stride, &offset);

	// 描画
	if (m_desc.idxCount > 0)
	{
		DXGI_FORMAT format;
		switch (m_desc.idxSize)
		{
		case 4: format = DXGI_FORMAT_R32_UINT; break;
		case 2: format = DXGI_FORMAT_R16_UINT; break;
		}
		pContext->IASetIndexBuffer(m_pIdxBuffer, format, 0);
		pContext->DrawIndexed(count ? count : m_desc.idxCount, 0, 0);
	}
	else
	{
		// 頂点バッファのみで描画
		pContext->Draw(count ? count : m_desc.vtxCount, 0);
	}

}

/*************************//*
@brief		| 頂点バッファの書き込み
@param[in]	| pVtx：頂点データ
@return		| HRESULT(書き込みに失敗した場合はE_FAIL)
*//*************************/
HRESULT MeshBuffer::Write(void* pVtx)
{
	if (!m_desc.isWrite) { return E_FAIL; }

	HRESULT hr;
	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pContext = GetContext();
	D3D11_MAPPED_SUBRESOURCE mapResource;

	// データコピー
	hr = pContext->Map(m_pVtxBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
	if (SUCCEEDED(hr))
	{
		rsize_t size = m_desc.vtxCount * m_desc.vtxSize;
		memcpy_s(mapResource.pData, size, pVtx, size);
		pContext->Unmap(m_pVtxBuffer, 0);
	}
	return hr;
}

/*************************//*
@brief  | バッファ情報の取得
@return | バッファ情報
*//*************************/
MeshBuffer::Description MeshBuffer::GetDesc()
{
	return m_desc;
}

/*************************//*
@brief		| 頂点バッファの作成
@param[in]	| pVtx 頂点データ
@param[in]	| size 頂点1つあたりのサイズ
@param[in]	| count 頂点数
@param[in]	| isWrite 書き込み可能かどうか
@return		| HRESULT
*//*************************/
HRESULT MeshBuffer::CreateVertexBuffer(const void* pVtx, UINT size, UINT count, bool isWrite)
{
	//--- 作成するバッファの情報
	D3D11_BUFFER_DESC bufDesc = {};
	bufDesc.ByteWidth = size * count;
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	if (isWrite)
	{
		bufDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}

	//--- バッファの初期値を設定
	D3D11_SUBRESOURCE_DATA subResource = {};
	subResource.pSysMem = pVtx;

	//--- 頂点バッファの作成
	HRESULT hr;
	ID3D11Device* pDevice = GetDevice();
	hr = pDevice->CreateBuffer(&bufDesc, &subResource, &m_pVtxBuffer);

	return hr;
}

/*************************//*
@brief		| インデックスバッファの作成
@param[in]	| pIdx インデックスデータ
@param[in]	| size インデックス1つあたりのサイズ(2 or 4)
@param[in]	| count インデックス数
@return		| HRESULT
*//*************************/
HRESULT MeshBuffer::CreateIndexBuffer(const void* pIdx, UINT size, UINT count)
{
	// インデックスサイズの確認
	switch (size)
	{
	default:
		return E_FAIL;
	case 2:
	case 4:
		break;
	}

	// バッファの情報を設定
	D3D11_BUFFER_DESC bufDesc = {};
	bufDesc.ByteWidth = size * count;
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	// バッファの初期データ
	D3D11_SUBRESOURCE_DATA subResource = {};
	subResource.pSysMem = pIdx;

	// インデックスバッファ生成
	ID3D11Device* pDevice = GetDevice();
	HRESULT hr;
	hr = pDevice->CreateBuffer(&bufDesc, &subResource, &m_pIdxBuffer);

	return hr;
}