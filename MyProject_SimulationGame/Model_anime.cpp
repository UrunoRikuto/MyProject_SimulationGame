/**********************************************************************************//*
	@file		|Model_anime.cpp
	@brief		|モデルのアニメーション処理
*//***********************************************************************************/
#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

/*************************//*
@brief		|アニメーションデータの追加読み込み
@param[in]	|file：読み込み先パス
@return		|アニメーション番号
*//*************************/
Model::AnimeNo Model::AddAnimation(const char* file)
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

	// アニメーションチェック
	if (IsError(!pScene->HasAnimations(), "no animation."))
	{
#if MODEL_FORCE_ERROR
		ShowErrorMessage(file, false);
#endif
		return ANIME_NONE;
	}

	// アニメーションデータ確保
	aiAnimation* assimpAnime = pScene->mAnimations[0];
	m_animes.push_back(Animation());
	Animation& anime = m_animes.back();

	// アニメーション設定
	using XMVectorKey = std::pair<float, DirectX::XMVECTOR>;
	using XMVectorKeys = std::map<float, DirectX::XMVECTOR>;
	float fbxToGameFrame = static_cast<float>(assimpAnime->mTicksPerSecond);
	anime.info.totalTime = static_cast<float>(assimpAnime->mDuration) / fbxToGameFrame;
	anime.info.speed = 1.0f;

	anime.channels.resize(assimpAnime->mNumChannels);
	AnimeChannels::iterator channelIt = anime.channels.begin();
	for(auto channelIt = anime.channels.begin(); channelIt != anime.channels.end(); ++channelIt)
	{
		// 対応するチャンネル(ボーン)を探索
		uint32_t channelIdx = static_cast<uint32_t>(channelIt - anime.channels.begin());
		aiNodeAnim* assimpChannel = assimpAnime->mChannels[channelIdx];
		channelIt->node = FindNode(assimpChannel->mNodeName.data);
		if (channelIt->node == NODE_NONE) { continue; }

		// 各キーの値を配列にコピー
		XMVectorKeys keys[3];
		AnimeTimeline& timeline = channelIt->timeline;
		// 位置
		for (UINT i = 0; i < assimpChannel->mNumPositionKeys; ++i)
		{
			aiVectorKey& key = assimpChannel->mPositionKeys[i];
			keys[0].insert(XMVectorKey(static_cast<float>(key.mTime) / fbxToGameFrame,
				DirectX::XMVectorSet(key.mValue.x, key.mValue.y, key.mValue.z, 0.0f)
			));
		}
		// 回転
		for (UINT i = 0; i < assimpChannel->mNumRotationKeys; ++i)
		{
			aiQuatKey& key = assimpChannel->mRotationKeys[i];
			keys[1].insert(XMVectorKey(static_cast<float>(key.mTime) / fbxToGameFrame,
				DirectX::XMVectorSet(key.mValue.x, key.mValue.y, key.mValue.z, key.mValue.w)));
		}
		// 拡縮
		for (UINT i = 0; i < assimpChannel->mNumScalingKeys; ++i)
		{
			aiVectorKey& key = assimpChannel->mScalingKeys[i];
			keys[2].insert(XMVectorKey(static_cast<float>(key.mTime) / fbxToGameFrame,
				DirectX::XMVectorSet(key.mValue.x, key.mValue.y, key.mValue.z, 0.0f)));
		}

		// 各タイムラインの先頭の参照を設定
		XMVectorKeys::iterator it[] = { keys[0].begin(), keys[1].begin(), keys[2].begin() };
		for (int i = 0; i < 3; ++i)
		{
			// キーが一つしかない場合は、参照終了
			if (keys[i].size() == 1)
				++it[i];
		}

		// 各要素ごとのタイムラインではなく、すべての変換を含めたタイムラインの作成
		while (it[0] != keys[0].end() && it[1] != keys[1].end() && it[2] != keys[2].end())
		{
			// 現状の参照位置で一番小さい時間を取得
			float time = anime.info.totalTime;
			for (int i = 0; i < 3; ++i)
			{
				if (it[i] != keys[i].end())
				{
					time = std::min(it[i]->first, time);
				}
			}

			// 時間に基づいて補間値を計算
			DirectX::XMVECTOR result[3];
			for (int i = 0; i < 3; ++i)
			{
				// 先頭のキーより小さい時間であれば、先頭の値を設定
				if (time < keys[i].begin()->first)
				{
					result[i] = keys[i].begin()->second;
				}
				// 最終キーより大きい時間であれば、最終の値を設定
				else if (keys[i].rbegin()->first <= time)
				{
					result[i] = keys[i].rbegin()->second;
					it[i] = keys[i].end();
				}
				// キー同士に挟まれた時間であれば、補間値を計算
				else
				{
					// 参照している時間と同じであれば、次の参照へキーを進める
					if (it[i]->first <= time)
					{
						++it[i];
					}

					// 補間値の計算
					XMVectorKeys::iterator prev = it[i];
					--prev;
					float rate = (time - prev->first) / (it[i]->first - prev->first);
					result[i] = DirectX::XMVectorLerp(prev->second, it[i]->second, rate);
				}
			}

			// 指定時間に基づいたキーを追加
			AnimeTransform transform;
			DirectX::XMStoreFloat3(&transform.translate, result[0]);
			DirectX::XMStoreFloat4(&transform.quaternion, result[1]);
			DirectX::XMStoreFloat3(&transform.scale, result[2]);
			timeline.insert(AnimeKey(time, transform));
		}
	}

	// アニメ番号を返す
	return static_cast<AnimeNo>(m_animes.size() - 1);
}

