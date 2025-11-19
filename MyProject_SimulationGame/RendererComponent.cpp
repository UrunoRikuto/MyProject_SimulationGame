/**************************************************//*
	@file	| RendererComponent.cpp
	@brief	| レンダラー用の仲介コンポーネントクラス
*//**************************************************/
#include "RendererComponent.h"

// 静的変数の初期化
std::map<std::string, RendererObject> CRendererComponent::m_RendererObjectMap = {};

/****************************************//*
	@brief　	| デストラクタ
*//****************************************/
CRendererComponent::~CRendererComponent()
{

}

/****************************************//*
	@brief　	| 描画用のパラメータをまとめて設定
	@param　	| inParam：描画用パラメータをまとめた構造体
*//****************************************/
void CRendererComponent::SetRendererParam(const RendererParam inParam)
{
	m_tParam = inParam;
}

/****************************************//*
	@brief　	| 表示するローカル座標を設定
	@param　	| inPos：設定する座標の値(DirectX::XMFLOAT3)
*//****************************************/
void CRendererComponent::SetPos(const DirectX::XMFLOAT3 inPos)
{
	m_tParam.m_f3Pos = inPos;
}

/****************************************//*
	@brief　	| 表示する2D座標を設定
	@param　	| inPos：設定する座標の値(DirectX::XMFLOAT2)
*//****************************************/
void CRendererComponent::SetPos(const DirectX::XMFLOAT2 inPos)
{
	m_tParam.m_f3Pos.x = inPos.x;
	m_tParam.m_f3Pos.y = inPos.y;
	m_tParam.m_f3Pos.z = 0.0f;
}

/****************************************//*
	@brief　	| 表示するローカルサイズを設定
	@param　	| inSize：設定するサイズの値(DirectX::XMFLOAT3)
*//****************************************/
void CRendererComponent::SetSize(const DirectX::XMFLOAT3 inSize)
{
	m_tParam.m_f3Size = inSize;
}

/****************************************//*
	@brief　	| 表示する2Dサイズを設定
	@param　	| inSize：設定するサイズの値(DirectX::XMFLOAT2)
*//****************************************/
void CRendererComponent::SetSize(const DirectX::XMFLOAT2 inSize)
{
	m_tParam.m_f3Size.x = inSize.x;
	m_tParam.m_f3Size.y = inSize.y;
	m_tParam.m_f3Size.z = 1.0f;
}

/****************************************//*
	@brief　	| 表示するローカル回転を設定
	@param　	| inRotation：設定する回転の値(DirectX::XMFLOAT3)
*//****************************************/
void CRendererComponent::SetRotation(const DirectX::XMFLOAT3 inRotation)
{
	m_tParam.m_f3Rotate = inRotation;
}

/****************************************//*
	@brief　	| 表示する2D回転を設定
	@param　	| inRotation：設定する回転の値(float)
*//****************************************/
void CRendererComponent::SetRotation(const float inRotation)
{
	m_tParam.m_f3Rotate.x = 0.0f;
	m_tParam.m_f3Rotate.y = 0.0f;
	m_tParam.m_f3Rotate.z = inRotation;
}

/****************************************//*
	@brief　	| 表示する色を設定
	@param　	| inColor：設定する色の値(DirectX::XMFLOAT4)
*//****************************************/
void CRendererComponent::SetColor(const DirectX::XMFLOAT4 inColor)
{
	m_tParam.m_f4Color = inColor;
}

/****************************************//*
	@brief　	| UV座標の位置を設定
	@param　	| inUVPos：設定するUV座標の位置(DirectX::XMFLOAT2)
*//****************************************/
void CRendererComponent::SetUVPos(const DirectX::XMFLOAT2 inUVPos)
{
	m_tParam.m_f2UVPos = inUVPos;
}

/****************************************//*
	@brief　	| UV座標のサイズを設定
	@param　	| inUVSize：設定するUV座標のサイズ(DirectX::XMFLOAT2)
*//****************************************/
void CRendererComponent::SetUVSize(const DirectX::XMFLOAT2 inUVSize)
{
	m_tParam.m_f2UVSize = inUVSize;
}

