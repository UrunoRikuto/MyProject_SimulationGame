/**************************************************//*
	@file	| Terrain.cpp
	@brief	| 地形(起伏)管理システムのcppファイル
	@note	| 起伏のある地面のメッシュを作り、建築可能な土地が増えるたびに、その周りを平らに作り直す
*//**************************************************/
// std::min / std::max をマクロに置き換えられないようにする
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "Terrain.h"
#include "FieldGrid.h"
#include "FbmNoise.h"
#include "RendererComponent.h"
#include <algorithm>
#include <cmath>

namespace
{
	// @brief 地面のモデルのキー
	const char* const Ground_Model_Key = "FieldGround";

	// @brief フィールドの外側の地面の広がり(中心からの距離)
	constexpr float Outer_Extent = 500.0f;

	// @brief 地面のテクスチャ上の色の位置(元のモデルと同じ)
	constexpr DirectX::XMFLOAT2 Ground_UV = { 0.630859f, 0.5f };

	// @brief 陰影の計算に使う光の向き(正規化済みの値は、使うときに求める)
	constexpr DirectX::XMFLOAT3 Light_Direction = { 0.5f, 0.8f, 0.35f };

	// @brief 斜面の陰影の強さ
	constexpr float Shade_Strength = 1.4f;

	// @brief 陰影の明るさの範囲
	constexpr float Shade_Min = 0.55f;
	constexpr float Shade_Max = 1.1f;

	// @brief 0～1に収めた値を、なめらかに変化する値にする
	float SmoothStep(float fT)
	{
		fT = std::clamp(fT, 0.0f, 1.0f);
		return fT * fT * (3.0f - 2.0f * fT);
	}
}

/*****************************************//*
	@brief　	| 地形の状態を初期化する
*//*****************************************/
void CTerrain::Reset()
{
	const size_t nCount = static_cast<size_t>(Vertex_Count_X) * Vertex_Count_Z;
	s_vBaseHeights.assign(nCount, 0.0f);
	s_vFlatness.assign(nCount, 1.0f);
	s_vHeights.assign(nCount, 0.0f);
	s_vVertices.clear();

	s_bGenerated = false;
	s_bMeshReady = false;
	s_bDirty = false;
}

/*****************************************//*
	@brief　	| 地形を作成する
	@param　	| nSeed：地形のシード値
*//*****************************************/
void CTerrain::Generate(unsigned int nSeed)
{
	if (s_vBaseHeights.size() != static_cast<size_t>(Vertex_Count_X) * Vertex_Count_Z) Reset();

	// 地形のノイズは、セルの種類(木・岩など)のノイズとは別のものにする
	FbmNoise noise(nSeed ^ 0x9E3779B9u);
	FbmNoise::Params params;
	params.octaves = 4;
	params.lacunarity = 2.0f;
	params.gain = 0.5f;
	params.frequency = 1.0f;
	params.amplitude = 1.0f;
	params.normalize = true;

	for (int x = 0; x < Vertex_Count_X; ++x)
	{
		for (int z = 0; z < Vertex_Count_Z; ++z)
		{
			float fHeight = noise.noise(static_cast<float>(x) * Noise_Scale, static_cast<float>(z) * Noise_Scale, params) * Max_Height;

			// フィールドの端に近づくほど、起伏をなくす(フィールドの外側の平らな地面とつなげるため)
			const int nEdge = std::min(std::min(x, Vertex_Count_X - 1 - x), std::min(z, Vertex_Count_Z - 1 - z));
			fHeight *= SmoothStep(static_cast<float>(nEdge) / Edge_Fade_Distance);

			s_vBaseHeights[VertexIndex(x, z)] = fHeight;
		}
	}

	s_bGenerated = true;

	// 全ての頂点を、描画用に計算し直す
	s_bDirty = true;
	s_nDirtyMinX = 0;
	s_nDirtyMinZ = 0;
	s_nDirtyMaxX = Vertex_Count_X - 1;
	s_nDirtyMaxZ = Vertex_Count_Z - 1;
}

/*****************************************//*
	@brief　	| 地形を使わない状態にする
*//*****************************************/
void CTerrain::Clear()
{
	s_bGenerated = false;
	s_bMeshReady = false;
	s_bDirty = false;
}

