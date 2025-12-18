/**************************************************//*
	@file	| FlockAttackAI.cpp
	@brief	| 集団攻撃AIクラスの定義
	@note	| 群れで攻撃行動を行うAIを定義
*//**************************************************/
#include "FlockAttackAI.h"
#include "Oparation.h"
#include "Defines.h"

static DirectX::XMFLOAT3 Limit(const DirectX::XMFLOAT3& v, float maxLen)
{
	float len = StructMath::Length(v);
	if (len > maxLen && len > 0.0001f) return v * (maxLen / len);
	return v;
}

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
	 @param		| pos	標的の位置
*//****************************************/
void CFlockAttackAI::SetTargetPosition(const DirectX::XMFLOAT3& pos)
{
	m_TargetPos = pos;
	m_HasTarget = true;
}

/****************************************//*
	 @brief		| ホーム座標の設定
	 @param		| pos	ホームの位置
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
	float x = Rand01() * 2.0f - 1.0f;
	float z = Rand01() * 2.0f - 1.0f;
	DirectX::XMFLOAT3 v{ x, 0.0f, z };
	if (StructMath::Length(v) < 0.0001f) v = { 1.0f, 0.0f, 0.0f };
	return StructMath::Normalize(v);
}

/****************************************//*
	 @brief		| シーク（目的地に向かう）
	 @param		| pos	現在位置
	 @param		| vel	現在速度
	 @param		| target	目的地
	 @param		| maxSpeed	最大速度
	 @param		| maxForce	最大ベクトル長
	 @return	| 修正ベクトル
*//****************************************/
DirectX::XMFLOAT3 CFlockAttackAI::Seek(
	const DirectX::XMFLOAT3& pos,
	const DirectX::XMFLOAT3& vel,
	const DirectX::XMFLOAT3& target,
	float maxSpeed,
	float maxForce)
{
	DirectX::XMFLOAT3 to = target - pos;
	float dist = StructMath::Length(to);
	if (dist < 0.001f) return { 0,0,0 };

	DirectX::XMFLOAT3 desired = StructMath::Normalize(to) * maxSpeed;
	DirectX::XMFLOAT3 steer = desired - vel;
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
	// 1) 群れ中心（近隣平均。無ければ自分）
	DirectX::XMFLOAT3 flockCenter = pos;
	if (!neighbors.empty())
	{
		flockCenter = { 0,0,0 };
		for (const auto& nb : neighbors) flockCenter += nb.v3Position;
		flockCenter = flockCenter * (1.0f / static_cast<float>(neighbors.size()));
	}

	// 2) Boids（パトロール中は固まり対策でCohesion弱め/Separation強め）
	BoidsParams params = m_BoidsParams;
	if (!m_HasTarget)
	{
		// 徘徊中は凝結を弱め、分離を強めに
		params.fWeightCohesion *=0.65f;
		params.fWeightSeparation *=1.05f;
		params.fWeightAlignment *=1.05f;
	}
	DirectX::XMFLOAT3 steering = BoidsSteering::Compute(pos, vel, neighbors, params);

	// 3) ターゲットがあるなら攻撃
	if (m_HasTarget)
	{
		steering += Seek(pos, vel, m_TargetPos, m_BoidsParams.fMaxSpeed, m_BoidsParams.fMaxForce) * 1.3f;
		return steering;
	}

	// 4) ターゲット無し＝なわばりパトロール
	// ホーム未設定なら「今いる場所」をホーム扱い（群れ生成直後の初期化が楽）
	if (!m_HasHome)
	{
		SetHomePosition(flockCenter);
	}

	// パトロール点の更新：到達 or 一定時間
	m_RepathTimer += fDeltaTime;

	const float distToPatrol = StructMath::Length(m_PatrolPoint - flockCenter);
	const bool needNewPoint =
		(!m_HasPatrolPoint) ||
		(distToPatrol < m_PatrolReach) ||
		(m_RepathTimer >= m_RepathInterval);

	if (needNewPoint)
	{
		m_RepathTimer = 0.0f;
		m_HasPatrolPoint = true;

		// ホームの周りの円周上へ（「巡回感」を出すなら “中心からランダム方向” が簡単でそれっぽい）
		DirectX::XMFLOAT3 dir = RandomDirXZ();
		m_PatrolPoint = m_HomePos + dir * m_PatrolRadius;
	}

	// 群れ中心をパトロール点に引っ張る（各個体は群れ中心に追従しつつ移動）
	// → 目的ができるので固まって停止しにくい
	DirectX::XMFLOAT3 patrol = Seek(pos, vel, m_PatrolPoint, m_BoidsParams.fMaxSpeed, m_BoidsParams.fMaxForce);
	steering += patrol * m_PatrolSeekWeight;

	return steering;
}