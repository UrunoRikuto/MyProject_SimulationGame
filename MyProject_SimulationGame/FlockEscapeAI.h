#pragma once
#include "GroupAI.h"

// @brief 集団逃避型 群れAI
class CFlockEscapeAI final : public CGroupAI
{
public:
	CFlockEscapeAI(const BoidsParams& params);

	DirectX::XMFLOAT3 UpdateAI(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& vel, const std::vector<BoidsNeighbor>& neighbors) override;

	// Threat
	void SetThreatPosition(const DirectX::XMFLOAT3& pos);
	bool HasThreat() const { return m_HasThreat; }
	const DirectX::XMFLOAT3& GetThreatPosition() const { return m_ThreatPos; }
	void ClearThreat() { m_HasThreat = false; }

	// @brief逃避中の密集具合を調整（密集しすぎ防止）
	void SetEscapeTuning(float separationWeight, float cohesionWeight)
	{
		m_EscapeSeparationWeight = separationWeight;
		m_EscapeCohesionWeight = cohesionWeight;
	}

	// @brief逃避中の整列重み倍率（全員が同じ向きに揃いすぎる対策）
	void SetEscapeAlignmentScale(float scale) { m_EscapeAlignmentScale = scale; }

	// @brief逃避強さ調整
	void SetFleeParams(float fleeWeight, float fleeRadius)
	{
		m_FleeWeight = fleeWeight;
		m_FleeRadius = fleeRadius;
	}

private:
	static DirectX::XMFLOAT3 Flee(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& vel, const DirectX::XMFLOAT3& threat, float maxSpeed, float maxForce);

private:
	bool m_HasThreat = false;
	DirectX::XMFLOAT3 m_ThreatPos{0.0f,0.0f,0.0f };

	float m_FleeWeight =1.5f;
	float m_FleeRadius =18.0f;

	//逃避中Boids調整
	float m_EscapeSeparationWeight =2.6f;
	float m_EscapeCohesionWeight =0.05f;
	float m_EscapeAlignmentScale =0.6f;
};