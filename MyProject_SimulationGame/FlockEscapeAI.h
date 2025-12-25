/**************************************************//*
	@file	| FlockEscapeAI.h
	@brief	| 集団逃避AIクラスの定義
	@note	| 群れで逃避行動を行うAIを定義
			| CGroupAIを継承
*//**************************************************/
#pragma once
#include "GroupAI.h"

// @brief 集団逃避型AIクラス
class CFlockEscapeAI final : public CGroupAI
{
public:
	// @brief コンストラクタ
	// @param params：Boidsパラメータ
	CFlockEscapeAI(const BoidsParams& params);

	// @brief 動物の行動を更新
	// @param pos：動物の現在位置
	// @param vel：動物の現在速度
	// @param neighbors：近隣の動物情報リスト
	// @return ステアリングベクトル
	DirectX::XMFLOAT3 UpdateAI(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& vel, const std::vector<BoidsNeighbor>& neighbors) override;

	// @brief 脅威位置設定
	// @param pos：脅威の位置
	void SetThreatPosition(const DirectX::XMFLOAT3& pos);

	// @brief 脅威があるか
	// @return 脅威がある場合true
	bool HasThreat() const { return m_HasThreat; }

	// @brief 脅威位置取得
	// @return 脅威の位置参照
	const DirectX::XMFLOAT3& GetThreatPosition() const { return m_ThreatPos; }

	// @brief 脅威解除
	void ClearThreat() { m_HasThreat = false; }

	// @brief 逃避中のBoids調整
	// @param separationWeight：分離重み
	// @param cohesionWeight：凝集重み
	void SetEscapeTuning(float separationWeight, float cohesionWeight)
	{
		// 逃避中Boids調整値設定
		m_EscapeSeparationWeight = separationWeight;
		// 凝集重み設定
		m_EscapeCohesionWeight = cohesionWeight;
	}

	// @brief 逃避中の整列強さ調整
	// @param scale：整列強さスケール
	void SetEscapeAlignmentScale(float scale) { m_EscapeAlignmentScale = scale; }

	// @brief 逃避パラメータ設定
	// @param fleeWeight：逃避重み
	// @param fleeRadius：逃避半径
	void SetFleeParams(float fleeWeight, float fleeRadius)
	{
		m_FleeWeight = fleeWeight;
		m_FleeRadius = fleeRadius;
	}

private:
	// @brief 逃避（脅威から遠ざかる）
	// @param pos：現在位置
	// @param vel：現在速度
	// @param threat：脅威位置
	// @param maxSpeed：最大速度
	// @param maxForce：最大ベクトル長
	// @return ステアリングベクトル
	static DirectX::XMFLOAT3 Flee(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& vel, const DirectX::XMFLOAT3& threat, float maxSpeed, float maxForce);

private:
	// @brief 脅威がいるかどうかのフラグ
	bool m_HasThreat = false;
	// @brief 脅威位置
	DirectX::XMFLOAT3 m_ThreatPos{0.0f,0.0f,0.0f };

	
	// @brief 逃避重み
	float m_FleeWeight =1.5f;
	// @brief 逃避半径
	float m_FleeRadius =18.0f;

	// @brief 逃避中の分離重み
	float m_EscapeSeparationWeight =2.6f;
	// @brief 逃避中の凝集重み
	float m_EscapeCohesionWeight =0.05f;
	// @brief 逃避中の整列強さスケール
	float m_EscapeAlignmentScale =0.6f;
};