/**********************************************************************************//*
	@file		| MeshBuffer.h
	@brief		| メッシュバッファのhファイル
*//***********************************************************************************/
#pragma once
#include "DirectX.h"

// @brief メッシュバッファ
class MeshBuffer
{
public:
	// @brief メッシュバッファの情報
	struct Description
	{
		const void* pVtx;
		UINT vtxSize;
		UINT vtxCount;
		bool isWrite;
		const void* pIdx;
		UINT idxSize;
		UINT idxCount;
		D3D11_PRIMITIVE_TOPOLOGY topology;
	};
public:
	// @brief コンストラクタ
	MeshBuffer();

	// @brief デストラクタ
	~MeshBuffer();

	// @brief メッシュバッファの作成
	// @param[in] desc メッシュバッファの情報
	// @return HRESULT
	HRESULT Create(const Description& desc);

	// @brief メッシュバッファの描画
	// @param[in] count 描画する頂点数(0を指定した場合はバッファに設定されている頂点数)
	void Draw(int count = 0);

	// @brief 頂点バッファの書き込み
	// @param[in] pVtx 頂点データ
	// @return HRESULT(書き込みに失敗した場合はE_FAIL)
	HRESULT Write(void* pVtx);

	// @brief バッファ情報の取得
	// @return バッファ情報
	Description GetDesc();

private:
	// @brief 頂点バッファの作成
	// @param[in] pVtx 頂点データ
	// @param[in] size 頂点1つあたりのサイズ
	// @param[in] count 頂点数
	// @param[in] isWrite 書き込み可能かどうか
	// @return HRESULT
	HRESULT CreateVertexBuffer(const void* pIdx, UINT size, UINT count, bool isWrite);

	// @brief インデックスバッファの作成
	// @param[in] pVtx 頂点データ
	// @param[in] size インデックス1つあたりのサイズ
	// @param[in] count インデックス数
	// @return HRESULT
	HRESULT CreateIndexBuffer(const void* pVtx, UINT size, UINT count);

private:
	// 頂点バッファ
	ID3D11Buffer* m_pVtxBuffer;
	// インデックスバッファ
	ID3D11Buffer* m_pIdxBuffer;
	// バッファ情報
	Description m_desc;

};