/**************************************************//*
	@file	| FlockAttackAI.cpp
	@brief	| 集団攻撃AIクラスの定義
	@note	| 群れで攻撃行動を行うAIを定義
*//**************************************************/
#include "FlockAttackAI.h"
#include "Oparation.h"
#include "Defines.h"


/****************************************//*
	@brief　	| 引数付きコンストラクタ
	@param      | params：Boidsパラメータ
*//****************************************/
CFlockAttackAI::CFlockAttackAI(const BoidsParams& params)
	: CGroupAI(params)
{
}

/****************************************//*
	 @brief		| 標的座標の設定
	 @param		| pos：標的の位置
*//****************************************/
void CFlockAttackAI::SetTargetPosition(const DirectX::XMFLOAT3& pos)
{
	m_TargetPos = pos;
	m_HasTarget = true;
}

/****************************************//*
	 @brief		| ホーム座標の設定
	 @param		| pos：ホームの位置
*//****************************************/
void CFlockAttackAI::SetHomePosition(const DirectX::XMFLOAT3& pos)
{
	m_HomePos = pos;
	m_HasHome = true;

	// 初回のパトロール点を作る
	m_HasPatrolPoint = false;
	m_RepathTimer = 0.0f;
}

/****************************************//*
	 @brief		| 0〜1の乱数取得
	 @return	| 乱数
*//****************************************/
float CFlockAttackAI::Rand01()
{
	return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

/****************************************//*
	 @brief		| XZ平面上のランダムベクトル取得
	 @return	| ランダムベクトル
*//****************************************/
DirectX::XMFLOAT3 CFlockAttackAI::RandomDirXZ()
{
	// -1.0f ～ +1.0f の範囲でランダム生成
	float x = Rand01() * 2.0f - 1.0f;
	float z = Rand01() * 2.0f - 1.0f;

	// 正規化
	DirectX::XMFLOAT3 v{ x, 0.0f, z };
	// 極端に小さい場合はX軸方向に補正
	if (StructMath::Length(v) < 0.0001f) v = { 1.0f, 0.0f, 0.0f };
	return StructMath::Normalize(v);
}

/****************************************//*
	 @brief		| シーク（目的地に向かう）
	 @param		| pos：現在位置
	 @param		| vel：現在速度
	 @param		| target：目的地
	 @param		| maxSpeed：最大速度
	 @param		| maxForce：最大ベクトル長
	 @return	| ステアリングベクトル
*//****************************************/
DirectX::XMFLOAT3 CFlockAttackAI::Seek(
	const DirectX::XMFLOAT3& pos,
	const DirectX::XMFLOAT3& vel,
	const DirectX::XMFLOAT3& target,
	float maxSpeed,
	float maxForce)
{
	// 目的地へのベクトル
	DirectX::XMFLOAT3 to = target - pos;
	// 距離が極端に小さい場合はゼロベクトルを返す
	float dist = StructMath::Length(to);
	if (dist < 0.001f) return { 0,0,0 };

	// 目的地方向への理想速度ベクトル
	DirectX::XMFLOAT3 desired = StructMath::Normalize(to) * maxSpeed;
	// ステアリングベクトル計算
	DirectX::XMFLOAT3 steer = desired - vel;

	// 最大力で制限して返す
	return Limit(steer, maxForce);
}

/****************************************//*
	@brief　	| 動物の行動を更新
	@param      | pos：動物の現在位置
	@param      | vel：動物の現在速度
	@param      | neighbors：近隣の動物情報リスト
	@return     | ステアリングベクトル
*//****************************************/
DirectX::XMFLOAT3 CFlockAttackAI::UpdateAI(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& vel, const std::vector<BoidsNeighbor>& neighbors)
{
	// 群れの中心位置を計算
	DirectX::XMFLOAT3 flockCenter = pos;
	if (!neighbors.empty())
	{
		flockCenter = { 0,0,0 };
		// 全メンバーの位置を加算
		for (const auto& nb : neighbors)
		{
			flockCenter += nb.v3Position;
		}
		// 平均化
		flockCenter = flockCenter * (1.0f / static_cast<float>(neighbors.size()));
	}

	// ボイドステアリングの計算
	BoidsParams params = m_BoidsParams;
	// ターゲットがない場合
	if (!m_HasTarget)
	{
		// 密集しすぎないように調整
		params.fWeightCohesion *=0.65f;
		params.fWeightSeparation *=1.05f;
		params.fWeightAlignment *=1.05f;
	}
	// ステアリング計算
	DirectX::XMFLOAT3 steering = BoidsSteering::Compute(pos, vel, neighbors, params);

	// ターゲットがいる場合
	if (m_HasTarget)
	{
		// ターゲットへ向かう力を追加
		steering += Seek(pos, vel, m_TargetPos, m_BoidsParams.fMaxSpeed, m_BoidsParams.fMaxForce) * 1.3f;

		// 最終ステアリングベクトルを返す
		return steering;
	}

	// ホーム位置の設定（未設定時のみ群れ中心をホームに）
	if (!m_HasHome)
	{
		// ホーム位置を群れの中心に設定
		SetHomePosition(flockCenter);
	}

	// パトロール点の更新
	m_RepathTimer += fDeltaTime;

	// パトロール点までの距離
	const float distToPatrol = StructMath::Length(m_PatrolPoint - flockCenter);

	// 新しいパトロール点が必要か確認
	const bool needNewPoint =
		// まだパトロール点がない or
		(!m_HasPatrolPoint) ||
		// 目標到達 or
		(distToPatrol < m_PatrolReach) ||
		// 一定時間経過
		(m_RepathTimer >= m_RepathInterval);

	// 新しいパトロール点を決定
	if (needNewPoint)
	{
		// リパス用タイマーリセット
		m_RepathTimer = 0.0f;
		// パトロール点設定済みに
		m_HasPatrolPoint = true;

		// ランダム方向ベクトル取得
		DirectX::XMFLOAT3 dir = RandomDirXZ();
		// パトロール点計算
		m_PatrolPoint = m_HomePos + dir * m_PatrolRadius;
	}

	// パトロール点へ向かう力を追加
	DirectX::XMFLOAT3 patrol = Seek(pos, vel, m_PatrolPoint, m_BoidsParams.fMaxSpeed, m_BoidsParams.fMaxForce);

	// パトロールの重みをかけて加算
	steering += patrol * m_PatrolSeekWeight;

	// 最終ステアリングベクトルを返す
	return steering;
}