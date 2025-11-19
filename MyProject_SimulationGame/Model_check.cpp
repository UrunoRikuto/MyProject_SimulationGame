/**********************************************************************************//*
	@file		| Model_check.cpp
	@brief		| モデルデータの共通処理
*//***********************************************************************************/
#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#ifdef _DEBUG
#include "Geometory.h"
#endif

/*************************//*
@brief  | エラーメッセージ取得
@return | エラーメッセージ
*//*************************/
std::string Model::GetError()
{
#ifdef _DEBUG
	return m_errorStr;
#else
	return "";
#endif
}

/*************************//*
@brief		| ボーンのデバッグ表示
@param[in]	| world：ワールド行列
*//*************************/
void Model::DrawBone(DirectX::XMMATRIX world)
{
#ifdef _DEBUG
	// 再帰処理
	std::function<void(int, DirectX::XMFLOAT3)> FuncDrawBone =
		[&FuncDrawBone, this, &world](int idx, DirectX::XMFLOAT3 parent)
	{
		// 親ノードから現在位置まで描画
		DirectX::XMFLOAT3 pos;
		DirectX::XMStoreFloat3(&pos, DirectX::XMVector3TransformCoord(DirectX::XMVectorZero(), m_nodes[idx].mat * world));
		Geometory::AddLine(parent, pos, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));

		// 子ノードの描画
		auto it = m_nodes[idx].children.begin();
		while (it != m_nodes[idx].children.end())
		{
			FuncDrawBone(*it, pos);
			++it;
		}
	};

	// 描画実行
	FuncDrawBone(0, DirectX::XMFLOAT3());
	Geometory::DrawLines();
#endif
}

/*************************//*
@brief			| アニメーションのループチェック
@param[in, out]	|info：再生情報
*//*************************/
void Model::CheckAnimePlayLoop(AnimePlayInfo& info)
{
	if (info.isLoop) {
		while (info.nowTime >= info.totalTime)
		{
			info.nowTime -= info.totalTime;
		}
	}
}

/*************************//*
@brief		| エラーメッセージつき、エラーチェック
@param[in]	| condition：エラー判定
@param[in]	| message：エラーメッセージ
@return		| エラー判定
*//*************************/
bool Model::IsError(bool condition, std::string message)
{
	if(condition)
		SetErrorMessage(message);
	return condition;
}

/*************************//*
@brief		| エラーメッセージの設定
@param[in]	| message：メッセージ内容
*//*************************/
void Model::SetErrorMessage(std::string message)
{
#ifdef _DEBUG
	m_errorStr += message + "\n";
#endif
}

/*************************//*
@brief		| エラーメッセージの表示
@param[in]	| caption：タイトルバー表示メッセージ
@param[in]	| isWarning：警告フラグ
*//*************************/
void Model::ShowErrorMessage(const char* caption, bool isWarning)
{
#ifdef _DEBUG
	std::string msg = GetError();
	if (msg.empty()) { return; }
	if(isWarning)
		MessageBox(NULL, msg.c_str(), (std::string("Warning - ") + caption).c_str(), MB_OK | MB_ICONWARNING);
	else
		MessageBox(NULL, msg.c_str(), (std::string("Error - ") + caption).c_str(), MB_OK | MB_ICONERROR);
#endif
}

/*************************//*
@brief		| ノードの探索
@param[in]	| name：探索ノード名
@return		| ノード番号
*//*************************/
Model::NodeIndex Model::FindNode(const char* name)
{
	// 構築済みのボーンノードから該当ノードを取得
	Model::Nodes::iterator it = std::find_if(m_nodes.begin(), m_nodes.end(),
		[name](const Node& val) {
			return val.name == name;
		});
	if (it == m_nodes.end()) {
		return NODE_NONE;
	}

	return static_cast<NodeIndex>(it - m_nodes.begin());
}

/*************************//*
@brief		| メッシュのフリーズチェック
@param[in]	| ptr：aiSceneへのポインタ
@return		| チェック結果
*//*************************/
bool Model::CheckMeshFreeze(const void* ptr)
{
	// 再帰処理でAssimpのノード情報を読み取り
	std::function<bool(std::string& name, aiNode*)> FuncFreezeCheck =
		[&FuncFreezeCheck, this](std::string& name, aiNode* assimpNode)
	{
		std::string nodeName = assimpNode->mName.data;
		if (nodeName.find(name) != std::string::npos)
		{
			if (fabsf(assimpNode->mTransformation.a4) > FLT_EPSILON) { return false; }
			if (fabsf(assimpNode->mTransformation.b4) > FLT_EPSILON) { return false; }
			if (fabsf(assimpNode->mTransformation.c4) > FLT_EPSILON) { return false; }

			return true;
		}

		for (UINT i = 0; i < assimpNode->mNumChildren; ++i)
		{
			if (!FuncFreezeCheck(name, assimpNode->mChildren[i]))
				return false;
		}
		return true;
	};

	const aiScene* pScene = reinterpret_cast<const aiScene*>(ptr);
	bool result = true;
	for (unsigned int i = 0; i < pScene->mNumMeshes; ++i) {
		std::string meshName = pScene->mMeshes[i]->mName.data;

		// ノード内を探索
		if (!FuncFreezeCheck(meshName, pScene->mRootNode)) {
			SetErrorMessage("no mesh freeze. [" + meshName + "]");
			result = false;
		}
	}

	return result;
}

/*************************//*
@brief		| アニメーション番号のチェック
@param[in]	| no：アニメーション番号
@return		| 有効なアニメーション番号かどうか
*//*************************/
bool Model::CheckAnimeNo(AnimeNo no)
{
	// パラメトリックアニメーション確認
	if (no == PARAMETRIC_ANIME)
	{
		// パラメトリック用のデータが両方正しく設定されているか
		return
			m_parametric[0] != ANIME_NONE &&
			m_parametric[1] != ANIME_NONE;
	}

	// 問題ないアニメーション番号かどうか
	return 0 <= no && no < m_animes.size();
}