/****************************************//*
	@brief　	| 表現技法の変更
	@param　	| inCulling：カリングモード
	@param　	| [D3D11_CULL_NONE：カリングしない(表裏どちらも描画)
	@param　	| [D3D11_CULL_FRONT：表面カリング(裏面のみ描画)
	@param　	| [D3D11_CULL_BACK：裏面カリング(表面のみ描画)
*//****************************************/
void CRendererComponent::SetCullingMode(const D3D11_CULL_MODE inCulling)
{
	m_tParam.m_eCulling = inCulling;
}

/****************************************//*
	@brief　	| 描画用のパラメータを取得
	@return　	| 描画用パラメータをまとめた構造体
*//****************************************/
RendererParam CRendererComponent::GetRendererParam()
{
	return m_tParam;
}

/****************************************//*
	@brief　	| 描画に使用するオブジェクトのキーをセット
	@param　	| inKey：ロードする際に同時に登録したキー
*//****************************************/
void CRendererComponent::SetKey(std::string inKey)
{
	// Map内から引数のキーが存在しているかチェック
	if (m_RendererObjectMap.find(inKey.c_str()) != m_RendererObjectMap.end())
	{
		// 存在していたらそのキーを設定する
		m_sKey = inKey;
	}
	else
	{
		// 存在していなかったらエラーメッセージを送信する
		inKey = "NotFind:" + inKey;
		MessageBox(NULL, inKey.c_str(), "Error", MB_OK);
	}
}

/****************************************//*
	@brief　	| モデル・テクスチャをロードし、キー位置に登録する
	@param　	| inKind：モデルorテクスチャ
	@param　	| inPath：テクスチャのパス
	@param　	| inKey：登録するキー
	@param　	| scale：モデルのスケール(scale倍)
	@param　	| flip：モデルのフリップ
*//****************************************/
void CRendererComponent::Load(const RendererKind inKind, const char* inPath, std::string inKey, const float scale, const Model::Flip flip)
{
	// そのキー位置に既にオブジェクトがロード済みかをチェックする
	auto itr = m_RendererObjectMap.find(inKey.c_str());
	if (itr != m_RendererObjectMap.end())
	{
		// ロード済みなら処理を飛ばす
		return;
	}

	RendererObject tObject{};   // マップに登録するオブジェクトのデータ
	tObject.m_eKind = inKind;   // オブジェクトの種類をセット

	Texture* pTexture = nullptr;    // 読み込み用テクスチャクラスポインタ
	Model* pModel = nullptr;        // 読み込み用モデルクラスポインタ

	switch (tObject.m_eKind)
	{
	case RendererKind::Texture:
        // テクスチャの読み込み
		pTexture = new(std::nothrow) Texture();
		if (FAILED(pTexture->Create(inPath)))
		{
			MessageBox(NULL, inPath, "Error", MB_OK);
			return;
		}
        // 読み込んだらデータをセットする
		tObject.m_Data = pTexture;
		break;
	case RendererKind::Model:
        // モデルの読み込み
		pModel = new Model();
		if (!pModel->Load(inPath, scale, flip))
		{
			MessageBox(NULL, inPath, "Error", MB_OK);
			return;
		}
        // Mesh情報の取得
		std::vector<Model::Mesh> meshVec;
		for (unsigned int i = 0; i < pModel->GetMeshNum(); i++)
		{
			Model::Mesh Mesh = *pModel->GetMesh(i);
			meshVec.push_back(Mesh);
		}

        // モデル用パラメータに一旦移す
		ModelParam tModel;
		tModel.m_pModel = pModel;
		tModel.m_tMeshVec = meshVec;

        // 移したらそのデータをセットする
		tObject.m_Data = tModel;
		break;
	}

    // セットされたデータをキーと共に登録する
	m_RendererObjectMap.emplace(inKey, tObject);
}

/****************************************//*
	@brief　	| 登録した全てのモデル・テクスチャを解放する
*//****************************************/
void CRendererComponent::UnLoad()
{
    // マップから全てのオブジェクト情報を取得
	for (auto& itr : m_RendererObjectMap)
	{
		switch (itr.second.m_eKind)
		{
		case RendererKind::Texture:
            // テクスチャならばテクスチャのdeleteをする
			SAFE_DELETE(std::get<Texture*>(itr.second.m_Data));
			break;
		case RendererKind::Model:
            // モデルならばモデルのdeleteをする
			SAFE_DELETE(std::get<ModelParam>(itr.second.m_Data).m_pModel);
			break;
		}
	}
}
