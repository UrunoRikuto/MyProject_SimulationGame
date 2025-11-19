/**********************************************************************************//*
	@file		|Model.cpp
	@brief		|3Dモデルデータ管理
*//***********************************************************************************/
#include "Model.h"
#include "DirectXTex/TextureLoad.h"
#include <algorithm>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#if _MSC_VER >= 1930
#ifdef _DEBUG
#pragma comment(lib, "assimp-vc143-mtd.lib")
#else
#pragma comment(lib, "assimp-vc143-mt.lib")
#endif
#elif _MSC_VER >= 1920
#ifdef _DEBUG
#pragma comment(lib, "assimp-vc142-mtd.lib")
#else
#pragma comment(lib, "assimp-vc142-mt.lib")
#endif
#elif _MSC_VER >= 1910
#ifdef _DEBUG
#pragma comment(lib, "assimp-vc141-mtd.lib")
#else
#pragma comment(lib, "assimp-vc141-mt.lib")
#endif
#endif

// staticメンバ変数定義
VertexShader*	Model::m_pDefVS		= nullptr;
PixelShader*	Model::m_pDefPS		= nullptr;
unsigned int	Model::m_shaderRef	= 0;
#ifdef _DEBUG
std::string		Model::m_errorStr	= "";
#endif

// プロトタイプ宣言
DirectX::XMMATRIX GetMatrixFromAssimpMatrix(aiMatrix4x4 M);
void MakeModelDefaultShader(VertexShader** vs, PixelShader** ps);

/*************************//*
@brief  |コンストラクタ
*//*************************/
Model::Model()
	: m_loadScale(1.0f)
	, m_loadFlip(None)
	, m_morphPlayNo(ANIME_NONE)
	, m_playNo(ANIME_NONE)
	, m_blendNo(ANIME_NONE)
	, m_parametric{ANIME_NONE, ANIME_NONE}
	, m_blendTime(0.0f)
	, m_blendTotalTime(0.0f)
	, m_parametricBlend(0.0f)
{
	// デフォルトシェーダーの適用
	if (m_shaderRef == 0)
	{
		MakeModelDefaultShader(&m_pDefVS, &m_pDefPS);
	}
	m_pVS = m_pDefVS;
	m_pPS = m_pDefPS;
	++m_shaderRef;
}

/*************************//*
@brief  |デストラクタ
*//*************************/
Model::~Model()
{
	Reset();
	--m_shaderRef;
	if (m_shaderRef <= 0)
	{
		delete m_pDefPS;
		delete m_pDefVS;
	}
}

/*************************//*
@brief  |内部データの削除
*//*************************/
void Model::Reset()
{
	// メッシュデータ削除
	auto meshIt = m_meshes.begin();
	while (meshIt != m_meshes.end())
	{
		if (meshIt->pMesh) delete meshIt->pMesh;
		++meshIt;
	}

	// マテリアルデータ削除
	auto matIt = m_materials.begin();
	while (matIt != m_materials.end())
	{
		if (matIt->pTexture) delete matIt->pTexture;
		++matIt;
	}

	// ノードデータ削除
	m_nodes.clear();
}

/*************************//*
@brief		| 頂点シェーダー設定
@param[in]	| vs：頂点シェーダー
*//*************************/
void Model::SetVertexShader(Shader* vs)
{
	if (vs && typeid(VertexShader) == typeid(*vs))
		m_pVS = static_cast<VertexShader*>(vs);
	else
		m_pVS = m_pDefVS;
}

/*************************//*
@brief		| ピクセルシェーダー設定
@param[in]	| ps：ピクセルシェーダー
*//*************************/
void Model::SetPixelShader(Shader* ps)
{
	if (ps && typeid(PixelShader) == typeid(*ps))
		m_pPS = static_cast<PixelShader*>(ps);
	else
		m_pPS = m_pDefPS;
}

