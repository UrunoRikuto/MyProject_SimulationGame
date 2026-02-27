/**********************************************************************************//*
	@file		| MeshBuffer.cpp
	@brief		| メッシュバッファのcpp
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
	// ローカルで保持しているコピーを解放
	SAFE_DELETE_ARRAY(m_desc.pIdx);
	SAFE_DELETE_ARRAY(m_desc.pVtx);
	SAFE_DELETE_ARRAY(m_desc.pInstance);
	// GPU 側バッファを解放
	SAFE_RELEASE(m_pIdxBuffer);
	SAFE_RELEASE(m_pVtxBuffer);
	SAFE_RELEASE(m_pInstanceBuffer);
}

/****************************************//*
	@brief		| メッシュバッファの作成
	@param		| desc：メッシュバッファの情報
	@return		|生成に成功したかどうか (HRESULT)
*//****************************************/
HRESULT MeshBuffer::Create(const Description& desc)
{
	HRESULT hr = E_FAIL;

	// 頂点バッファ作成: 描画される頂点データを GPU に作成
	// CreateVertexBuffer は desc.isWrite に応じて動的バッファを作成します。
	hr = CreateVertexBuffer(desc.pVtx, desc.vtxSize, desc.vtxCount, desc.isWrite);
	if (FAILED(hr)) { return hr; }

	// インデックスバッファ作成: インデックスが指定されていれば作成
	if (desc.pIdx) {
		hr = CreateIndexBuffer(desc.pIdx, desc.idxSize, desc.idxCount);
		if (FAILED(hr)) { return hr; }
	}

	// インスタンスバッファが指定されていれば作成
	// インスタンス描画では別スロットにインスタンス用バッファをセットする想定
	if (desc.pInstance && desc.instanceCount >0 && desc.instanceSize >0)
	{
		hr = CreateInstanceBuffer(desc.pInstance, desc.instanceSize, desc.instanceCount);
		if (FAILED(hr)) { return hr; }
	}

	// バッファ情報のコピー: ローカルに保持しておくことで Draw / Update に使える
	m_desc = desc;

	// 頂点、インデックス、インスタンスの情報をローカルにコピーして保持
	//これにより、外部のメモリが解放されても参照を保持できる
	rsize_t vtxMemSize = static_cast<rsize_t>(desc.vtxSize) * desc.vtxCount;
	void* pVtx = new char[vtxMemSize];
	memcpy_s(pVtx, vtxMemSize, desc.pVtx, vtxMemSize);
	m_desc.pVtx = pVtx;
	if (m_desc.pIdx) {
		rsize_t idxMemSize = static_cast<rsize_t>(desc.idxSize) * desc.idxCount;
		void* pIdx = new char[idxMemSize];
		memcpy_s(pIdx, idxMemSize, desc.pIdx, idxMemSize);
		m_desc.pIdx = pIdx;
	}
	if (m_desc.pInstance) {
		rsize_t instMemSize = static_cast<rsize_t>(desc.instanceSize) * desc.instanceCount;
		void* pInst = new char[instMemSize];
		memcpy_s(pInst, instMemSize, desc.pInstance, instMemSize);
		m_desc.pInstance = pInst;
	}

	return hr;
}

