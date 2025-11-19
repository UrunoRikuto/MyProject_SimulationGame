/**************************************************//*
	@file	| RendererComponent.h
	@brief	| レンダラー用の仲介コンポーネントクラス
*//**************************************************/
#pragma once
#include "Component.h"
#include "Texture.h"
#include "Model.h"
#include <variant>

// @brief 描画するオブジェクトの種類
enum class RendererKind
{
	// テクスチャ
	Texture,	

	// モデル
	Model,		
};

// @brief オブジェクトの汎用パラメータ
struct RendererParam
{
	// 位置
	DirectX::XMFLOAT3 m_f3Pos;

	// サイズ
	DirectX::XMFLOAT3 m_f3Size;

	// 回転
	DirectX::XMFLOAT3 m_f3Rotate;

	// 色
	DirectX::XMFLOAT4 m_f4Color;

	// UV座標
	DirectX::XMFLOAT2 m_f2UVPos;

	// UVサイズ
	DirectX::XMFLOAT2 m_f2UVSize;

	// カリングモード
	D3D11_CULL_MODE m_eCulling;
};

// @brief モデル用の使用パラメータ
struct ModelParam
{
	// モデル本体のポインタ
    Model* m_pModel;

	// メッシュ情報の配列
    std::vector<Model::Mesh> m_tMeshVec;
};

// @brief 描画するオブジェクトの情報
struct RendererObject
{
	// オブジェクトの種類
	RendererKind m_eKind;

	// オブジェクトのデータ(テクスチャorモデルパラメータ)
    std::variant<Texture*, ModelParam> m_Data;
};

// @brief レンダラー用の仲介コンポーネントクラス
class CRendererComponent : public CComponent
{
public:
	// @brief コンストラクタ
	using CComponent::CComponent;

	// @brief デストラクタ
	virtual ~CRendererComponent();

	// 各種パラメータの設定
	
	// @brief 描画用のパラメータをまとめて設定
	// @param inParam：描画用パラメータをまとめた構造体
	void SetRendererParam(const RendererParam inParam);
	
	// @brief 表示するローカル座標を設定
	// @param inPos：設定する座標の値(DirectX::XMFLOAT3)
	void SetPos(const DirectX::XMFLOAT3 inPos);
	
	// @brief 表示する2D座標を設定
	// @param inPos：設定する座標の値(DirectX::XMFLOAT2)
	void SetPos(const DirectX::XMFLOAT2 inPos);

	// @brief 表示するローカルサイズを設定
	// @param inSize：設定するサイズの値(DirectX::XMFLOAT3)
	void SetSize(const DirectX::XMFLOAT3 inSize);

	// @brief 表示する2Dサイズを設定
	// @param inSize：設定するサイズの値(DirectX::XMFLOAT2)
	void SetSize(const DirectX::XMFLOAT2 inSize);
	
	// @brief 表示するローカル回転を設定
	// @param inRotation：設定する回転の値(DirectX::XMFLOAT3)
	void SetRotation(const DirectX::XMFLOAT3 inRotation);
	
	// @brief 表示する2D回転を設定
	// @param inRotation：設定する回転の値(float)
	void SetRotation(const float inRotation);

	// @brief 表示する色を設定
	// @param inColor：設定する色の値(DirectX::XMFLOAT4)
	void SetColor(const DirectX::XMFLOAT4 inColor);

	// @brief UV座標の位置を設定
	// @param inUVPos：設定するUV座標の位置(DirectX::XMFLOAT2)
	void SetUVPos(const DirectX::XMFLOAT2 inUVPos);

	// @brief UV座標のサイズを設定
	// @param inUVSize：設定するUV座標のサイズ(DirectX::XMFLOAT2)
	void SetUVSize(const DirectX::XMFLOAT2 inUVSize);

	// @brief 表現技法の変更
	// @param inCulling：カリングモード
	// @param [D3D11_CULL_NONE：カリングしない(表裏どちらも描画)
	// @param [D3D11_CULL_FRONT：表面カリング(裏面のみ描画)
	// @param [D3D11_CULL_BACK：裏面カリング(表面のみ描画)
	void SetCullingMode(const D3D11_CULL_MODE inCulling);

	// @brief 描画用のパラメータを取得
	// @return 描画用パラメータをまとめた構造体
	RendererParam GetRendererParam();

	// @brief モデル・テクスチャをロードし、キー位置に登録する
	// @param inKind：モデルorテクスチャ
	// @param inPath：テクスチャのパス
	// @param inKey：登録するキー
	// @param scale：モデルのスケール(scale倍)
	// @param flip：モデルのフリップ
	static void Load(const RendererKind inKind, const char* inPath, std::string inKey, const float scale = 1.0f, const Model::Flip flip = Model::Flip::None);

	// @brief 登録した全てのモデル・テクスチャを解放する
	static void UnLoad();

	// @brief 描画に使用するオブジェクトのキーをセット
	// @param inKey：ロードする際に同時に登録したキー
	void SetKey(std::string inKey);

protected:

	// @brief レンダラーの統合パラメータ
	RendererParam m_tParam;
	
	// @brief 読み込んだオブジェクトをキー位置に保存するマップ
	static std::map<std::string, RendererObject> m_RendererObjectMap;

	// @brief 呼び出し用のキー
	std::string m_sKey;

};
