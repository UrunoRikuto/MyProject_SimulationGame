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

	// @brief 標的位置設定
	// @param pos：標的の位置
	void SetTargetPosition(const DirectX::XMFLOAT3& pos);

	// @brief 標的があるか
	// @return 標的がある場合true
	bool HasTarget() const { return m_HasTarget; }

	// @brief 標的位置取得
	// @return 標的の位置参照
	const DirectX::XMFLOAT3& GetTargetPosition() const { return m_TargetPos; }

	// @brief 標的解除
	void ClearTarget() { m_HasTarget = false; }

	// @brief ホーム位置設定
	// @param pos：ホームの位置
	void SetHomePosition(const DirectX::XMFLOAT3& pos);

private:
	// @brief 目標位置へ向かうステアリング計算
	// @param pos：現在位置
	// @param vel：現在速度
	// @param target：目標位置
	// @param maxSpeed：最大速度
	// @param maxForce：最大力
	// @return ステアリングベクトル
	static DirectX::XMFLOAT3 Seek(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& vel, const DirectX::XMFLOAT3& target, float maxSpeed, float maxForce);
	
	// @brief 0〜1の乱数取得
	static float Rand01();

	// @brief XZ平面上のランダム単位ベクトル取得
	static DirectX::XMFLOAT3 RandomDirXZ();

private:
	// @brief 標的がいるかどうかのフラグ
	bool m_HasTarget = false;
	// @brief 標的位置
	DirectX::XMFLOAT3 m_TargetPos{ 0.0f, 0.0f, 0.0f };

	// @brief ホーム位置があるかどうかのフラグ
	bool m_HasHome = false;
	// @brief ホーム位置
	DirectX::XMFLOAT3 m_HomePos{ 0.0f, 0.0f, 0.0f };

	// @brief パトロール目標点
	DirectX::XMFLOAT3 m_PatrolPoint{ 0.0f, 0.0f, 0.0f };
	// @brief パトロール目標点があるかどうかのフラグ
	bool m_HasPatrolPoint = false;

	// @brief パトロール半径
	float m_PatrolRadius = 25.0f;
	// @brief パトロール到達距離
	float m_PatrolReach = 3.0f;
	// @brief パトロール再経路探索間隔
	float m_RepathInterval = 4.0f;
	// @brief パトロール再経路探索タイマー
	float m_RepathTimer = 0.0f;
	// @brief パトロールシーク重み
	float m_PatrolSeekWeight = 1.0f;
};

