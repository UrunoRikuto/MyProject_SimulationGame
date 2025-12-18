/**************************************************//*
	@file	| BoidsSteering.cpp
	@brief	| Boidsのステアリングクラス実装
	@note	| Boidsのステアリング処理
*//**************************************************/
#include "BoidsSteering.h"
#include "DirectXMath.h"

// @brief ベクトルの長さを制限
// @param v ベクトル
// @param maxLen 最大長さ
static Vec3 Limit(const Vec3& v, float maxLen)
{
    float len = Length(v);
    if (len > maxLen && len > 0.0001f)
        return v * (maxLen / len);
    return v;
}

/****************************************//*
	@brief　	| Boidsのステアリング力を計算
    @param      | selfPos：自身の位置
    @param      | selfVel：自身の速度
    @param      | neighbors：近隣情報リスト
    @param      | params：Boidsパラメータ
    @return     | ステアリングベクトル
*//****************************************/
Vec3 BoidsSteering::Compute(const Vec3& selfPos, const Vec3& selfVel, const std::vector<BoidsNeighbor>& neighbors, const BoidsParams& params)
{
	// 各ステアリング力の計算
    // 分離
    Vec3 sep = Limit(Separation(selfPos, neighbors, params), params.fMaxSeparationForce);
	// 整列
    Vec3 ali = Limit(Alignment(selfPos, selfVel, neighbors, params), params.fMaxAlignmentForce);
	// 凝集
    Vec3 coh = Limit(Cohesion(selfPos, selfVel, neighbors, params), params.fMaxCohesionForce);

	// 重み付けと合成
    Vec3 force = sep * params.fWeightSeparation
               + ali * params.fWeightAlignment
               + coh * params.fWeightCohesion;

	// 最大力で制限して返す
    return Limit(force, params.fMaxForce);
}

/****************************************//*
    @brief　	| 分離ステアリング力を計算
    @param      | selfPos：自身の位置
    @param      | neighbors：近隣情報リスト
    @param      | params：Boidsパラメータ
    @return     | 分離ステアリングベクトル
*//****************************************/
Vec3 BoidsSteering::Separation(const Vec3& selfPos, const std::vector<BoidsNeighbor>& neighbors, const BoidsParams& params)
{
    Vec3 steer{ 0,0,0 };
    int count = 0;

	// 近隣のBoidsをループ
    for (auto& nb : neighbors)
    {
		// 自身との距離を計算
        Vec3 toMe = selfPos - nb.v3Position;
		// 分離半径以内なら反発力を加算
        float dist = Length(toMe);
		// 分離半径以内なら反発力を加算
        if (dist < params.fSeparationRadius && dist > 0.0001f)
        {
            // 近距離で強く反発するように寄与を距離の二乗で減衰
            float weight = 1.0f / (dist * dist);
			// 反発力を加算
            steer += Normalize(toMe) * weight;
            count++;
        }
    }

    if (count == 0) return { 0,0,0 };
    return steer * (1.0f / count);
}

/****************************************//*
    @brief　	| 整列ステアリング力を計算
    @param      | selfPos：自身の位置
    @param      | selfVel：自身の速度
    @param      | neighbors：近隣情報リスト
    @param      | params：Boidsパラメータ
    @return     | 整列ステアリングベクトル
*//****************************************/
Vec3 BoidsSteering::Alignment(const Vec3& selfPos, const Vec3& selfVel, const std::vector<BoidsNeighbor>& neighbors, const BoidsParams& params)
{
    Vec3 avgVel{ 0,0,0 };
    int count = 0;

	// 近隣のBoidsをループ
    for (auto& nb : neighbors)
    {
		// 自身との距離を計算
        float dist = Length(selfPos - nb.v3Position);

		// 視野半径以内なら平均速度に加算
        if (dist < params.fViewRadius)
        {
			// 速度を加算
            avgVel += nb.v3Velocity;
            count++;
        }
    }
    if (count == 0) return { 0,0,0 };

	// 平均速度を計算
    avgVel = avgVel * (1.0f / count);
    Vec3 desired = Normalize(avgVel) * params.fMaxSpeed;

    // 現在速度との差分（本来のアラインメントのステア）
    return desired - selfVel;
}

/****************************************//*
    @brief　	| 凝集ステアリング力を計算
    @param      | selfPos：自身の位置
    @param      | selfVel：自身の速度
    @param      | neighbors：近隣情報リスト
    @param      | params：Boidsパラメータ
    @return     | 凝集ステアリングベクトル
*//****************************************/
Vec3 BoidsSteering::Cohesion(const Vec3& selfPos, const Vec3& selfVel, const std::vector<BoidsNeighbor>& neighbors, const BoidsParams& params)
{
    Vec3 center{ 0,0,0 };
    int count = 0;

	// 近隣のBoidsをループ
    for (auto& nb : neighbors)
    {
		// 自身との距離を計算
        float dist = Length(selfPos - nb.v3Position);

		// 視野半径以内なら中心位置に加算
        if (dist < params.fViewRadius)
        {
			// 位置を加算
            center += nb.v3Position;
            count++;
        }
    }

    if (count == 0) return { 0,0,0 };

	// 中心位置の平均を計算
    center = center * (1.0f / count);
	// 求心方向の望ましい速度を計算
    Vec3 desired = Normalize(center - selfPos) * params.fMaxSpeed;

    // 現在速度との差分（本来のコヒージョンのステア）
    return desired - selfVel;
}