/*****************************************//*
	@brief　	| セルが建築可能な土地になったことを知らせる
	@param　	| nCellX：セルのXインデックス
	@param　	| nCellY：セルのYインデックス
	@note		| セルの周りの頂点を、セルからの距離に応じてなめらかに平らにする
*//*****************************************/
void CTerrain::MarkCellFlat(int nCellX, int nCellY)
{
	if (s_vFlatness.size() != static_cast<size_t>(Vertex_Count_X) * Vertex_Count_Z) Reset();

	const int nRadius = static_cast<int>(std::ceil(Flat_Radius));
	const int nMinX = std::max(nCellX - nRadius, 0);
	const int nMaxX = std::min(nCellX + 1 + nRadius, Vertex_Count_X - 1);
	const int nMinZ = std::max(nCellY - nRadius, 0);
	const int nMaxZ = std::min(nCellY + 1 + nRadius, Vertex_Count_Z - 1);

	for (int x = nMinX; x <= nMaxX; ++x)
	{
		for (int z = nMinZ; z <= nMaxZ; ++z)
		{
			// セル(頂点の座標でnCellX～nCellX+1の四角形)までの距離
			const float fDx = std::max(std::max(static_cast<float>(nCellX - x), static_cast<float>(x - (nCellX + 1))), 0.0f);
			const float fDz = std::max(std::max(static_cast<float>(nCellY - z), static_cast<float>(z - (nCellY + 1))), 0.0f);
			const float fDistance = std::sqrt(fDx * fDx + fDz * fDz);

			float& fFlat = s_vFlatness[VertexIndex(x, z)];
			fFlat = std::min(fFlat, SmoothStep(fDistance / Flat_Radius));
		}
	}

	// 反映待ちの範囲を広げる
	if (!s_bDirty)
	{
		s_bDirty = true;
		s_nDirtyMinX = nMinX;
		s_nDirtyMinZ = nMinZ;
		s_nDirtyMaxX = nMaxX;
		s_nDirtyMaxZ = nMaxZ;
	}
	else
	{
		s_nDirtyMinX = std::min(s_nDirtyMinX, nMinX);
		s_nDirtyMinZ = std::min(s_nDirtyMinZ, nMinZ);
		s_nDirtyMaxX = std::max(s_nDirtyMaxX, nMaxX);
		s_nDirtyMaxZ = std::max(s_nDirtyMaxZ, nMaxZ);
	}
}

/*****************************************//*
	@brief　	| 毎フレームの更新
*//*****************************************/
void CTerrain::Update()
{
	if (!s_bGenerated) return;

	// 地面のモデルを探す
	const RendererObject* pObj = CRendererComponent::FindRendererObject(Ground_Model_Key);
	if (pObj == nullptr) return;
	Model* pModel = std::get<ModelParam>(pObj->m_Data).m_pModel;
	if (pModel == nullptr) return;

	// 最初は、メッシュ全体を作成する
	if (!s_bMeshReady)
	{
		BuildMesh(pModel);
		return;
	}

	if (!s_bDirty) return;

	// 変更があった範囲の頂点を計算し直して、メッシュに反映する
	RecomputeRegion(s_nDirtyMinX, s_nDirtyMinZ, s_nDirtyMaxX, s_nDirtyMaxZ);
	pModel->WriteMeshVertices(0, s_vVertices);
	s_bDirty = false;
}

