/**********************************************************************************//*
	@file		| MeshBuffer.cpp
	@brief		| メッシュバッファの実装
	@note		| 頂点・インデックス・インスタンスバッファの作成／更新／描画を行う
*//***********************************************************************************/
#include "MeshBuffer.h"

/****************************************//*
	@brief	| コンストラクタ
*//****************************************/
MeshBuffer::MeshBuffer()
	: m_pVtxBuffer(NULL)
	, m_pIdxBuffer(NULL)
	, m_pInstanceBuffer(NULL)
	, m_desc{}
{
}

/****************************************//*
	@brief	| デストラクタ
*//****************************************/
MeshBuffer::~MeshBuffer()
{
	SAFE_DELETE_ARRAY(m_desc.pIdx);
	SAFE_DELETE_ARRAY(m_desc.pVtx);
	SAFE_DELETE_ARRAY(m_desc.pInstance);
	SAFE_RELEASE(m_pIdxBuffer);
	SAFE_RELEASE(m_pVtxBuffer);
	SAFE_RELEASE(m_pInstanceBuffer);
}

/****************************************//*
	@brief	| メッシュバッファの作成
	@param	| desc：メッシュバッファの設定（頂点/インデックス/インスタンス情報）
	@return	| HRESULT（成功:S_OK、失敗:エラーコード）
*//****************************************/
HRESULT MeshBuffer::Create(const Description& desc)
{
	HRESULT hr = E_FAIL;

	// 頂点バッファ作成
	hr = CreateVertexBuffer(desc.pVtx, desc.vtxSize, desc.vtxCount, desc.isWrite);
	if (FAILED(hr)) { return hr; }

	// インデックスバッファ作成（指定があれば）
	if (desc.pIdx) {
		hr = CreateIndexBuffer(desc.pIdx, desc.idxSize, desc.idxCount);
		if (FAILED(hr)) { return hr; }
	}

	// インスタンスバッファが指定されていれば作成
	if (desc.pInstance && desc.instanceCount >0 && desc.instanceSize >0)
	{
		hr = CreateInstanceBuffer(desc.pInstance, desc.instanceSize, desc.instanceCount);
		if (FAILED(hr)) { return hr; }
	}

	// 設定情報を内部にコピー
	m_desc = desc;

	// 頂点/インデックス/インスタンスのデータをローカルにコピーして保持
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
	if (m_desc.pInstance) {
		rsize_t instMemSize = desc.instanceSize * desc.instanceCount;
		void* pInst = new char[instMemSize];
		memcpy_s(pInst, instMemSize, desc.pInstance, instMemSize);
		m_desc.pInstance = pInst;
	}

	return hr;
}

/****************************************//*
	@brief	| メッシュバッファの描画
	@param	| count：描画する頂点/インデックス数（0の場合は内部のカウントを使用）
	@param	| instanceCount：インスタンス数（0の場合は内部のインスタンス数を使用）
*//****************************************/
void MeshBuffer::Draw(int count, int instanceCount)
{
	// デバイスコンテキスト取得
	ID3D11DeviceContext* pContext = GetContext();
	UINT stride = m_desc.vtxSize;
	UINT offset =0;

	// プリミティブトポロジーと頂点バッファをセット
	pContext->IASetPrimitiveTopology(m_desc.topology);
	pContext->IASetVertexBuffers(0,1, &m_pVtxBuffer, &stride, &offset);

	// インスタンスバッファがあればスロット1にセット
	if (m_pInstanceBuffer)
	{
		UINT instStride = m_desc.instanceSize;
		UINT instOffset =0;
		pContext->IASetVertexBuffers(1,1, &m_pInstanceBuffer, &instStride, &instOffset);
	}

	// インデックスバッファありの場合はインデックス描画
	if (m_desc.idxCount >0)
	{
		DXGI_FORMAT format;
		switch (m_desc.idxSize)
		{
		case 4: format = DXGI_FORMAT_R32_UINT; break;
		case 2: format = DXGI_FORMAT_R16_UINT; break;
		default: format = DXGI_FORMAT_R16_UINT; break;
		}
		pContext->IASetIndexBuffer(m_pIdxBuffer, format,0);
		if (m_pInstanceBuffer)
		{
			pContext->DrawIndexedInstanced(count ? count : m_desc.idxCount, instanceCount ? instanceCount : m_desc.instanceCount,0,0,0);
		}
		else
		{
			pContext->DrawIndexed(count ? count : m_desc.idxCount,0,0);
		}
	}
	else
	{
		// インデックスなし：頂点描画
		if (m_pInstanceBuffer)
		{
			pContext->DrawInstanced(count ? count : m_desc.vtxCount, instanceCount ? instanceCount : m_desc.instanceCount,0,0);
		}
		else
		{
			pContext->Draw(count ? count : m_desc.vtxCount,0);
		}
	}

}

/****************************************//*
	@brief	| 頂点バッファへの書き込み（動的バッファ用）
	@param	| pVtx：頂点データへのポインタ
	@return	| HRESULT（成功:S_OK、失敗:エラーコード）
*//****************************************/
HRESULT MeshBuffer::Write(void* pVtx)
{
	if (!m_desc.isWrite) { return E_FAIL; }

	HRESULT hr;
	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pContext = GetContext();
	D3D11_MAPPED_SUBRESOURCE mapResource;

	// バッファをマップしてデータをコピー
	hr = pContext->Map(m_pVtxBuffer,0, D3D11_MAP_WRITE_DISCARD,0, &mapResource);
	if (SUCCEEDED(hr))
	{
		rsize_t size = m_desc.vtxCount * m_desc.vtxSize;
		memcpy_s(mapResource.pData, size, pVtx, size);
		pContext->Unmap(m_pVtxBuffer,0);
	}
	return hr;
}

