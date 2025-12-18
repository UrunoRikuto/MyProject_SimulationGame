#include "FlockEscapeAI.h"
#include "Oparation.h"

static DirectX::XMFLOAT3 Limit(const DirectX::XMFLOAT3& v, float maxLen)
{
	float len = StructMath::Length(v);
	if (len > maxLen && len > 0.0001f) return v * (maxLen / len);
	return v;
}

CFlockEscapeAI::CFlockEscapeAI(const BoidsParams& params)
	: CGroupAI(params)
{
}

void CFlockEscapeAI::SetThreatPosition(const DirectX::XMFLOAT3& pos)
{
	m_ThreatPos = pos;
	m_HasThreat = true;
}

DirectX::XMFLOAT3 CFlockEscapeAI::Flee(
	const DirectX::XMFLOAT3& pos,
	const DirectX::XMFLOAT3& vel,
	const DirectX::XMFLOAT3& threat,
	float maxSpeed,
	float maxForce)
{
	// threat -> pos（脅威から遠ざかる方向）
	DirectX::XMFLOAT3 away = pos - threat;
	float dist = StructMath::Length(away);
	if (dist < 0.001f) return { 0,0,0 };

	DirectX::XMFLOAT3 desired = StructMath::Normalize(away) * maxSpeed;
	DirectX::XMFLOAT3 steer = desired - vel;
	return Limit(steer, maxForce);
}

DirectX::XMFLOAT3 CFlockEscapeAI::UpdateAI(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& vel, const std::vector<BoidsNeighbor>& neighbors)
{
	// 逃避中は密集しすぎないようにBoids重みを調整
	BoidsParams params = m_BoidsParams;
	if (m_HasThreat)
	{
		params.fWeightSeparation = m_EscapeSeparationWeight;
		params.fWeightCohesion = m_EscapeCohesionWeight;
	}

	DirectX::XMFLOAT3 steering = BoidsSteering::Compute(pos, vel, neighbors, params);

	// 逃避（脅威共有時のみ）
	if (m_HasThreat)
	{
		// 近い時だけ強く（遠い時に逃げ続けない保険）
		float dist = StructMath::Distance(pos, m_ThreatPos);
		if (dist <= m_FleeRadius)
		{
			steering += Flee(pos, vel, m_ThreatPos, m_BoidsParams.fMaxSpeed, m_BoidsParams.fMaxForce) * m_FleeWeight;
		}
	}

	return steering;
}