/*************************//*
@brief		| モデルデータの読み込み
@param[in]	| file：読み込み先パス
@param[in]	| scale：拡大率
@param[in]	| flip：反転設定
@return		| 読み込み結果
*//*************************/
bool Model::Load(const char* file, float scale, Flip flip)
{
	Reset();

	// 読み込み時の設定を保存
	m_loadScale = scale;
	m_loadFlip = flip;

	// Assimp側で読み込みを実行
	const aiScene* pScene = static_cast<const aiScene*>(LoadAssimpScene(file));
	if (!pScene) { return false; }

	// フリーズチェック
#ifdef _DEBUG
	CheckMeshFreeze(pScene);
#endif

	// ノードの作成
	MakeNodes(pScene);
	// メッシュ作成
	MakeMesh(pScene);
	// マテリアルの作成
	MakeMaterial(pScene, GetDirectory(file));

#if MODEL_FORCE_ERROR
	ShowErrorMessage(file, true);
#endif
	return true;
}

/*************************//*
@brief		|更新処理
@param[in]	| tick：経過時間(秒)
*//*************************/
void Model::Step(float tick)
{
	StepAnime(tick);
	StepMorph(tick);
	UpdateMorph();
}

/*************************//*
@brief		|描画処理
@param[in]	| meshNo：描画メッシュ番号(-1ですべて表示
*//*************************/
void Model::Draw(int meshNo)
{
	// シェーダー設定
	m_pVS->Bind();
	m_pPS->Bind();

	// テクスチャ自動設定
	bool isAutoTexture = (meshNo == -1);

	// 描画数設定
	size_t drawNum = m_meshes.size();
	if (meshNo != -1)
		drawNum = meshNo + 1;
	else
		meshNo = 0;

	// 描画
	for (UINT i = meshNo; i < drawNum; ++i)
	{
		if (isAutoTexture) {
			m_pPS->SetTexture(0, m_materials[m_meshes[i].materialID].pTexture);
		}
		m_meshes[i].pMesh->Draw();
	}
}

/*************************//*
@brief		|Assimpでの読み込み実行
@param[in]	| file：読み込み先パス
@return		| aiSceneへのポインタ
*//*************************/
const void* Model::LoadAssimpScene(const char* file)
{
#ifdef _DEBUG
	m_errorStr = "";
#endif

	// assimpの設定
	static Assimp::Importer importer;
	int flag = 0;
	flag |= aiProcess_Triangulate;
	flag |= aiProcess_FlipUVs;
	if (m_loadFlip == Flip::XFlip)  flag |= aiProcess_MakeLeftHanded;

	// assimpで読み込み
	const aiScene* pScene = importer.ReadFile(file, flag);
#ifdef _DEBUG
	if (!pScene)
		m_errorStr += importer.GetErrorString();
#endif

	return pScene;
}

/*************************//*
@brief		|ノード情報の作成
@param[in]	| ptr：aiSceneへのポインタ
*//*************************/
void Model::MakeNodes(const void* ptr)
{
	// 再帰処理でAssimpのノード情報を読み取り
	using FuncRecurciveMakeNodes = std::function<NodeIndex(aiNode*, NodeIndex, DirectX::XMMATRIX)>;
	FuncRecurciveMakeNodes func = [&func, this](aiNode* assimpNode, NodeIndex parent, DirectX::XMMATRIX mat)
	{
		// assimp側で階層情報が細かく分割されている場合の処理
		std::string name = assimpNode->mName.data;
		if (name.find("$AssimpFbx") != std::string::npos)
		{
			// 分割前のノードに到達するまで、各行列を掛け合わせていく
			DirectX::XMMATRIX transform = GetMatrixFromAssimpMatrix(assimpNode->mTransformation);
			mat = transform * mat;
			return func(assimpNode->mChildren[0], parent, mat);
		}

		// Assimpのノード情報を格納
		Node node;
		node.name	= name;
		node.parent	= parent;
		node.children.resize(assimpNode->mNumChildren);
		node.mat = mat;

		// ノードの一覧に追加
		m_nodes.push_back(node);
		NodeIndex nodeIndex = static_cast<NodeIndex>(m_nodes.size() - 1);

		// 子要素の情報を追加
		for (UINT i = 0; i < assimpNode->mNumChildren; ++i)
		{
			m_nodes[nodeIndex].children[i] =
				func(assimpNode->mChildren[i], nodeIndex, DirectX::XMMatrixIdentity());
		}

		return nodeIndex;
	};

	// ノード作成
	const aiScene* pScene = reinterpret_cast<const aiScene*>(ptr);
	func(pScene->mRootNode, NODE_NONE, DirectX::XMMatrixIdentity());

	// アニメーション計算領域に、ノード数分の初期データを作成
	AnimeTransform init = {
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f)
	};
	for (int i = 0; i < MAX_ANIMEPATTERN; ++i)
	{
		m_animeTransform[i].resize(m_nodes.size(), init);
	}
}