/****************************************//*
	@brief	| バッファ情報の取得
	@return	| 内部に保持している Description のコピー
*//****************************************/
MeshBuffer::Description MeshBuffer::GetDesc()
{
	return m_desc;
}

/****************************************//*
	@brief	| 頂点バッファの内部作成
	@param	| pVtx：頂点データ
	@param	| size：頂点1つあたりのサイズ（バイト）
	@param	| count：頂点数
	@param	| isWrite：書き込み可能にするか（動的バッファにする）
	@return	| HRESULT（成功:S_OK、失敗:エラーコード）
*//****************************************/
HRESULT MeshBuffer::CreateVertexBuffer(const void* pVtx, UINT size, UINT count, bool isWrite)
{
	// バッファ記述を設定
	D3D11_BUFFER_DESC bufDesc = {};
	bufDesc.ByteWidth = size * count;
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	if (isWrite)
	{
		bufDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}

	// 初期データ
	D3D11_SUBRESOURCE_DATA subResource = {};
	subResource.pSysMem = pVtx;

	// バッファ作成
	HRESULT hr;
	ID3D11Device* pDevice = GetDevice();
	hr = pDevice->CreateBuffer(&bufDesc, &subResource, &m_pVtxBuffer);

	return hr;
}

/****************************************//*
	@brief	| インデックスバッファの作成
	@param	| pIdx：インデックスデータ
	@param	| size：インデックス1つあたりのサイズ（バイト：2または4）
	@param	| count：インデックス数
	@return	| HRESULT（成功:S_OK、失敗:エラーコード）
*//****************************************/
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

	// バッファ記述
	D3D11_BUFFER_DESC bufDesc = {};
	bufDesc.ByteWidth = size * count;
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	// 初期データ
	D3D11_SUBRESOURCE_DATA subResource = {};
	subResource.pSysMem = pIdx;

	// バッファ生成
	ID3D11Device* pDevice = GetDevice();
	HRESULT hr;
	hr = pDevice->CreateBuffer(&bufDesc, &subResource, &m_pIdxBuffer);

	return hr;
}

/****************************************//*
	@brief	| インスタンスバッファの作成
	@param	| pInst：インスタンスデータ
	@param	| size：1インスタンスあたりのサイズ（バイト）
	@param	| count：インスタンス数
	@return	| HRESULT（成功:S_OK、失敗:エラーコード）
*//****************************************/
HRESULT MeshBuffer::CreateInstanceBuffer(const void* pInst, UINT size, UINT count)
{
	if (!pInst || size ==0 || count ==0) return E_FAIL;

	//既存のインスタンスバッファ/コピーデータを解放
	SAFE_RELEASE(m_pInstanceBuffer);
	SAFE_DELETE_ARRAY(m_desc.pInstance);

	D3D11_BUFFER_DESC bufDesc = {};
	bufDesc.ByteWidth = size * count;
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA subResource = {};
	subResource.pSysMem = pInst;

	ID3D11Device* pDevice = GetDevice();
	HRESULT hr = pDevice->CreateBuffer(&bufDesc, &subResource, &m_pInstanceBuffer);
	if (FAILED(hr)) return hr;

	// インスタンスデータを内部にコピーして保持
	rsize_t instMemSize = static_cast<rsize_t>(size) * count;
	m_desc.pInstance = new char[instMemSize];
	memcpy_s(m_desc.pInstance, instMemSize, pInst, instMemSize);
	m_desc.instanceSize = size;
	m_desc.instanceCount = count;

	return S_OK;
}

/****************************************//*
	@brief	| インスタンスバッファの更新
	@param	| pInst：インスタンスデータ
	@param	| size：1インスタンスあたりのサイズ（バイト）
	@param	| count：インスタンス数
	@return	| HRESULT（成功:S_OK、失敗:エラーコード）
*//****************************************/
HRESULT MeshBuffer::UpdateInstanceBuffer(const void* pInst, UINT size, UINT count)
{
	if (!pInst || size ==0 || count ==0) return E_FAIL;

	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pContext = GetContext();

	//既存のインスタンスバッファがあり、容量が十分なら UpdateSubresourceで更新
	if (m_pInstanceBuffer && m_desc.instanceSize == size && m_desc.instanceCount >= count)
	{
		pContext->UpdateSubresource(m_pInstanceBuffer,0, nullptr, pInst, size * count,0);
		// 内部コピーも更新
		if (m_desc.pInstance)
		{
			memcpy_s((void*)m_desc.pInstance, static_cast<rsize_t>(m_desc.instanceSize) * m_desc.instanceCount, pInst, static_cast<rsize_t>(size) * count);
			m_desc.instanceCount = count;
		}
		return S_OK;
	}

	//それ以外はバッファを再作成してコピー
	SAFE_RELEASE(m_pInstanceBuffer);
	SAFE_DELETE_ARRAY(m_desc.pInstance);

	D3D11_BUFFER_DESC bufDesc = {};
	bufDesc.ByteWidth = size * count;
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA subResource = {};
	subResource.pSysMem = pInst;

	HRESULT hr = pDevice->CreateBuffer(&bufDesc, &subResource, &m_pInstanceBuffer);
	if (FAILED(hr)) return hr;

	// 内部コピーを作成
	rsize_t instMemSize = static_cast<rsize_t>(size) * count;
	m_desc.pInstance = new char[instMemSize];
	memcpy_s(m_desc.pInstance, instMemSize, pInst, instMemSize);
	m_desc.instanceSize = size;
	m_desc.instanceCount = count;

	return S_OK;
}