/**********************************************************************************//*
	@file		| Model_morph.cpp
	@brief		| モーフデータの読み込み、更新処理
*//***********************************************************************************/
#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

/*************************//*
@brief		| モーフデータの追加
@param[in]	|file：読み込み先パス
@param[out]	|out：読み取ったデータに割り当てられたインデックス(複数読み込む可能性があるため
@return		|読み取り結果
*//*************************/
bool Model::AddMorph(const char* file, Indices* out)
{
	// Assimp側で読み込みを実行
	const aiScene* pScene = static_cast<const aiScene*>(LoadAssimpScene(file));
	if (!pScene)
	{
#if MODEL_FORCE_ERROR
		ShowErrorMessage(file, false);
#endif
		return false;
	}

	// メッシュチェック
	if (IsError(!pScene->HasMeshes(), "no meshes."))
	{
#if MODEL_FORCE_ERROR
		ShowErrorMessage(file, true);
#endif
		return false;
	}

	// メッシュごとに割り当てられているモーフターゲットを作成
	for (unsigned int i = 0; i < pScene->mNumMeshes; ++i)
	{
		MakeMorphMesh(pScene->mMeshes[i], i, out);
	}

#if MODEL_FORCE_ERROR
	ShowErrorMessage(file, true);
#endif
	return true;
}

/*************************//*
@brief		| モーフの合成割合を指定
@param[in]	|no：ブレンド割合を変更するモーフのデータ
*//*************************/
void Model::SetMorphWeight(MorphNo no, float weight)
{
	if (no < m_morphes.size())
		m_morphes[no].weight = weight;
}

/*************************//*
@brief		| モーフアニメーションの追加
@param[in]	| file：読み込み先パス
@return		| アニメーション番号
*//*************************/
Model::AnimeNo Model::AddMorphAnime(const char* file)
{
	// Assimp側で読み込みを実行
	const aiScene* pScene = static_cast<const aiScene*>(LoadAssimpScene(file));
	if (!pScene)
	{
#if MODEL_FORCE_ERROR
		ShowErrorMessage(file, false);
#endif
		return ANIME_NONE;
	}

	// アニメーションデータのチェック
	if (IsError(!pScene->HasAnimations(), "no morph anime."))
	{
#if MODEL_FORCE_ERROR
		ShowErrorMessage(file, true);
#endif
		return ANIME_NONE;
	}

	// モーフアニメーションのチェック
	aiAnimation* assimpAnime = pScene->mAnimations[0];
	if (IsError(assimpAnime->mNumMorphMeshChannels <= 0, "no morph anime."))
	{
#if MODEL_FORCE_ERROR
		ShowErrorMessage(file, true);
#endif
		return ANIME_NONE;
	}

	// アニメーション対象のメッシュが読み込まれているか確認
	aiMeshMorphAnim* assimpMorphAnime = assimpAnime->mMorphMeshChannels[0];
	std::string meshName = assimpMorphAnime->mName.data;
	meshName = meshName.substr(0, meshName.size() - 2); // assimpのアニメーション名に(多分) "*0"が含まれるので削除
	NodeIndex mesh = FindNode(meshName.c_str());
	if (IsError(mesh == NODE_NONE, "no match morph target mesh."))
	{
#if MODEL_FORCE_ERROR
		ShowErrorMessage(file, false);
#endif
		return ANIME_NONE;
	}

	// 該当のメッシュに割り当てられているモーフデータの名称を取得
	using Names = std::vector<std::string>;
	Names morphNames;
	for (unsigned int i = 0; i < pScene->mNumMeshes; ++i) {
		
		// モーフターゲットの名称とメッシュの名称を確認
		aiMesh* assimpMesh = pScene->mMeshes[i];
		if (meshName != assimpMesh->mName.data) { continue; }

		// メッシュ中に割り当てられているモーフの名称を取得
		for (unsigned int j = 0; j < assimpMesh->mNumAnimMeshes; ++j) {
			morphNames.push_back(assimpMesh->mAnimMeshes[j]->mName.data);
		}
		break;
	}

	// 割り当てられなかった名称がないか確認
	if (IsError(morphNames.empty(), "no names."))
	{
#if MODEL_FORCE_ERROR
		ShowErrorMessage(file, false);
#endif
		return ANIME_NONE;
	}

	// 確実にアニメーションデータがありそうなので、データを作成
	m_morphAnimes.push_back({});
	MorphAnimation& anime = m_morphAnimes.back();

	// アニメーション対象のモーフの名称から、既に読み込まれているモーフデータのインデックスに変換
	Names::iterator it = morphNames.begin();
	while (it != morphNames.end()) {

		// モーフデータの一覧から名前と一致するデータを探索
		auto morphIt = std::find_if(m_morphes.begin(), m_morphes.end(),
			[&it](MorphMesh& mesh) {
				return (*it) == mesh.name;
			});

		// インデックスへ変換
		if (morphIt != m_morphes.end()) {
			anime.morphs.push_back(static_cast<unsigned int>(morphIt - m_morphes.begin()));
		}
		else
			anime.morphs.push_back(-1);
		++it;
	}

	// アニメーションデータの読み取り
	MakeMorphTimeline(anime, assimpAnime);
	// タイムラインの最終データをアニメーションの再生時間として格納
	anime.info.totalTime = anime.timelines.back().time;

#if MODEL_FORCE_ERROR
	ShowErrorMessage(file, true);
#endif
	return static_cast<AnimeNo>(m_morphAnimes.size() - 1);
}