/*************************//*
@brief		|メッシュ情報の作成
@param[in]	| ptr：aiSceneへのポインタ
@param[in]	| meshIndex：メッシュ番号
*//*************************/
void Model::MakeVertexWeight(const void* ptr, int meshIndex)
{
	const aiScene* pScene = reinterpret_cast<const aiScene*>(ptr);
	aiMesh* assimpMesh = pScene->mMeshes[meshIndex];
	if (assimpMesh->HasBones())
		MakeVertexWeightHasBone(assimpMesh, m_meshes[meshIndex]);
	else
		MakeVertexWeightFromNode(ptr, assimpMesh, m_meshes[meshIndex]);
}

/*************************//*
@brief		|ボーン情報から頂点ウェイトを取得
@param[in]	| ptr：aiMeshへのポインタ
@param[out]	| mesh：頂点情報を変更するメッシュ
*//*************************/
void Model::MakeVertexWeightHasBone(const void* ptr, Mesh& mesh)
{
	const aiMesh* assimpMesh = reinterpret_cast<const aiMesh*>(ptr);
	DirectX::XMMATRIX mFlip =
		DirectX::XMMatrixScaling(m_loadFlip == ZFlipUseAnime ? -1.0f : 1.0f, 1.0f, 1.0f);
	DirectX::XMMATRIX mScale =
		DirectX::XMMatrixScaling(1.f / m_loadScale, 1.f / m_loadScale, 1.f / m_loadScale);

	// 読み取り用のウェイトデータ作成
	struct WeightPair
	{
		unsigned int idx;
		float weight;
	};
	std::vector<std::vector<WeightPair>> weights;
	weights.resize(mesh.vertices.size());


	// メッシュに割り当てられているボーン領域確保
	mesh.bones.resize(assimpMesh->mNumBones);
	for (auto boneIt = mesh.bones.begin(); boneIt != mesh.bones.end(); ++boneIt)
	{
		UINT boneIdx = static_cast<UINT>(boneIt - mesh.bones.begin());
		aiBone* assimpBone = assimpMesh->mBones[boneIdx];

		// 構築済みのノードから該当ノードのインデックスを取得
		boneIt->nodeIndex = FindNode(assimpBone->mName.data);
		if (boneIt->nodeIndex == NODE_NONE)
		{
			continue;
		}

		// メッシュに割り当てられているボーンを、原点に戻す逆行列を設定
		boneIt->invOffset = GetMatrixFromAssimpMatrix(assimpBone->mOffsetMatrix);
		boneIt->invOffset.r[3].m128_f32[0] *= m_loadScale;
		boneIt->invOffset.r[3].m128_f32[1] *= m_loadScale;
		boneIt->invOffset.r[3].m128_f32[2] *= m_loadScale;
		boneIt->invOffset = mFlip * boneIt->invOffset * mScale;

		// ウェイトの設定
		for (UINT i = 0; i < assimpBone->mNumWeights; ++i)
		{
			aiVertexWeight& weight = assimpBone->mWeights[i];
			weights[weight.mVertexId].push_back({ boneIdx, weight.mWeight });
		}
	}

	// 取得してきた頂点ウェイトを頂点データに書き込み
	for (int i = 0; i < weights.size(); ++i)
	{
		// プログラム側の割り当て数を超える場合の正規化処理
		if (weights[i].size() >= MAX_WEIGHT)
		{
			// 割合が高い順にソート
			std::sort(weights[i].begin(), weights[i].end(),
				[](WeightPair& a, WeightPair& b) {
					return a.weight > b.weight;
				});
			// ウェイト数に合わせて正規化
			float total = 0.0f;
			for (int j = 0; j < MAX_WEIGHT; ++j)
				total += weights[i][j].weight;
			for (int j = 0; j < MAX_WEIGHT; ++j)
				weights[i][j].weight /= total;
		}

		// 正規化したデータを頂点データに割り当て
		for (int j = 0; j < weights[i].size() && j < MAX_WEIGHT; ++j)
		{
			mesh.vertices[i].index[j] = weights[i][j].idx;
			mesh.vertices[i].weight[j] = weights[i][j].weight;
		}
	}
}