/*****************************************//*
	@brief　	| 指定した場所の地形の高さを取得する
	@param　	| fX：ワールドのX座標
	@param　	| fZ：ワールドのZ座標
*//*****************************************/
float CTerrain::GetHeight(float fX, float fZ)
{
	if (!s_bGenerated) return 0.0f;

	// フィールドの左下(頂点0)を原点にした、頂点の単位の座標にする
	const float fCellX = CFieldCell::CELL_SIZE.x;
	const float fCellZ = CFieldCell::CELL_SIZE.z;
	const float fVx = (fX + CFieldGrid::GetHalfExtentX()) / fCellX;
	const float fVz = (fZ + CFieldGrid::GetHalfExtentZ()) / fCellZ;

	// フィールドの外は平ら
	if (fVx < 0.0f || fVz < 0.0f || fVx >= static_cast<float>(Vertex_Count_X - 1) || fVz >= static_cast<float>(Vertex_Count_Z - 1)) return 0.0f;

	const int nX = static_cast<int>(fVx);
	const int nZ = static_cast<int>(fVz);
	const float fTx = fVx - static_cast<float>(nX);
	const float fTz = fVz - static_cast<float>(nZ);

	// 4つの頂点の高さをなめらかにつなぐ
	const float fH00 = s_vHeights[VertexIndex(nX, nZ)];
	const float fH10 = s_vHeights[VertexIndex(nX + 1, nZ)];
	const float fH01 = s_vHeights[VertexIndex(nX, nZ + 1)];
	const float fH11 = s_vHeights[VertexIndex(nX + 1, nZ + 1)];
	const float fH0 = fH00 + (fH10 - fH00) * fTx;
	const float fH1 = fH01 + (fH11 - fH01) * fTx;
	return fH0 + (fH1 - fH0) * fTz;
}

