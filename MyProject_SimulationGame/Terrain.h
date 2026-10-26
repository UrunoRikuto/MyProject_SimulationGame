/**************************************************//*
	@file	| Terrain.h
	@brief	| 地形(起伏)管理システムのhファイル
	@note	| フィールドの地面に起伏を持たせ、その高さを取得できるようにする
			| 建築可能な土地とその周りは平らになるように、高さを補正する
			| 地形の高さは、見た目だけに影響する(オブジェクトの座標そのものは変わらず、描画時に高さを足す)
*//**************************************************/
#pragma once
#include <DirectXMath.h>
#include <vector>
#include "Model.h"

// @brief 地形管理クラス
class CTerrain
{
public:
	// @brief 頂点の数(セルの数 + 1)
	static constexpr int Vertex_Count_X = 201;
	static constexpr int Vertex_Count_Z = 201;

	// @brief 起伏の最大の高さ(平らな土地を基準に、盛り上がる・凹む最大の量)
	static constexpr float Max_Height = 6.0f;

	// @brief 起伏の大きさ(値が小さいほどなだらかで、大きな起伏になる)
	static constexpr float Noise_Scale = 0.03f;

	// @brief 建築可能な土地から、起伏が元の高さに戻るまでの距離(頂点の数)
	static constexpr float Flat_Radius = 5.0f;

	// @brief フィールドの端から、起伏が無くなるまでの距離(頂点の数)
	static constexpr float Edge_Fade_Distance = 10.0f;

public:
	// @brief 地形の状態を初期化する(新しいゲームを始めるときに呼ぶ)
	static void Reset();

	// @brief 地形を作成する
	// @param nSeed：地形のシード値
	static void Generate(unsigned int nSeed);

	// @brief 地形を使わない状態にする(ゲームシーンを抜けるときに呼ぶ。高さは常に0になる)
	static void Clear();

	// @brief セルが建築可能な土地になったことを知らせる(そのセルとその周りが平らになる)
	// @param nCellX：セルのXインデックス
	// @param nCellY：セルのYインデックス
	static void MarkCellFlat(int nCellX, int nCellY);

	// @brief 毎フレームの更新(地形が変わっていれば、描画用のメッシュに反映する)
	static void Update();

	// @brief 指定した場所の地形の高さを取得する
	// @param fX：ワールドのX座標
	// @param fZ：ワールドのZ座標
	// @return 地形の高さ(建築可能な土地や、地形を使っていないときは0)
	static float GetHeight(float fX, float fZ);

private:
	// @brief 描画用のメッシュを作成して、モデルに設定する
	static void BuildMesh(Model* pModel);

	// @brief 指定した範囲の頂点の高さ・法線・色を計算し直す
	static void RecomputeRegion(int nMinX, int nMinZ, int nMaxX, int nMaxZ);

	// @brief 頂点の番号を求める
	static int VertexIndex(int nX, int nZ) { return nX * Vertex_Count_Z + nZ; }

private:
	// @brief 地形を使っているかどうか
	inline static bool s_bGenerated = false;
	// @brief 描画用のメッシュを作成済みかどうか
	inline static bool s_bMeshReady = false;
	// @brief 反映待ちの変更があるかどうか
	inline static bool s_bDirty = false;
	// @brief 反映待ちの範囲(頂点の番号)
	inline static int s_nDirtyMinX = 0;
	inline static int s_nDirtyMinZ = 0;
	inline static int s_nDirtyMaxX = 0;
	inline static int s_nDirtyMaxZ = 0;

	// @brief 頂点ごとの、元の起伏の高さ(建築可能な土地による補正の前)
	inline static std::vector<float> s_vBaseHeights;
	// @brief 頂点ごとの、起伏の残り具合(0:完全に平ら 1:元の起伏のまま)
	inline static std::vector<float> s_vFlatness;
	// @brief 頂点ごとの、現在の高さ
	inline static std::vector<float> s_vHeights;
	// @brief 描画用の頂点
	inline static Model::Vertices s_vVertices;
};
