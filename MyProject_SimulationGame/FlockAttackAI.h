/**************************************************//*
	@file	| FlockAttackAI.h
	@brief	| 集団攻撃AIクラスの定義
	@note	| 群れで攻撃行動を行うAIを定義
*//**************************************************/
#pragma once
#include "GroupAI.h"

// @brief 群れ攻撃AIクラス
class CFlockAttackAI : public CGroupAI
{
public:
	// @brief 引数付きコンストラクタ
	CFlockAttackAI(const BoidsParams& params);

	// @brief 動物の行動を更新
	// @param pos：動物の現在位置
	// @param vel：動物の現在速度
	// @param neighbors：近隣の動物情報リスト
	// @return ステアリングベクトル
	DirectX::XMFLOAT3 UpdateAI(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& vel, const std::vector<BoidsNeighbor>& neighbors) override;

	// Hunt
	// @brief 標的位置設定（値で保持）
	void SetTargetPosition(const DirectX::XMFLOAT3& pos);

	// @brief 標的があるか
	bool HasTarget() const { return m_HasTarget; }

	// @brief 標的位置取得（参照）
	const DirectX::XMFLOAT3& GetTargetPosition() const { return m_TargetPos; }

	// @brief 標的解除
	void ClearTarget() { m_HasTarget = false; }

	// Patrol (territory)
	// @brief 自陣地位置設定（値で保持）
	void SetHomePosition(const DirectX::XMFLOAT3& pos);

private:
	static DirectX::XMFLOAT3 Seek(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& vel, const DirectX::XMFLOAT3& target, float maxSpeed, float maxForce);
	static float Rand01();
	static DirectX::XMFLOAT3 RandomDirXZ();

private:
	// --- Hunt target ---
	bool m_HasTarget = false;
	DirectX::XMFLOAT3 m_TargetPos{ 0.0f, 0.0f, 0.0f };

	// --- Patrol territory ---
	bool m_HasHome = false;
	DirectX::XMFLOAT3 m_HomePos{ 0.0f, 0.0f, 0.0f };

	DirectX::XMFLOAT3 m_PatrolPoint{ 0.0f, 0.0f, 0.0f };
	bool m_HasPatrolPoint = false;

	float m_PatrolRadius = 25.0f;     // なわばり半径
	float m_PatrolReach = 3.0f;       // 目標到達判定
	float m_RepathInterval = 4.0f;    // 念のため定期的にも更新
	float m_RepathTimer = 0.0f;

	float m_PatrolSeekWeight = 1.0f;  // 目標へ向かう強さ（Boidsに足す）
};