/*************************//*
@brief		|アニメーション再生
@param[in]	|no：再生するアニメーション番号
@param[in]	|loop：ループ再生フラグ
@param[in]	|speed：再生速度
*//*************************/
void Model::PlayAnime(AnimeNo no, bool loop, float speed)
{
	// 再生チェック
	if (!CheckAnimeNo(no)) { return; }
	if (m_playNo == no) { return; }

	// 合成アニメーションかチェック
	if (no != PARAMETRIC_ANIME)
	{
		// 通常の初期化
		InitAnime(no);
		m_animes[no].info.isLoop	= loop;
		m_animes[no].info.speed		= speed;
	}
	else
	{
		// 合成アニメーションの元になっているアニメーションを初期化
		InitAnime(m_parametric[0]);
		InitAnime(m_parametric[1]);
		m_animes[m_parametric[0]].info.isLoop = loop;
		m_animes[m_parametric[1]].info.isLoop = loop;
		SetParametricBlend(0.0f);
	}

	// 再生アニメーションの設定
	m_playNo = no;
}

/*************************//*
@brief		|アニメーションブレンド再生
@param[in]	|no：再生するアニメーション番号
@param[in]	|blendTime：ブレンドに掛ける時間
@param[in]	|loop：ループフラグ
@param[in]	|speed：再生速度
*//*************************/
void Model::PlayBlend(AnimeNo no, AnimeTime blendTime, bool loop, float speed)
{
	// 再生チェック
	if (!CheckAnimeNo(no)) { return; }

	// 合成アニメーションかチェック
	if (no != PARAMETRIC_ANIME)
	{
		InitAnime(no);
		m_animes[no].info.isLoop = loop;
		m_animes[no].info.speed = speed;
	}
	else
	{
		// 合成アニメーションの元になっているアニメーションを初期化
		InitAnime(m_parametric[0]);
		InitAnime(m_parametric[1]);
		m_animes[m_parametric[0]].info.isLoop = loop;
		m_animes[m_parametric[1]].info.isLoop = loop;
		SetParametricBlend(0.0f);
	}

	// ブレンドの設定
	m_blendTime = 0.0f;
	m_blendTotalTime = blendTime;
	m_blendNo = no;
}

/*************************//*
@brief		|合成元アニメーションの設定
@param[in]	|no1：合成元アニメ1
@param[in]	|no2：合成元アニメ2
*//*************************/
void Model::SetParametric(AnimeNo no1, AnimeNo no2)
{
	// アニメーションチェック
	if (!CheckAnimeNo(no1)) { return; }
	if (!CheckAnimeNo(no2)) { return; }

	// 合成設定
	m_parametric[0] = no1;
	m_parametric[1] = no2;
	SetParametricBlend(0.0f);
}

/*************************//*
@brief		|合成割合の設定
@param[in]	|blendRate：合成割合(0.0f～1.0f)
*//*************************/
void Model::SetParametricBlend(float blendRate)
{
	// 合成元アニメが設定されているか確認
	if (m_parametric[0] == ANIME_NONE || m_parametric[1] == ANIME_NONE) return;

	// 合成割合設定
	m_parametricBlend = blendRate;

	// 割合に基づいてアニメーションの再生速度を設定
	Animation& anime1 = m_animes[m_parametric[0]];
	Animation& anime2 = m_animes[m_parametric[1]];
	float blendTotalTime =
		anime1.info.totalTime * (1.0f - m_parametricBlend) +
		anime2.info.totalTime * m_parametricBlend;
	anime1.info.speed = anime1.info.totalTime / blendTotalTime;
	anime2.info.speed = anime2.info.totalTime / blendTotalTime;
}

