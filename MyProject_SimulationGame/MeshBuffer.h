/**********************************************************************************//*
	@file		| MeshBuffer.h
	@brief		| メッシュバッファのhファイル
*//***********************************************************************************/
#pragma once
#include "DirectX.h"

// メッシュバッファクラス
class MeshBuffer
{
public:
	// @brief メッシュバッファの情報
	struct Description
	{
		// 頂点バッファ情報
		const void* pVtx;
		// 頂点1つあたりのサイズ
		UINT vtxSize;
		// 頂点数
		UINT vtxCount;
		// 書き込み可能かどうか
		bool isWrite;
		// インデックスバッファ情報
		const void* pIdx;
		// インデックス1つあたりのサイズ
		UINT idxSize;
		// インデックス数
		UINT idxCount;
		// プリミティブトポロジー
		D3D11_PRIMITIVE_TOPOLOGY topology;
	};
public:
	// @brief コンストラクタ
	MeshBuffer();

	// @brief デストラクタ
	~MeshBuffer();

	// @brief メッシュバッファの作成
	// @param desc：メッシュバッファの情報
	// @return 生成に成功したかどうか
	HRESULT Create(const Description& desc);

	// @brief メッシュバッファの描画
	// @param count：描画する頂点数(0を指定した場合はバッファに設定されている頂点数)
	void Draw(int count = 0);

	// @brief 頂点バッファの書き込み
	// @param pVtx：頂点データ
	// @return 書き込みに成功したかどうか
	HRESULT Write(void* pVtx);
	
	// @brief バッファ情報の取得
	// @return バッファ情報
	Description GetDesc();

private:
	// @brief 頂点バッファの作成
	// @param pVtx：頂点データ
	// @param size：頂点1つあたりのサイズ
	// @param count：頂点数
	// @param isWrite：書き込み可能かどうか
	// @return 生成に成功したかどうか
	HRESULT CreateVertexBuffer(const void* pIdx, UINT size, UINT count, bool isWrite);

	// @brief インデックスバッファの作成
	// @param pVtx：頂点データ
	// @param size：頂点1つあたりのサイズ
	// @param count：頂点数
	// @return 生成に成功したかどうか
	HRESULT CreateIndexBuffer(const void* pVtx, UINT size, UINT count);

private:
	// @brief 頂点バッファ
	ID3D11Buffer* m_pVtxBuffer;
	// @brief インデックスバッファ
	ID3D11Buffer* m_pIdxBuffer;
	// @brief バッファ情報
	Description m_desc;

};