/*************************//*
@brief		| モーフアニメーションの再生
@param[in]	| no：再生するアニメーション
@param[in]	| loop：ループ再生フラグ
@param[in]	| speed：再生速度
*//*************************/
void Model::PlayMorph(AnimeNo no, bool loop, float speed)
{
	if (no < m_morphAnimes.size()) {
		m_morphPlayNo = no;
		m_morphAnimes[no].info.isLoop = loop;
		m_morphAnimes[no].info.speed = speed;
		m_morphAnimes[no].info.nowTime = 0.0f;
	}
}

/*************************//*
@brief		| モーフのメッシュデータ作成
@param[in]	| ptr：メッシュデータ
@param[in]	| meshIndex：モーフ対応させるメッシュのインデックス
@param[out]	| out：モーフを複数読み込んだ場合の読み取りインデックス情報
*//*************************/
void Model::MakeMorphMesh(const void* ptr, int meshIndex, Indices* out)
{
	const aiMesh* asmpMesh = static_cast<const aiMesh*>(ptr);

	// モーフが含まれるメッシュかチェック
	unsigned int animeMeshNum = asmpMesh->mNumAnimMeshes;
	if (animeMeshNum <= 0)
	{
		std::string msg;
		msg += "no morph. [";
		msg += asmpMesh->mName.data;
		msg += "]";
		SetErrorMessage(msg.c_str());
		return;
	}

	// 書き込み可能なメッシュに変更
	MeshBuffer* mesh = m_meshes[meshIndex].pMesh;
	MeshBuffer::Description desc = mesh->GetDesc();
	if (!desc.isWrite) {
		desc.pVtx = m_meshes[meshIndex].vertices.data();
		desc.pIdx = m_meshes[meshIndex].indices.data();
		desc.isWrite = true;
		mesh->Create(desc);
	}

	// モーフターゲットのメッシュを生成
	for (unsigned int i = 0; i < animeMeshNum; ++i)
	{
		const aiAnimMesh* animeMesh = asmpMesh->mAnimMeshes[i];

		// 頂点数が一致しているか確認
		unsigned int vtxNum = animeMesh->mNumVertices;
		if (vtxNum != m_meshes[meshIndex].vertices.size())
		{
			std::string msg;
			msg += "no match morph vtxNum. [";
			msg += animeMesh->mName.data;
			msg += "]";
			SetErrorMessage(msg.c_str());
			continue;
		}

		// すでに同名のモーフが作成されていないか判定
		auto it = std::find_if(m_morphes.begin(), m_morphes.end(),
			[&animeMesh](MorphMesh& mesh) {
				return mesh.name == animeMesh->mName.data;
			});
		if (it != m_morphes.end()) {
			continue;
		}

		// モーフの情報を作成
		m_morphes.push_back({});
		Model::MorphMesh& morph = m_morphes.back();
		morph.name		= animeMesh->mName.data;
		morph.meshNo	= meshIndex;
		morph.weight	= animeMesh->mWeight;
		MakeMorphVertices(morph.vertices, animeMesh);

		// 生成したモーフのインデックスを作成
		if (out) {
			out->push_back(static_cast<unsigned long>(m_morphes.size() - 1));
		}
	}
}

/*************************//*
@brief		| モーフの頂点データ作成
@param[out]	| out：頂点データ格納先
@param[in]	| ptr：メッシュ情報
*//*************************/
void Model::MakeMorphVertices(MorphVertices& out, const void* ptr)
{
	const aiAnimMesh* animeMesh = static_cast<const aiAnimMesh*>(ptr);
	unsigned int vtxNum = animeMesh->mNumVertices;

	out.resize(vtxNum);
	for(unsigned int i = 0; i < vtxNum; ++ i)
	{
		// 座標
		if (animeMesh->HasPositions()) {
			aiVector3D& vec = animeMesh->mVertices[i];
			out[i].pos = { vec.x * m_loadScale, vec.y * m_loadScale, vec.z * m_loadScale };
		}
		// 法線
		if (animeMesh->HasNormals()) {
			aiVector3D& vec = animeMesh->mNormals[i];
			out[i].normal = { vec.x, vec.y, vec.z };
		}
		// UV座標
		if (animeMesh->HasTextureCoords(0)) {
			aiVector3D& vec = animeMesh->mTextureCoords[0][i];
			out[i].uv = { vec.x, vec.y };
		}
	}
}