/*************************//*
@brief		|アニメーションの再生時間を変更
@param[in]	|no：変更するアニメーション番号
@param[in]	|time：新しい再生時間
*//*************************/
void Model::SetAnimeTime(AnimeNo no, AnimeTime time)
{
	// アニメーションチェック
	if (!CheckAnimeNo(no)) { return; }
	if (no == PARAMETRIC_ANIME) { return; }

	// 再生時間変更
	Animation& anime = m_animes[no];
	anime.info.nowTime = time;
	CheckAnimePlayLoop(anime.info);
}

/*************************//*
@brief		|アニメーションの更新
@param[in]	|tick：更新時間
*//*************************/
void Model::StepAnime(float tick)
{
	// アニメーションの再生確認
	if (m_playNo == ANIME_NONE) { return; }

	//--- アニメーション行列の更新
	// パラメトリック
	if (m_playNo == PARAMETRIC_ANIME || m_blendNo == PARAMETRIC_ANIME)
	{
		CalcAnime(PARAMETRIC0, m_parametric[0]);
		CalcAnime(PARAMETRIC1, m_parametric[1]);
	}
	// メインアニメ
	if (m_playNo != ANIME_NONE && m_playNo != PARAMETRIC_ANIME)
	{
		CalcAnime(MAIN, m_playNo);
	}
	// ブレンドアニメ
	if (m_blendNo != ANIME_NONE && m_blendNo != PARAMETRIC_ANIME)
	{
		CalcAnime(BLEND, m_blendNo);
	}

	// アニメーション行列に基づいて骨行列を更新
	CalcBones(0, DirectX::XMMatrixScaling(m_loadScale, m_loadScale, m_loadScale));

	//--- アニメーションの時間更新
	// メインアニメ
	UpdateAnime(m_playNo, tick);
	// ブレンドアニメ
	if (m_blendNo != ANIME_NONE)
	{
		UpdateAnime(m_blendNo, tick);
		m_blendTime += tick;
		if (m_blendTime <= m_blendTime)
		{
			// ブレンドアニメの自動終了
			m_blendTime = 0.0f;
			m_blendTotalTime = 0.0f;
			m_playNo = m_blendNo;
			m_blendNo = ANIME_NONE;
		}
	}
	// パラメトリック
	if (m_playNo == PARAMETRIC_ANIME || m_blendNo == PARAMETRIC_ANIME)
	{
		UpdateAnime(m_parametric[0], tick);
		UpdateAnime(m_parametric[1], tick);
	}
}

/*************************//*
@brief		|アニメーション情報の初期化
@param[in]	|no：アニメーション番号
*//*************************/
void Model::InitAnime(AnimeNo no)
{
	// アニメの設定なし、パラメトリックで設定されているなら初期化しない
	if (no == ANIME_NONE || no == PARAMETRIC_ANIME) { return; }

	Animation& anime	= m_animes[no];
	anime.info.nowTime	= 0.0f;
	anime.info.speed	= 1.0f;
	anime.info.isLoop	= false;
}

/*************************//*
@brief		|アニメーションの更新
@param[in]	|no：アニメーション番号
@param[in]	|tick：更新時間
*//*************************/
void Model::UpdateAnime(AnimeNo no, float tick)
{
	// アニメーションチェック
	if (!CheckAnimeNo(no)) { return; }
	if (no == PARAMETRIC_ANIME) { return; }

	// アニメーション時間の更新
	Animation& anime = m_animes[no];
	SetAnimeTime(no, anime.info.nowTime + anime.info.speed * tick);
}