/****************************************//*
	@brief		| メッシュバッファの描画
	@param		| count：描画する頂点/インデックス数 (0 を指定した場合は m_desc の値を使う)
	@param		| instanceCount：描画するインスタンス数 (0 を指定した場合は m_desc の値を使う)
*//****************************************/
void MeshBuffer::Draw(int count, int instanceCount)
{
	// デバイスコンテキスト取得
	ID3D11DeviceContext* pContext = GetContext();

	// 頂点ストライド(1頂点あたりのバイト数) とオフセット
	UINT stride = m_desc.vtxSize;
	UINT offset =0;

	// プリミティブトポロジーを設定
	pContext->IASetPrimitiveTopology(m_desc.topology);

	// スロット0 に頂点バッファをバインド
	// IASetVertexBuffers( StartSlot, NumBuffers, ppVertexBuffers, pStrides, pOffsets )
	pContext->IASetVertexBuffers(0,1, &m_pVtxBuffer, &stride, &offset);

	// インスタンスバッファがあればスロット1にセット
	// シェーダ側でインスタンス用のセマンティクス（例: INSTPOS 等）を定義し、
	// 入力レイアウトで slot1 を受け取る必要があります。
	if (m_pInstanceBuffer)
	{
		UINT instStride = m_desc.instanceSize; // インスタンス1つあたりのバイト数
		UINT instOffset =0;
		// スロット1にインスタンス用バッファをバインド
		pContext->IASetVertexBuffers(1,1, &m_pInstanceBuffer, &instStride, &instOffset);
	}

	// 描画: インデックスがある場合はインデックスバッファをセットして DrawIndexed 系を呼ぶ
	if (m_desc.idxCount >0)
	{
		// インデックスのフォーマットを設定（2バイト = R16、4バイト = R32）
		DXGI_FORMAT format;
		switch (m_desc.idxSize)
		{
		case4: format = DXGI_FORMAT_R32_UINT; break;
		case2: format = DXGI_FORMAT_R16_UINT; break;
		default: format = DXGI_FORMAT_R16_UINT; break; // 想定外は16bitにフォールバック
		}

		// IA にインデックスバッファをセット
		pContext->IASetIndexBuffer(m_pIdxBuffer, format,0);

		if (m_pInstanceBuffer)
		{
			// インスタンス化されたインデックス描画
			// DrawIndexedInstanced( IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation )
			UINT useIndexCount = count ? static_cast<UINT>(count) : m_desc.idxCount;
			UINT useInstanceCount = instanceCount ? static_cast<UINT>(instanceCount) : m_desc.instanceCount;
			pContext->DrawIndexedInstanced(useIndexCount, useInstanceCount,0,0,0);
		}
		else
		{
			// 非インスタンス化のインデックス描画
			UINT useIndexCount = count ? static_cast<UINT>(count) : m_desc.idxCount;
			pContext->DrawIndexed(useIndexCount,0,0);
		}
	}
	else
	{
		// 頂点バッファのみで描画
		if (m_pInstanceBuffer)
		{
			// インスタンス化された頂点描画
			UINT useVertexCount = count ? static_cast<UINT>(count) : m_desc.vtxCount;
			UINT useInstanceCount = instanceCount ? static_cast<UINT>(instanceCount) : m_desc.instanceCount;
			pContext->DrawInstanced(useVertexCount, useInstanceCount,0,0);
		}
		else
		{
			// 単純な頂点描画
			UINT useVertexCount = count ? static_cast<UINT>(count) : m_desc.vtxCount;
			pContext->Draw(useVertexCount,0);
		}
	}

}

/****************************************//*
	@brief		| 頂点バッファの書き込み
	@param		| pVtx：頂点データ
	@return		| 書き込みに成功したかどうか (HRESULT)
*//****************************************/
HRESULT MeshBuffer::Write(void* pVtx)
{
	if (!m_desc.isWrite) { return E_FAIL; }

	HRESULT hr;
	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pContext = GetContext();
	D3D11_MAPPED_SUBRESOURCE mapResource;

	// データコピー: Mapしてデータを書き込む。サイズは vtxCount * vtxSize
	// D3D11_MAP_WRITE_DISCARD を使用することでドライバが新しいバッファを割り当てる場合があり、
	// 同期待ちを最小化できます（古いデータは使われなくなります）。
	hr = pContext->Map(m_pVtxBuffer,0, D3D11_MAP_WRITE_DISCARD,0, &mapResource);
	if (SUCCEEDED(hr))
	{
		rsize_t size = static_cast<rsize_t>(m_desc.vtxCount) * m_desc.vtxSize;
		// mapResource.pData にコピー
		memcpy_s(mapResource.pData, size, pVtx, size);
		// Unmapして変更を確定
		pContext->Unmap(m_pVtxBuffer,0);
	}
	return hr;
}

/****************************************//*
	@brief		| バッファ情報の取得
	@return		| バッファ情報 (Description)
*//****************************************/
MeshBuffer::Description MeshBuffer::GetDesc()
{
	return m_desc;
}

