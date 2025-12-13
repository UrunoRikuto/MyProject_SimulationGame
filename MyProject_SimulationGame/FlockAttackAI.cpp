/**************************************************//*
	@file	| FlockAttackAI.cpp
	@brief	| 群れ攻撃AIクラスの定義
	@note	| 群れで攻撃行動を行うAIを定義
*//**************************************************/
#include "FlockAttackAI.h"

/****************************************//*
	@brief　	| 引数付きコンストラクタ
	@param      | params：Boidsパラメータ
*//****************************************/
CFlockAttackAI::CFlockAttackAI(const BoidsParams& params)
	: CGroupAI(params)
	, m_TargetPos(nullptr)
{
}

/****************************************//*
	@brief　	| 動物の行動を更新
	@param      | pos：動物の現在位置
	@param      | vel：動物の現在速度
	@param      | neighbors：近隣の動物情報リスト
*//****************************************/
Vec3 CFlockAttackAI::UpdateAI(const Vec3& pos,const Vec3& vel,const std::vector<BoidsNeighbor>& neighbors)
{
    // Boids の基本ステアリング（群れ挙動）
    Vec3 steering = ComputeGroupSteering(pos, vel, neighbors);

    // 攻撃対象が存在するなら「襲う」ベクトルを加算
    if (m_TargetPos)
    {
		// 標的へのベクトル計算
        Vec3 toTarget = *m_TargetPos - pos;
		// その距離を計算
        float dist = Length(toTarget);

		// 距離が十分にあるなら襲うベクトルを計算
        if (dist > 0.001f)
        {
            // 追跡：近づくほどスピードを緩める
			Vec3 desired = Normalize(toTarget) * m_BoidsParams.fMaxSpeed;

            // 距離が近いと減速（柔らかく接近する）
            if (dist < 3.0f)
            {
                desired = desired * (dist / 3.0f);
            }

			// 望ましい速度から現在速度を引いたものが襲うベクトル
            Vec3 attack = desired - vel;

            // 重みづけして加算
            steering += attack * 1.3f;
        }
    }

	// ステアリングを返す
    return steering;
};