/*************************//*
@brief		|ノード情報から頂点ウェイトを取得
@param[in]	| scene：aiSceneへのポインタ
@param[in]	| ptr：aiMeshへのポインタ
@param[out]	| mesh：頂点情報を変更するメッシュ
*//*************************/
void Model::MakeVertexWeightFromNode(const void* scene, const void* ptr, Mesh& mesh)
{
	const aiScene* pScene = reinterpret_cast<const aiScene*>(scene);
	const aiMesh* assimpMesh = reinterpret_cast<const aiMesh*>(ptr);

	// 階層内のメッシュを探索
	NodeIndex nodeIndex = FindNode(assimpMesh->mName.data);
	if (nodeIndex == NODE_NONE) { return; }

	// メッシュでない親ノードを再帰探索
	using FuncRecurciveFindNoMesh = std::function<int(int)>;
	FuncRecurciveFindNoMesh func = [&func, this, pScene](NodeIndex parent)
	{
		if (parent == NODE_NONE) { return NODE_NONE; }

		// 現在探索中のノード名がメッシュの一覧と一致するか確認
		std::string name = m_nodes[parent].name;
		for (UINT i = 0; i < pScene->mNumMeshes; ++i)
		{
			if (name == pScene->mMeshes[i]->mName.data)
			{
				return func(m_nodes[parent].parent);
			}
		}

		// 一致しないノードを変換元として扱う
		return parent;
	};

	// ボーン割り当て情報を作成
	Bone bone = {};
	bone.nodeIndex = func(m_nodes[nodeIndex].parent);
	if (bone.nodeIndex != NODE_NONE) {
		// ノードに保存されている行列を逆行列として設定
		bone.invOffset = DirectX::XMMatrixInverse(nullptr, m_nodes[bone.nodeIndex].mat);
		// ボーンの情報を設定
		mesh.bones.resize(1);
		mesh.bones[0] = bone;

		// 頂点すべてを割り当てたボーンに100%依存とする
		for (auto vtxIt = mesh.vertices.begin(); vtxIt != mesh.vertices.end(); ++vtxIt)
			vtxIt->weight[0] = 1.0f;
	}
}

/*************************//*
@brief		|Assimpの行列からDirectXの行列へ変換
@param[in]	| M：Assimpの行列
@return		| DirectXの行列
*//*************************/
DirectX::XMMATRIX GetMatrixFromAssimpMatrix(aiMatrix4x4 M)
{
	return DirectX::XMMatrixSet(
		M.a1, M.b1, M.c1, M.d1,
		M.a2, M.b2, M.c2, M.d2,
		M.a3, M.b3, M.c3, M.d3,
		M.a4, M.b4, M.c4, M.d4
	);
}