/*****************************************//*
	@brief　	| 描画用のメッシュを作成して、モデルに設定する
	@param　	| pModel：地面のモデル
*//*****************************************/
void CTerrain::BuildMesh(Model* pModel)
{
	const int nGridVertexCount = Vertex_Count_X * Vertex_Count_Z;

	// 頂点(フィールドの格子 + 外側の地面の四隅)
	s_vVertices.assign(static_cast<size_t>(nGridVertexCount) + 4, Model::Vertex{});
	const float fOriginX = -CFieldGrid::GetHalfExtentX();
	const float fOriginZ = -CFieldGrid::GetHalfExtentZ();
	for (int x = 0; x < Vertex_Count_X; ++x)
	{
		for (int z = 0; z < Vertex_Count_Z; ++z)
		{
			Model::Vertex& v = s_vVertices[VertexIndex(x, z)];
			v.pos.x = fOriginX + static_cast<float>(x) * CFieldCell::CELL_SIZE.x;
			v.pos.z = fOriginZ + static_cast<float>(z) * CFieldCell::CELL_SIZE.z;
			v.uv = DirectX::XMFLOAT2(Ground_UV.x, Ground_UV.y);
		}
	}
	const float fOuterX[4] = { -Outer_Extent, Outer_Extent, -Outer_Extent, Outer_Extent };
	const float fOuterZ[4] = { -Outer_Extent, -Outer_Extent, Outer_Extent, Outer_Extent };
	for (int i = 0; i < 4; ++i)
	{
		Model::Vertex& v = s_vVertices[static_cast<size_t>(nGridVertexCount) + i];
		v.pos = DirectX::XMFLOAT3(fOuterX[i], 0.0f, fOuterZ[i]);
		v.normal = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
		v.uv = DirectX::XMFLOAT2(Ground_UV.x, Ground_UV.y);
		v.color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	// 高さ・法線・色を計算
	RecomputeRegion(0, 0, Vertex_Count_X - 1, Vertex_Count_Z - 1);

	// インデックス(フィールドの格子 + 外側の地面)
	Model::Indices indices;
	indices.reserve(static_cast<size_t>(Vertex_Count_X - 1) * (Vertex_Count_Z - 1) * 6 + 4 * 6);
	for (int x = 0; x < Vertex_Count_X - 1; ++x)
	{
		for (int z = 0; z < Vertex_Count_Z - 1; ++z)
		{
			const unsigned long v00 = VertexIndex(x, z);
			const unsigned long v10 = VertexIndex(x + 1, z);
			const unsigned long v01 = VertexIndex(x, z + 1);
			const unsigned long v11 = VertexIndex(x + 1, z + 1);
			indices.insert(indices.end(), { v00, v01, v11, v00, v11, v10 });
		}
	}
	// 外側の地面は、フィールドの四隅の頂点と外側の四隅でつなぐ(フィールドの端の高さは0なので、隙間は見えない)
	const unsigned long c00 = VertexIndex(0, 0);
	const unsigned long c10 = VertexIndex(Vertex_Count_X - 1, 0);
	const unsigned long c01 = VertexIndex(0, Vertex_Count_Z - 1);
	const unsigned long c11 = VertexIndex(Vertex_Count_X - 1, Vertex_Count_Z - 1);
	const unsigned long o00 = nGridVertexCount + 0;
	const unsigned long o10 = nGridVertexCount + 1;
	const unsigned long o01 = nGridVertexCount + 2;
	const unsigned long o11 = nGridVertexCount + 3;
	// 手前(z小)・奥(z大)・左(x小)・右(x大)
	// (上から見て、フィールドの格子と同じ時計回りの向きにする。逆向きの面は描画されない)
	indices.insert(indices.end(), { c00, o10, o00, c00, c10, o10 });
	indices.insert(indices.end(), { c01, o11, c11, c01, o01, o11 });
	indices.insert(indices.end(), { c00, o01, c01, c00, o00, o01 });
	indices.insert(indices.end(), { c10, o11, o10, c10, c11, o11 });

	pModel->SetMeshData(0, s_vVertices, indices);

	s_bMeshReady = true;
	s_bDirty = false;
}

/*****************************************//*
	@brief　	| 指定した範囲の頂点の高さ・法線・色を計算し直す
	@param　	| nMinX, nMinZ, nMaxX, nMaxZ：計算し直す頂点の範囲(頂点の番号)
	@note		| 法線は隣の頂点の高さを使うため、範囲の外側1つ分も計算し直す
*//*****************************************/
void CTerrain::RecomputeRegion(int nMinX, int nMinZ, int nMaxX, int nMaxZ)
{
	nMinX = std::max(nMinX, 0);
	nMinZ = std::max(nMinZ, 0);
	nMaxX = std::min(nMaxX, Vertex_Count_X - 1);
	nMaxZ = std::min(nMaxZ, Vertex_Count_Z - 1);

	// 高さ(元の起伏 × 残り具合)
	for (int x = nMinX; x <= nMaxX; ++x)
	{
		for (int z = nMinZ; z <= nMaxZ; ++z)
		{
			const int i = VertexIndex(x, z);
			s_vHeights[i] = s_vBaseHeights[i] * s_vFlatness[i];
		}
	}

	// 法線と陰影の色
	DirectX::XMVECTOR vLight = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&Light_Direction));
	const float fFlatShade = DirectX::XMVectorGetY(vLight);
	const int nStartX = std::max(nMinX - 1, 0);
	const int nStartZ = std::max(nMinZ - 1, 0);
	const int nEndX = std::min(nMaxX + 1, Vertex_Count_X - 1);
	const int nEndZ = std::min(nMaxZ + 1, Vertex_Count_Z - 1);
	for (int x = nStartX; x <= nEndX; ++x)
	{
		for (int z = nStartZ; z <= nEndZ; ++z)
		{
			const int nL = std::max(x - 1, 0), nR = std::min(x + 1, Vertex_Count_X - 1);
			const int nB = std::max(z - 1, 0), nF = std::min(z + 1, Vertex_Count_Z - 1);
			const float fDx = (s_vHeights[VertexIndex(nR, z)] - s_vHeights[VertexIndex(nL, z)]) / (static_cast<float>(nR - nL) * CFieldCell::CELL_SIZE.x);
			const float fDz = (s_vHeights[VertexIndex(x, nF)] - s_vHeights[VertexIndex(x, nB)]) / (static_cast<float>(nF - nB) * CFieldCell::CELL_SIZE.z);

			DirectX::XMVECTOR vNormal = DirectX::XMVector3Normalize(DirectX::XMVectorSet(-fDx, 1.0f, -fDz, 0.0f));
			const float fDot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(vNormal, vLight));
			const float fShade = std::clamp(1.0f + Shade_Strength * (fDot - fFlatShade), Shade_Min, Shade_Max);

			Model::Vertex& v = s_vVertices[VertexIndex(x, z)];
			v.pos.y = s_vHeights[VertexIndex(x, z)];
			DirectX::XMStoreFloat3(&v.normal, vNormal);
			v.color = DirectX::XMFLOAT4(fShade, fShade, fShade, 1.0f);
		}
	}
}
