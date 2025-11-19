/**********************************************************************************//*
	@file		| Geometory.h
	@brief		| 3D図形描画クラスヘッダ
*//***********************************************************************************/
#pragma once
#include <DirectXMath.h>
#include "Shader.h"
#include "MeshBuffer.h"

// @brief 3D図形描画クラス
class Geometory
{
private:
	// @brief 線分描画用頂点構造体
	struct LineVertex
	{
		float pos[3];
		float color[4];
	};
	// @brief 通常描画用頂点構造体
	struct Vertex
	{
		float pos[3];
		float uv[2];
	};
public:
	// @brief 初期化
	static void Init();

	// @brief 終了
	static void Uninit();

	// @brief ワールド行列設定
	// @param[in] world ワールド行列
	static void SetWorld(DirectX::XMFLOAT4X4 world);

	// @brief ビュー行列設定
	// @param[in] view ビュー行列
	static void SetView(DirectX::XMFLOAT4X4 view);

	// @brief プロジェクション行列設定
	// @param[in] proj プロジェクション行列
	static void SetProjection(DirectX::XMFLOAT4X4 proj);

	// @brief 線分追加
	// @param[in] start 線分の始点
	// @param[in] end 線分の終点
	// @param[in] color 線分の色
	static void AddLine(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end,
		DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// @brief 線分描画
	static void DrawLines();

	// @brief 箱描画
	static void DrawBox();

	// @brief 円柱描画
	static void DrawCylinder();

	// @brief 球描画
	static void DrawSphere();

	// @brief 水平四角形描画
	// @param[in] tPoint1 四角形を囲む2点のうちの1点
	// @param[in] tPoint2 四角形を囲む2点のうちの1点
	// @param[in] tColor 四角形の色
	static void DrawHorizontalSquare(DirectX::XMFLOAT3 tPoint1, DirectX::XMFLOAT3 tPoint2, DirectX::XMFLOAT4 tColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	
	// @brief 垂直X四角形描画
	// @param[in] tPoint1 四角形を囲む2点のうちの1点
	// @param[in] tPoint2 四角形を囲む2点のうちの1点
	// @param[in] tColor 四角形の色
	static void DrawVerticalXSquare(DirectX::XMFLOAT3 tPoint1, DirectX::XMFLOAT3 tPoint2, DirectX::XMFLOAT4 tColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	
	// @brief 垂直Z四角形描画
	// @param[in] tPoint1 四角形を囲む2点のうちの1点
	// @param[in] tPoint2 四角形を囲む2点のうちの1点
	// @param[in] tColor 四角形の色
	static void DrawVerticalZSquare(DirectX::XMFLOAT3 tPoint1, DirectX::XMFLOAT3 tPoint2, DirectX::XMFLOAT4 tColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

private:
	// @brief 頂点シェーダ作成
	static void MakeVS();

	// @brief ピクセルシェーダ作成
	static void MakePS();

	// @brief 線分描画用シェーダ作成
	static void MakeLineShader();

	// @brief 線分の頂点バッファ作成
	static void MakeLine();

private:
	// 線分の最大数
	static const int MAX_LINE_NUM = 1000;

	// 円の分割数
	static const int CIRCLE_DETAIL = 16;
private:
	// 各種メッシュバッファ
	static MeshBuffer* m_pBox;
	static MeshBuffer* m_pCylinder;
	static MeshBuffer* m_pSphere;
	static MeshBuffer* m_pLines;
	static Shader* m_pVS;
	static Shader* m_pPS;
	static Shader* m_pLineShader[2];
	static DirectX::XMFLOAT4X4 m_WVP[3];
	static void* m_pLineVtx;
	static int m_lineCnt;
};