/*************************//*
@brief		|タイムラインデータの作成
@param[out]	| anime：タイムラインデータ格納元
@param[in]	| ptr：モーフのアニメーションデータ
*//*************************/
void Model::MakeMorphTimeline(MorphAnimation& anime, const void* ptr)
{
	const aiAnimation* assimpAnime = static_cast<const aiAnimation*>(ptr);
	const aiMeshMorphAnim* assimpMorphAnime = assimpAnime->mMorphMeshChannels[0];
	float fbxToGameFrame = static_cast<float>(assimpAnime->mTicksPerSecond);
	
	// キー数に応じてタイムライン作成
	anime.timelines.resize(assimpMorphAnime->mNumKeys);
	for (unsigned int i = 0; i < assimpMorphAnime->mNumKeys; ++i)
	{
		aiMeshMorphKey* assimpKey = &assimpMorphAnime->mKeys[i];
		MorphTimeline& timeline = anime.timelines[i];

		// タイムラインのキー時間を設定
		timeline.time = static_cast<float>(assimpKey->mTime) / fbxToGameFrame;
		// 事前計算済みの割り当てモーフ数に応じてタイムラインのデータを拡張
		timeline.weights.resize(anime.morphs.size());

		// 既存キーをタイムラインに割り当て
		int index = 0;
		for (int j = 0; j < timeline.weights.size(); ++j) {
			// 割り当て済みのモーフに対するキーが存在するか判定
			if (assimpKey->mValues[index] == j) {
				timeline.weights[assimpKey->mValues[index]] = static_cast<float>(assimpKey->mWeights[index]);
				++index;
			}
			else
			{
				// 後の処理で補間データ格納先として、-1を設定
				timeline.weights[j] = -1.0f;
			}
		}
	}

	// キーがなかったデータに対して補間処理を実行
	for (int i = 0; i < anime.morphs.size(); ++i)
	{
		int index = 0;
		float value = anime.timelines[index].weights[i];

		// -1以外のキーが出てくるまで探索
		while (value < 0.0f) {
			value = anime.timelines[index].weights[i];
			++index;
			if (index >= anime.timelines.size() && value < 0.0f) {
				value = 0.0f; // 最後まで-1の値が格納されていた場合
				break;
			}
		}

		// スタート地点まで戻って、探索位置まで同じデータを格納
		for (int j = index - 1; j >= 0; --j) {
			anime.timelines[j].weights[i] = value;
		}

		// タイムラインの終端まで-1の個所に補間データを格納
		while (index < anime.timelines.size() - 1) {
			// 探索位置の更新
			int start = index;

			// -1以外のキーまで探索
			do {
				++index;
				// 終端に到達するまで-1しかない場合、スタート地点の値を格納
				if (index >= anime.timelines.size()) {
					--index;
					value = anime.timelines[start].weights[i];
					break;
				}
				value = anime.timelines[index].weights[i];
			} while (value < 0.0f);

			// 開始地点から探索位置まで補間データを格納
			AnimeTime startTime	= anime.timelines[start].time;
			AnimeTime timeLen	= anime.timelines[index].time - startTime;
			float startValue	= anime.timelines[start].weights[i];
			float endValue		= value;
			for (int j = start + 1; j <= index; ++j) {
				// 現在のキー位置に応じた補間の値を計算
				float rate = (anime.timelines[j].time - startTime) / timeLen;
				anime.timelines[j].weights[i] = endValue * rate + startValue * (1.0f - rate);
			}
		}
	}
}