/*************************//*
@brief		|デフォルトシェーダーの作成
@param[out]	| vs：頂点シェーダー
@param[out]	| ps：ピクセルシェーダー
*//*************************/
void MakeModelDefaultShader(VertexShader** vs, PixelShader** ps)
{
	const char* ModelVS = R"EOT(
struct VS_IN {
	float3 pos : POSITION0;
	float3 normal : NORMAL0;
	float2 uv : TEXCOORD0;
};
struct VS_OUT {
	float4 pos : SV_POSITION;
	float3 normal : NORMAL0;
	float2 uv : TEXCOORD0;
};
VS_OUT main(VS_IN vin) {
	VS_OUT vout;
	vout.pos = float4(vin.pos, 1.0f);
	vout.pos.z += 0.5f;
	vout.pos.y -= 0.8f;
	vout.normal = vin.normal;
	vout.uv = vin.uv;
	return vout;
})EOT";
	const char* ModelPS = R"EOT(
struct PS_IN {
	float4 pos : SV_POSITION;
	float3 normal : NORMAL0;
	float2 uv : TEXCOORD0;
};
Texture2D tex : register(t0);
SamplerState samp : register(s0);
float4 main(PS_IN pin) : SV_TARGET
{
	return tex.Sample(samp, pin.uv);
})EOT";
	*vs = new VertexShader();
	(*vs)->Compile(ModelVS);
	*ps = new PixelShader();
	(*ps)->Compile(ModelPS);
}

/*************************//*
@brief		|メッシュ情報の作成
@param[in]	| ptr：aiSceneへのポインタ
*//*************************/
void Model::MakeMesh(const void* ptr)
{
	// 事前準備
	aiVector3D zero3(0.0f, 0.0f, 0.0f);
	aiColor4D one4(1.0f, 1.0f, 1.0f, 1.0f);
	const aiScene* pScene = reinterpret_cast<const aiScene*>(ptr);
	float xFlip = m_loadFlip == Flip::XFlip ? -1.0f : 1.0f;
	float zFlip = (m_loadFlip == Flip::ZFlip || m_loadFlip == Flip::ZFlipUseAnime) ? -1.0f : 1.0f;
	int idx1 = (m_loadFlip == Flip::XFlip || m_loadFlip == Flip::ZFlip) ? 2 : 1;
	int idx2 = (m_loadFlip == Flip::XFlip || m_loadFlip == Flip::ZFlip) ? 1 : 2;

	// メッシュの作成
	m_meshes.resize(pScene->mNumMeshes);
	for (unsigned int i = 0; i < m_meshes.size(); ++i)
	{
		aiMesh* assimpMesh = pScene->mMeshes[i];
		Mesh& mesh = m_meshes[i];

		// ノード階層内の探索
		mesh.nodeIndex = FindNode(assimpMesh->mName.data);
		if (mesh.nodeIndex == NODE_NONE) { continue; }

		// 頂点書き込み先の領域を用意
		mesh.vertices.resize(assimpMesh->mNumVertices);

		// 頂点データの書き込み
		for (unsigned int j = 0; j < mesh.vertices.size(); ++j) {
			// モデルデータから値の取得
			aiVector3D pos = assimpMesh->mVertices[j];
			aiVector3D normal = assimpMesh->HasNormals() ? assimpMesh->mNormals[j] : zero3;
			aiVector3D uv = assimpMesh->HasTextureCoords(0) ?
				assimpMesh->mTextureCoords[0][j] : zero3;
			aiColor4D color = assimpMesh->HasVertexColors(0) ? assimpMesh->mColors[0][j] : one4;
			// 値を設定
			mesh.vertices[j] = {
				DirectX::XMFLOAT3(pos.x * m_loadScale * xFlip, pos.y * m_loadScale, pos.z * m_loadScale * zFlip),
				DirectX::XMFLOAT3(normal.x, normal.y, normal.z),
				DirectX::XMFLOAT2(uv.x, uv.y),
				DirectX::XMFLOAT4(color.r, color.g, color.b, color.a)
			};
		}

		// ボーン生成
		MakeVertexWeight(pScene, i);

		// インデックスの書き込み先の用意
		// mNumFacesはポリゴンの数を表す(１ポリゴンで3インデックス
		mesh.indices.resize(assimpMesh->mNumFaces * 3);

		// インデックスの書き込み
		for (unsigned int j = 0; j < assimpMesh->mNumFaces; ++j) {
			// モデルデータから値の取得
			aiFace face = assimpMesh->mFaces[j];

			// 値の設定
			int idx = j * 3;
			mesh.indices[idx + 0] = face.mIndices[0];
			mesh.indices[idx + 1] = face.mIndices[idx1];
			mesh.indices[idx + 2] = face.mIndices[idx2];
		}

		// マテリアルの割り当て
		mesh.materialID = assimpMesh->mMaterialIndex;

		// メッシュを元に頂点バッファ作成
		MeshBuffer::Description desc = {};
		desc.pVtx = mesh.vertices.data();
		desc.vtxSize = sizeof(Vertex);
		desc.vtxCount = static_cast<int>(mesh.vertices.size());
		desc.pIdx = mesh.indices.data();
		desc.idxSize = sizeof(unsigned long);
		desc.idxCount = static_cast<int>(mesh.indices.size());
		desc.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		mesh.pMesh = new MeshBuffer();
		mesh.pMesh->Create(desc);
	}
}

