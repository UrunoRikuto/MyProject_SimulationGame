/**************************************************//*
	@file	| FlockEscapeAI.cpp
	@brief	| 集団逃避AIクラスの定義
	@note	| 群れで逃避行動を行うAIを定義
*//**************************************************/
#include "FlockEscapeAI.h"
#include "Oparation.h"

/*****************************************//*
	@brief　	| 引数付きコンストラクタ
	@param      | params：Boidsパラメータ
*//*****************************************/
CFlockEscapeAI::CFlockEscapeAI(const BoidsParams& params)
	: CGroupAI(params)
{
}

/*****************************************//*
	 @brief		| 脅威座標の設定
	 @param		| pos：脅威の位置
*//*****************************************/
void CFlockEscapeAI::SetThreatPosition(const DirectX::XMFLOAT3& pos)
{
	m_ThreatPos = pos;
	m_HasThreat = true;
}

/*****************************************//*
	 @brief		| 逃避（脅威から遠ざかる）
	 @param		| pos：現在位置
	 @param		| vel：現在速度
	 @param		| threat：脅威位置
	 @param		| maxSpeed：最大速度
	 @param		| maxForce：最大ベクトル長
	 @return	| ステアリングベクトル
*//*****************************************/
DirectX::XMFLOAT3 CFlockEscapeAI::Flee(
	const DirectX::XMFLOAT3& pos,
	const DirectX::XMFLOAT3& vel,
	const DirectX::XMFLOAT3& threat,
	float maxSpeed,
	float maxForce)
{
	// 脅威から遠ざかるベクトル
	DirectX::XMFLOAT3 away = pos - threat;

	// 距離が極端に小さい場合はゼロベクトルを返す
	float dist = StructMath::Length(away);
	if (dist < 0.001f) return { 0,0,0 };

	// 脅威から遠ざかる理想速度ベクトル
	DirectX::XMFLOAT3 desired = StructMath::Normalize(away) * maxSpeed;

	// ステアリングベクトル計算
	DirectX::XMFLOAT3 steer = desired - vel;

	// 最大力で制限して返す
	return Limit(steer, maxForce);
}

/*****************************************//*
	@brief　	| 動物の行動を更新
	@param      | pos：動物の現在位置
	@param      | vel：動物の現在速度
	@param      | neighbors：近隣の動物情報リスト
	@return     | ステアリングベクトル
*//*****************************************/
DirectX::XMFLOAT3 CFlockEscapeAI::UpdateAI(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& vel, const std::vector<BoidsNeighbor>& neighbors)
{
	BoidsParams params = m_BoidsParams;

	// 脅威がいる場合
	if (m_HasThreat)
	{
		// 逃避用にBoidsパラメータを調整
		params.fWeightSeparation = m_EscapeSeparationWeight;
		params.fWeightCohesion = m_EscapeCohesionWeight;
		params.fWeightAlignment *= m_EscapeAlignmentScale;
	}

	// ボイドステアリングの計算
	DirectX::XMFLOAT3 steering = BoidsSteering::Compute(pos, vel, neighbors, params);

	// 脅威がいる場合
	if (m_HasThreat)
	{
		// 脅威との距離を計算
		const float dist = StructMath::Distance(pos, m_ThreatPos);

		// 逃避の強さを距離に応じて調整
		float t = 1.0f - (dist / m_FleeRadius);
		if (t < 0.0f) t = 0.0f;
		if (t > 1.0f) t = 1.0f;

		// 逃避ステアリングを加算
		const float scale = 0.2f + 0.8f * t;

		// 逃避ステアリングを加算
		steering += Flee(pos, vel, m_ThreatPos, m_BoidsParams.fMaxSpeed, m_BoidsParams.fMaxForce) * (m_FleeWeight * scale);
	}

	// ステアリングベクトルを返す
	return steering;
}