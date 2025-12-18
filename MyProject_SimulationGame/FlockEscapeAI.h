#pragma once
#include "GroupAI.h"

// @brief W’c“¦”ğŒ^ ŒQ‚êAI
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

	// @brief“¦”ğ’†‚Ì–§W‹ï‡‚ğ’²®i–§W‚µ‚·‚¬–h~j
	void SetEscapeTuning(float separationWeight, float cohesionWeight)
	{
		m_EscapeSeparationWeight = separationWeight;
		m_EscapeCohesionWeight = cohesionWeight;
	}

private:
	static DirectX::XMFLOAT3 Flee(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& vel, const DirectX::XMFLOAT3& threat, float maxSpeed, float maxForce);

private:
	bool m_HasThreat = false;
	DirectX::XMFLOAT3 m_ThreatPos{0.0f,0.0f,0.0f };

	float m_FleeWeight =1.5f;
	float m_FleeRadius =18.0f;

	//“¦”ğ’†Boids’²®
	float m_EscapeSeparationWeight =2.6f;
	float m_EscapeCohesionWeight =0.05f;
};