/*************************//*
@brief		|マテリアル情報の作成
@param[in]	| ptr：aiSceneへのポインタ
@param[in]	| directory：モデルの読み込み先ディレクトリ
*//*************************/
void Model::MakeMaterial(const void* ptr, std::string directory)
{
	// 事前準備
	aiColor3D color(0.0f, 0.0f, 0.0f);
	const aiScene* pScene = reinterpret_cast<const aiScene*>(ptr);
	float shininess;

	// マテリアルの作成
	m_materials.resize(pScene->mNumMaterials);
	for (unsigned int i = 0; i < m_materials.size(); ++i)
	{
		aiMaterial* assimpMaterial = pScene->mMaterials[i];
		Material& material = m_materials[i];

		//--- 各種マテリアルパラメーターの読み取り
		// 拡散光の読み取り
		if (assimpMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
			material.diffuse = DirectX::XMFLOAT4(color.r, color.g, color.b, 1.0f);
		else
			material.diffuse = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		// 環境光の読み取り
		if (assimpMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS)
			material.ambient = DirectX::XMFLOAT4(color.r, color.g, color.b, 1.0f);
		else
			material.ambient = DirectX::XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
		// 反射光の読み取り
		if (assimpMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS)
			material.specular = DirectX::XMFLOAT4(color.r, color.g, color.b, 0.0f);
		else
			material.specular = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		// 反射光の強さを読み取り
		if (assimpMaterial->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS)
			material.specular.w = shininess;

		// テクスチャ読み込み処理
		HRESULT hr;
		aiString path;

		// テクスチャのパス情報を読み込み
		material.pTexture = nullptr;
		if (assimpMaterial->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), path) != AI_SUCCESS) {
			continue;
		}

		// テクスチャ領域確保
		material.pTexture = new Texture;

		// そのまま読み込み
		hr = material.pTexture->Create(path.C_Str());
		if (SUCCEEDED(hr)) { continue; }

		// ディレクトリと連結して探索
		hr = material.pTexture->Create((directory + path.C_Str()).c_str());
		if (SUCCEEDED(hr)) { continue; }

		// モデルと同じ階層を探索
		// パスからファイル名のみ取得
		std::string fullPath = path.C_Str();
		std::string baseDir = GetDirectory(fullPath.c_str());
		std::string fileName = fullPath.substr(baseDir.size());

		// テクスチャの読込
		hr = material.pTexture->Create((directory + fileName).c_str());
		if (SUCCEEDED(hr)) { continue; }

		// テクスチャが見つからなかった
		delete material.pTexture;
		material.pTexture = nullptr;
		SetErrorMessage("Not find texture. [" + fullPath + "]");
	}
}