/*************************//*
@brief  | モーフの合成結果の更新
*//*************************/
void Model::UpdateMorph()
{
	// 更新可能なメッシュを探索
	for(auto meshIt = m_meshes.begin(); meshIt != m_meshes.end(); ++ meshIt)
	{
		// 書き込み不可のメッシュに関しては処理を行わない
		if (!meshIt->pMesh->GetDesc().isWrite) { continue; }

		// 該当の番号のメッシュに一致するモーフターゲットを探索
		std::vector<int> morphIdx;
		int meshNo = static_cast<int>(meshIt - m_meshes.begin());
		auto morphIt = m_morphes.begin();
		float totalWeight = 0.0f;
		while (morphIt != m_morphes.end()) {
			if (morphIt->meshNo == meshNo) {
				if (morphIt->weight > FLT_EPSILON) {
					// メッシュにかかる最大ウェイトの計算
					totalWeight += morphIt->weight;
					// 更新が必要なモーフの情報を保存
					morphIdx.push_back(static_cast<NodeIndex>(morphIt - m_morphes.begin()));
				}
			}
			++morphIt;
		}

		// ブレンド前のデータを事前コピー
		Vertices vtx;
		vtx.resize(meshIt->vertices.size());
		for (unsigned int i = 0; i < meshIt->vertices.size(); ++i) {
			vtx[i] = meshIt->vertices[i];
			// モーフのブレンド対象のデータだけ初期化
			vtx[i].pos = {};
			vtx[i].normal = {};
			vtx[i].uv = {};
		}


		// ブレンドの割合が1.0に満たない場合、残りの割合をデフォルトの頂点とのブレンドと見なす
		if (totalWeight < 1.0f)
		{
			float weight = 1.0f - totalWeight;
			auto vtxIt = meshIt->vertices.begin();
			while (vtxIt != meshIt->vertices.end()) {
				AddMorphVtxWeight(
					&vtx[vtxIt - meshIt->vertices.begin()],
					{ vtxIt->pos, vtxIt->normal, vtxIt->uv }, weight);
				++vtxIt;
			}
			totalWeight = 1.0f;
		}

		// モーフターゲットに基づいてブレンドの割合を指定
		auto idxIt = morphIdx.begin();
		while (idxIt != morphIdx.end())
		{
			auto morph = m_morphes[*idxIt];
			auto vtxIt = morph.vertices.begin();
			while (vtxIt != morph.vertices.end()) {
				AddMorphVtxWeight(
					&vtx[vtxIt - morph.vertices.begin()],
					*vtxIt, morph.weight / totalWeight);
				++vtxIt;
			}
			++idxIt;
		}

		// モーフの合成結果を書き込み
		meshIt->pMesh->Write(vtx.data());
	}
}

/*************************//*
@brief		| モーフアニメーションの更新
@param[in]	| tick：更新時間
*//*************************/
void Model::StepMorph(float tick)
{
	// アニメーションチェック
	if (m_morphPlayNo == ANIME_NONE) { return; }

	// アニメーション再生時間の更新
	MorphAnimation& anime = m_morphAnimes[m_morphPlayNo];
	anime.info.nowTime += tick * anime.info.speed;
	CheckAnimePlayLoop(anime.info);

	// 現在の時刻から各モーフのブレンド割合を計算
	float totalWeight = 0.0f;
	if (anime.info.nowTime <= anime.timelines[0].time)
	{
		// 最初のキーより前の時間は、先頭の値で計算
		SetMorphWeight(m_morphPlayNo, anime.timelines[0].weights[0]);
		totalWeight += anime.timelines[0].weights[0];
	}
	else if (anime.info.nowTime >= anime.timelines.back().time)
	{
		// 最終のキーより後の時間は、最終の値で計算
		SetMorphWeight(m_morphPlayNo, anime.timelines.back().weights[0]);
		totalWeight = anime.timelines.back().weights[0];
	}
	else
	{
		// タイムライン中の値は、前後のキーの補間で計算
		for (int i = 1; i < anime.timelines.size(); ++i)
		{
			if (anime.info.nowTime > anime.timelines[i].time) { continue; }

			// 補間の割合を計算
			auto start	= anime.timelines[i - 1];
			auto end	= anime.timelines[i];
			float rate = (anime.info.nowTime - start.time) / (end.time - start.time);

			// 複数モーフのブレンド率を計算
			for (int j = 0; j < anime.morphs.size(); ++j) {
				float weight = end.weights[j] * rate + start.weights[j] * (1.0f - rate);
				SetMorphWeight(anime.morphs[j], weight);
				totalWeight += weight;
			}
			break;
		}
	}

	// アニメーション対象以外のモーフのブレンドを変更
	float weight = 1.0f - std::min(1.0f, totalWeight);
	for (int i = 0; i < m_morphes.size(); ++i) {
		bool isAnime = false;
		for (int j = 0; j < anime.morphs.size(); ++j) {
			if (i == anime.morphs[j]) {
				isAnime = true;
				break;
			}
		}

		if (!isAnime) {
			SetMorphWeight(i, m_morphes[i].weight * weight);
		}
	}
}

/*************************//*
@brief		| モーフ合成時の頂点データの計算
@param[out]	| out：合成先データ
@param[in]	| in：合成元データ
@param[in]	| weight：元データの合成割合
*//*************************/
void Model::AddMorphVtxWeight(Vertex* out, MorphVertex in, float weight)
{
	out->pos.x += in.pos.x * weight;
	out->pos.y += in.pos.y * weight;
	out->pos.z += in.pos.z * weight;
	out->normal.x += in.normal.x * weight;
	out->normal.y += in.normal.y * weight;
	out->normal.z += in.normal.z * weight;
	out->uv.x += in.uv.x * weight;
	out->uv.y += in.uv.y * weight;
}

