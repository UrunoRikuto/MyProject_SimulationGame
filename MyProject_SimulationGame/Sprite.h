/**********************************************************************************//*
    @file		| Sprite.h
    @brief		| スプライト描画
*//***********************************************************************************/
#pragma once
#include <DirectXMath.h>
#include <memory>
#include "MeshBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "RendererComponent.h"

// @brief スプライトの種類
enum class SpriteKind
{
    Screen,
    World,
    Billboard,

    Max
};

// @brief スプライト描画クラス
class Sprite
{
public:
    // @brief 初期化
	static void Init();

    // @brief 終了
	static void Uninit();

    // @brief 描画
	static void Draw();

    // @brief オフセット設定
    // @param offset：オフセット値
	static void SetOffset(DirectX::XMFLOAT2 offset);

    // @brief サイズ設定
    // @param size：サイズ値
	static void SetSize(DirectX::XMFLOAT2 size);

    // @brief UV座標設定
    // @param pos：UV座標
	static void SetUVPos(DirectX::XMFLOAT2 pos);

    // @brief UVスケール設定
    // @param scale：UVスケール
	static void SetUVScale(DirectX::XMFLOAT2 scale);

    // @brief 色設定
    // @param color：色値
	static void SetColor(DirectX::XMFLOAT4 color);

    // @brief テクスチャ設定
    // @param tex：テクスチャ
	static void SetTexture(Texture* tex);

    // @brief ワールド行列設定
    // @param world：ワールド行列
	static void SetWorld(DirectX::XMFLOAT4X4 world);

    // @brief ビュー行列設定
    // @param view：ビュー行列
	static void SetView(DirectX::XMFLOAT4X4 view);

    // @brief プロジェクション行列設定
    // @param proj：プロジェクション行列
	static void SetProjection(DirectX::XMFLOAT4X4 proj);

    // @brief 頂点シェーダー設定
    // @param vs：頂点シェーダー
	static void SetVertexShader(Shader* vs);

    // @brief ピクセルシェーダー設定
    // @param ps：ピクセルシェーダー
	static void SetPixelShader(Shader* ps);
 
    // @brief パラメーター設定
    // @param param：レンダラー用パラメーター
	// @param inKind：スプライトの種類
	static void SetParam(RendererParam param, SpriteKind inKind);
private:
    // スプライト描画用データ
	struct Data
	{
        // メッシュバッファ
		std::shared_ptr<MeshBuffer> mesh;

        // ワールド、ビュー、プロジェクション行列
		DirectX::XMFLOAT4X4 matrix[3];

        // パラメーター設定
        // 0:オフセット、サイズ
        // 1:UV座標、UVスケール
        // 2:色
		DirectX::XMFLOAT4 param[3];

        // テクスチャ情報
		Texture* texture;

        // 頂点シェーダー
		Shader* vs;

        // ピクセルシェーダー
		Shader* ps;
	};
    // スプライト描画用データ
	static Data m_data;
    // デフォルト頂点シェーダー
	static std::shared_ptr<VertexShader> m_defVS;
    // デフォルトピクセルシェーダー
	static std::shared_ptr<PixelShader> m_defPS;

};