/*************************//*
@brief		|アニメーションの計算
@param[in]	|kind：アニメーションの種類
@param[in]	|no：アニメーション番号
*//*************************/
void Model::CalcAnime(AnimePattern kind, AnimeNo no)
{
	// アニメーションチェック
	if (!CheckAnimeNo(no)) { return; }
	if (no == PARAMETRIC_ANIME) { return; }

	Animation& anime = m_animes[no];
	AnimeChannels::iterator channelIt = anime.channels.begin();
	for(auto channelIt = anime.channels.begin(); channelIt != anime.channels.end(); ++channelIt)
	{
		// アニメーションで変更するボーンがなければスキップ
		if (channelIt->node == NODE_NONE) { continue; }

		// タイムラインの設定がなければスキップ
		AnimeTimeline& timeline = channelIt->timeline;
		if (timeline.empty()) { continue; }

		// ボーンの姿勢をタイムラインに沿って更新
		AnimeTransform& transform = m_animeTransform[kind][channelIt->node];
		if (timeline.size() <= 1)
		{
			// キーが一つしかないので値をそのまま使用
			transform = channelIt->timeline[0];
		}
		else
		{
			AnimeTimeline::iterator startIt = timeline.begin();
			if (anime.info.nowTime <= startIt->first)
			{
				// 先頭キーよりも前の時間なら、先頭の値を使用
				transform = startIt->second;
			}
			else if (timeline.rbegin()->first <= anime.info.nowTime)
			{
				// 最終キーよりも後の時間なら、最後の値を使用
				transform = timeline.rbegin()->second;
			}
			else
			{
				// 指定された時間を挟む2つのキーから、補間された値を計算
				AnimeTimeline::iterator nextIt = timeline.upper_bound(anime.info.nowTime);
				startIt = nextIt;
				--startIt;
				float rate = (anime.info.nowTime - startIt->first) / (nextIt->first - startIt->first);
				LerpTransform(&transform, startIt->second, nextIt->second, rate);
			}
		}
	}
}

/*************************//*
@brief		|アニメーション再生方法別に計算されている行列を合成
@param[in]	|index：計算対象のノード
@param[in]	|parent：親の姿勢行列
*//*************************/
void Model::CalcBones(NodeIndex index, const DirectX::XMMATRIX parent)
{
	AnimeTransform transform; // 合成結果の格納先

	// パラメトリックの合成
	if (m_playNo == PARAMETRIC_ANIME || m_blendNo == PARAMETRIC_ANIME)
	{
		LerpTransform(&transform,
			m_animeTransform[PARAMETRIC0][index],
			m_animeTransform[PARAMETRIC1][index],
			m_parametricBlend);

		// のちのブレンドとの合成のために、パラメトリックの結果を再生・ブレンドのいずれかに格納
		if (m_playNo == PARAMETRIC_ANIME) { m_animeTransform[MAIN][index] = transform; }
		if (m_blendNo == PARAMETRIC_ANIME) { m_animeTransform[BLEND][index] = transform; }
	}

	// ブレンドアニメの合成
	if (m_blendNo != ANIME_NONE)
	{
		LerpTransform(&transform,
			m_animeTransform[MAIN][index],
			m_animeTransform[BLEND][index],
			m_blendTime / m_blendTotalTime);
	}
	else
	{
		// 合成の必要がなかったので、メインの情報をそのまま格納
		transform = m_animeTransform[MAIN][index];
	}

	// 該当ノードの姿勢行列を計算
	Node& node = m_nodes[index];
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&transform.translate));
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&transform.quaternion));
	DirectX::XMMATRIX S = DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&transform.scale));
	node.mat = (S * R * T) * parent;

	// 子要素の姿勢を更新
	Children::iterator it = node.children.begin();
	while (it != node.children.end())
	{
		CalcBones(*it, node.mat);
		++it;
	}
}

/*************************//*
@brief		|アニメーションの補間
@param[out]	|pOut：結果の格納先
@param[in]	|a：補間元1
@param[in]	|b：補間元2
@param[in]	|rate：補間割合
*//*************************/
void Model::LerpTransform(AnimeTransform* pOut, const AnimeTransform& a, const AnimeTransform& b, float rate)
{
	// 補間元の情報をベクトルに格納
	DirectX::XMVECTOR vec[][2] = {
		{ DirectX::XMLoadFloat3(&a.translate),	DirectX::XMLoadFloat3(&b.translate) },
		{ DirectX::XMLoadFloat4(&a.quaternion),	DirectX::XMLoadFloat4(&b.quaternion) },
		{ DirectX::XMLoadFloat3(&a.scale),		DirectX::XMLoadFloat3(&b.scale) },
	};

	// 補間(※Quaternionだけは球面補間が必要？
	for (int i = 0; i < 3; ++i)
		vec[i][0] = DirectX::XMVectorLerp(vec[i][0], vec[i][1], rate);

	// 計算結果の格納
	DirectX::XMStoreFloat3(&pOut->translate, vec[0][0]);
	DirectX::XMStoreFloat4(&pOut->quaternion, vec[1][0]);
	DirectX::XMStoreFloat3(&pOut->scale, vec[2][0]);
}