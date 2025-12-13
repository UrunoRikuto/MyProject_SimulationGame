/**************************************************//*
	@file	| BoidsSteering.cpp
	@brief	| Boidsのステアリングクラス実装
	@note	| Boidsのステアリングを実装
*//**************************************************/
#include "BoidsSteering.h"
#include "DirectXMath.h"

/****************************************//*
	@brief　	| ステアリング計算
	@param      | selfPos：自身の位置
	@param      | selfVel：自身の速度
	@param      | neighbors：近隣情報リスト
	@param      | params：Boidsパラメータ
	@return     | ステアリングベクトル
*//****************************************/
Vec3 BoidsSteering::Compute(const Vec3& selfPos, const Vec3& selfVel, const std::vector<BoidsNeighbor>& neighbors, const BoidsParams& params)
{
	// 各ルールの力を計算
	// 分離
    Vec3 sep = Separation(selfPos, neighbors, params);
	// 整列
    Vec3 ali = Alignment(selfPos, neighbors, params);
	// 結合
    Vec3 coh = Cohesion(selfPos, neighbors, params);

	// 重み付けして合成
    Vec3 force = sep * params.fWeightSeparation
        + ali * params.fWeightAlignment
        + coh * params.fWeightCohesion;

	// 最大力でクランプ
    float len = Length(force);
    if (len > params.fMaxForce)
        force = force * (params.fMaxForce / len);

    return force;
}

/****************************************//*
    @brief　	| 分離ルール計算
    @param      | selfPos：自身の位置
    @param      | neighbors：近隣情報リスト
    @param      | params：Boidsパラメータ
    @return     | 分離ベクトル
*//****************************************/
Vec3 BoidsSteering::Separation(const Vec3& selfPos, const std::vector<BoidsNeighbor>& neighbors, const BoidsParams& params)
{
    Vec3 steer{ 0,0,0 };
    int count = 0;

	// 近隣エージェントをループ
    for (auto& nb : neighbors)
    {
		// 自身との距離計算
        float dist = Length(selfPos - *nb.v3Position);

		// 分離距離以内なら分離ベクトルに寄与
        if (dist < params.fSeparationRadius && dist > 0.0001f)
        {
			// 近隣エージェントから遠ざかる方向に寄与
            steer += Normalize(selfPos - *nb.v3Position) * (1.0f / dist);

			// カウントアップ
            count++;
        }
    }

	// 平均化
    if (count == 0) return { 0,0,0 };
    return steer * (1.0f / count);
}

/****************************************//*
    @brief　	| 整列ルール計算
    @param      | selfPos：自身の位置
    @param      | neighbors：近隣情報リスト
    @param      | params：Boidsパラメータ
    @return     | 整列ベクトル
*//****************************************/
Vec3 BoidsSteering::Alignment(const Vec3& selfPos, const std::vector<BoidsNeighbor>& neighbors, const BoidsParams& params)
{
    Vec3 avgVel{ 0,0,0 };
    int count = 0;

	// 近隣エージェントをループ
    for (auto& nb : neighbors)
    {
		// 自身との距離計算
        float dist = Length(selfPos - *nb.v3Position);

		// 視界距離以内なら平均速度に寄与
        if (dist < params.fViewRadius)
        {
			// 速度を加算
            avgVel += *nb.v3Velocity;
            count++;
        }
    }
	// 平均化
    if (count == 0) return { 0,0,0 };

	// 平均速度計算
    avgVel = avgVel * (1.0f / count);
	// 目標速度を最大速度にスケーリング
    avgVel = Normalize(avgVel) * params.fMaxSpeed;

	// 目標速度から現在速度を引いたものが整列ベクトル
    return avgVel - selfPos;
}

/****************************************//*
    @brief　	| 結合ルール計算
    @param      | selfPos：自身の位置
    @param      | neighbors：近隣情報リスト
    @param      | params：Boidsパラメータ
    @return     | 結合ベクトル
*//****************************************/
Vec3 BoidsSteering::Cohesion(const Vec3& selfPos, const std::vector<BoidsNeighbor>& neighbors, const BoidsParams& params)
{
    Vec3 center{ 0,0,0 };
    int count = 0;

	// 近隣エージェントをループ
    for (auto& nb : neighbors)
    {
		// 自身との距離計算
        float dist = Length(selfPos - *nb.v3Position);

		// 視界距離以内なら中心点に寄与
        if (dist < params.fViewRadius)
        {
			// 位置を加算
            center += *nb.v3Position;
            count++;
        }
    }

	// 平均化
    if (count == 0) return { 0,0,0 };

	// 中心点計算
    center = center * (1.0f / count);

	// 目標位置への望ましい速度を計算
    Vec3 desired = Normalize(center - selfPos) * params.fMaxSpeed;

	// 目標速度から現在速度を引いたものが結合ベクトル
    return desired;
}
