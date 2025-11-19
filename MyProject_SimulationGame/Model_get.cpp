/**********************************************************************************//*
	@file		| Model_get.cpp
	@brief		| モデルデータの取得処理
*//***********************************************************************************/
#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

/*************************//*
@brief  | メッシュ数を取得
@return | メッシュ数
*//*************************/
unsigned int Model::GetMeshNum()
{
	return static_cast<int>(m_meshes.size());
}

/*************************//*
@brief		| メッシュ情報を取得
@param[in]	| index：取得するメッシュ番号
@return		| メッシュ情報
*//*************************/
const Model::Mesh* Model::GetMesh(unsigned int index)
{
	if (index < GetMeshNum())
		return &m_meshes[index];
	return nullptr;
}

/*************************//*
@brief  | マテリアル数を取得
@return | マテリアル数
*//*************************/
unsigned int Model::GetMaterialNum()
{
	return static_cast<int>(m_materials.size());
}

/*************************//*
@brief		| マテリアル情報を取得
@param[in]	| index：取得するマテリアル番号
@return		| マテリアル情報
*//*************************/
const Model::Material* Model::GetMaterial(unsigned int index)
{
	if (index < GetMaterialNum())
		return &m_materials[index];
	return nullptr;
}

/*************************//*
@brief		| ボーンの変換行列を取得
@param[in]	| index：取得するボーン番号
@return		| 該当ボーンの変換行列
*//*************************/
DirectX::XMMATRIX Model::GetBoneMatrix(NodeIndex index)
{
	if (index < m_nodes.size())
		return m_nodes[index].mat;
	return DirectX::XMMatrixIdentity();
}

/*************************//*
@brief		| アニメーションが再生中か判定
@param[in]	| no：アニメーション番号
@return		| 再生中か
*//*************************/
bool Model::IsAnimePlay(AnimeNo no)
{
	// アニメーションチェック
	if (!CheckAnimeNo(no)) { return false; }

	// パラメトリックは合成元のアニメを基準に判断
	if (no == PARAMETRIC_ANIME) { no = m_parametric[0]; }

	// 再生時間の判定
	if (m_animes[no].info.totalTime < m_animes[no].info.nowTime) { return false; }

	// それぞれの再生番号に設定されているか確認
	if (m_playNo == no) { return true; }
	if (m_blendNo == no) { return true; }
	if (m_playNo == PARAMETRIC_ANIME || m_blendNo == PARAMETRIC_ANIME)
	{
		if (m_parametric[0] == no) { return true; }
		if (m_parametric[1] == no) { return true; }
	}

	// 再生中でない
	return false;
}

/*************************//*
@brief	| 再生中のアニメ番号を取得
@return	| 再生中のアニメ番号
*//*************************/
Model::AnimeNo Model::GetAnimePlayNo()
{
	return m_playNo;
}

/*************************//*
@brief  | 再生中のブレンドアニメ番号を取得
@return | アニメ番号
*//*************************/
Model::AnimeNo Model::GetAnimeBlendNo()
{
	return m_blendNo;
}

/*************************//*
@brief  | アニメーション情報を取得
@return | アニメーション情報
*//*************************/
const Model::AnimePlayInfo* Model::GetPlayAnimeInfo()
{
	AnimeNo no = GetAnimePlayNo();
	if (!CheckAnimeNo(no)) { return nullptr; }
	return &m_animes[no].info;
}

/*************************//*
@brief		| ディレクトリ情報の取得
@param[in]	| file：ファイルパス
@return		| ディレクトリ部分の情報
*//*************************/
std::string Model::GetDirectory(const char* file)
{
	// ディレクトリの読み取り
	std::string directory = file;
	auto strIt = directory.begin();
	while (strIt != directory.end()) {
		if (*strIt == '/')
			*strIt = '\\';
		++strIt;
	}
	directory = directory.substr(0, directory.find_last_of('\\') + 1);
	return directory;
}