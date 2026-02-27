/**********************************************************************************//*
	@file		| MeshBuffer.h
	@brief		| メッシュバッファのヘッダファイル
	@note		| 頂点/インデックス/インスタンスバッファの管理と描画を行うクラス定義
*//***********************************************************************************/
#pragma once
#include "DirectX.h"

// メッシュバッファクラス
class MeshBuffer
{
public:
	// @brief メッシュバッファの記述情報
	struct Description
	{
		// 頂点バッファの先頭データ
		const void* pVtx;
		// 頂点1つあたりのバイトサイズ
		UINT vtxSize;
		// 頂点数
		UINT vtxCount;
		// 頂点バッファが書き込み可能かどうか
		bool isWrite;
		// インデックスバッファの先頭データ
		const void* pIdx;
		// インデックス1つあたりのサイズ（バイト）
		UINT idxSize;
		// インデックス数
		UINT idxCount;
		// プリミティブトポロジー（例: D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST）
		D3D11_PRIMITIVE_TOPOLOGY topology;

		// --- インスタンシング用 ---
		// インスタンス初期データ（Create時に渡す）
		void* pInstance; // 内部でコピー等を行うため非constにしている
		//1インスタンスあたりのバイトサイズ
		UINT instanceSize;
		// インスタンス数
		UINT instanceCount;
	};
public:
	// @brief コンストラクタ
	MeshBuffer();

	// @brief デストラクタ
	~MeshBuffer();

	// @brief メッシュバッファの作成
	// @param desc：メッシュバッファの記述情報
	// @return HRESULT（成功: S_OKなど）
	HRESULT Create(const Description& desc);

	// @brief メッシュバッファの描画
	// @param count：描画する頂点数（0の場合はバッファに設定されている頂点数を使用）
	// @param instanceCount：インスタンス数（デフォルト0でインスタンシング無し）
	void Draw(int count =0, int instanceCount =0);

	// @brief 頂点バッファへの書き込み
	// @param pVtx：頂点データへのポインタ
	// @return HRESULT（成功: S_OKなど）
	HRESULT Write(void* pVtx);
	
	// @brief 設定済みのバッファ情報を取得
	// @return Description構造体のコピー
	Description GetDesc();

	// @brief インスタンスバッファの作成
	// @param pInst：インスタンス初期データ
	// @param size：1インスタンスあたりのバイトサイズ
	// @param count：インスタンス数
	// @return HRESULT（成功: S_OKなど）
	HRESULT CreateInstanceBuffer(const void* pInst, UINT size, UINT count);

	// @brief インスタンスバッファの更新（既存バッファへデータを上書き）
	// @param pInst：インスタンスデータ
	// @param size：1インスタンスあたりのバイトサイズ
	// @param count：インスタンス数
	// @return HRESULT（成功: S_OKなど）
	HRESULT UpdateInstanceBuffer(const void* pInst, UINT size, UINT count);

private:
	// @brief 頂点バッファの内部作成
	// @param pVtx：頂点データ
	// @param size：頂点1つあたりのサイズ（バイト）
	// @param count：頂点数
	// @param isWrite：書き込み可能にするか
	// @return HRESULT（成功: S_OKなど）
	HRESULT CreateVertexBuffer(const void* pVtx, UINT size, UINT count, bool isWrite);

	// @brief インデックスバッファの内部作成
	// @param pIdx：インデックスデータ
	// @param size：インデックス1つあたりのサイズ（バイト）
	// @param count：インデックス数
	// @return HRESULT（成功: S_OKなど）
	HRESULT CreateIndexBuffer(const void* pIdx, UINT size, UINT count);

private:
	// @brief 頂点バッファ
	ID3D11Buffer* m_pVtxBuffer;
	// @brief インデックスバッファ
	ID3D11Buffer* m_pIdxBuffer;
	// @brief インスタンスバッファ
	ID3D11Buffer* m_pInstanceBuffer;
	// @brief バッファ記述情報
	Description m_desc;

};