/****************************************//*
	@brief		| 頂点バッファの作成
	@param		| pVtx：頂点データ
	@param		| size：頂点1つあたりのサイズ
	@param		| count：頂点数
	@param		| isWrite：書き込み可能かどうか
	@return		|生成に成功したかどうか (HRESULT)
*//****************************************/
HRESULT MeshBuffer::CreateVertexBuffer(const void* pVtx, UINT size, UINT count, bool isWrite)
{
	//--- 作成するバッファの情報
	D3D11_BUFFER_DESC bufDesc = {};
	bufDesc.ByteWidth = size * count;
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	if (isWrite)
	{
		// 動的に書き換える予定がある場合は Usage と CPU アクセスを変更
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

/****************************************//*
	@brief		| インデックスバッファの作成
	@param		| pIdx：インデックスデータ
	@param		| size：インデックス1つあたりのサイズ（2 または4）
	@param		| count：インデックス数
	@return		|生成に成功したかどうか (HRESULT)
*//****************************************/
HRESULT MeshBuffer::CreateIndexBuffer(const void* pIdx, UINT size, UINT count)
{
	// インデックスサイズの確認
	switch (size)
	{
	default:
		return E_FAIL;
	case2:
	case4:
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

/****************************************//*
	@brief		| インスタンスバッファの作成
	@param		| pInst：インスタンスデータ
	@param		| size：インスタンス1つあたりのサイズ
	@param		| count：インスタンス数
	@return		|生成に成功したかどうか (HRESULT)
*//****************************************/
HRESULT MeshBuffer::CreateInstanceBuffer(const void* pInst, UINT size, UINT count)
{
	if (!pInst || size ==0 || count ==0) return E_FAIL;

	//既存のインスタンスバッファとローカルコピーを解放
	SAFE_RELEASE(m_pInstanceBuffer);
	SAFE_DELETE_ARRAY(m_desc.pInstance);

	D3D11_BUFFER_DESC bufDesc = {};
	bufDesc.ByteWidth = size * count;
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // インスタンスは頂点ストリームとしてバインドする
	D3D11_SUBRESOURCE_DATA subResource = {};
	subResource.pSysMem = pInst;

	ID3D11Device* pDevice = GetDevice();
	HRESULT hr = pDevice->CreateBuffer(&bufDesc, &subResource, &m_pInstanceBuffer);
	if (FAILED(hr)) return hr;

	// インスタンスデータをローカルにコピーして保持
	rsize_t instMemSize = static_cast<rsize_t>(size) * count;
	m_desc.pInstance = new char[instMemSize];
	memcpy_s(m_desc.pInstance, instMemSize, pInst, instMemSize);
	m_desc.instanceSize = size;
	m_desc.instanceCount = count;

	return S_OK;
}

/****************************************//*
	@brief		| インスタンスバッファの更新
	@param		| pInst：インスタンスデータ
	@param		| size：インスタンス1つあたりのサイズ
	@param		| count：インスタンス数
	@return		| 更新に成功したかどうか (HRESULT)
*//****************************************/
HRESULT MeshBuffer::UpdateInstanceBuffer(const void* pInst, UINT size, UINT count)
{
	if (!pInst || size ==0 || count ==0) return E_FAIL;

	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pContext = GetContext();

	//既存のインスタンスバッファがあり、サイズが一致し容量が足りる場合は UpdateSubresource を使う
	if (m_pInstanceBuffer && m_desc.instanceSize == size && m_desc.instanceCount >= count)
	{
		// UpdateSubresourceで既存バッファの先頭から書き換える（高速）
		pContext->UpdateSubresource(m_pInstanceBuffer,0, nullptr, pInst, size * count,0);
		// ローカルコピーも更新（既存領域に上書き）
		if (m_desc.pInstance)
		{
			memcpy_s((void*)m_desc.pInstance, static_cast<rsize_t>(m_desc.instanceSize) * m_desc.instanceCount, pInst, static_cast<rsize_t>(size) * count);
			// ローカルで保持する instanceCount を更新
			m_desc.instanceCount = count;
		}
		return S_OK;
	}

	//ここに来たら新しいサイズが必要なので古いものを解放して再作成
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

	// 新しいインスタンスデータをローカルコピーとして保持
	rsize_t instMemSize = static_cast<rsize_t>(size) * count;
	m_desc.pInstance = new char[instMemSize];
	memcpy_s(m_desc.pInstance, instMemSize, pInst, instMemSize);
	m_desc.instanceSize = size;
	m_desc.instanceCount = count;

	return S_OK;
}

