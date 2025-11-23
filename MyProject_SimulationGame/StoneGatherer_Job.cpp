/**************************************************//*
	@file	| StoneGatherer_Job.cpp
	@brief	| 石を探して収集する職業クラスのcppファイル
	@note	| 石を探して収集する職業の処理を実装
			| CCollect_Strategyを継承
*//**************************************************/
#include "StoneGatherer_Job.h"
#include "Main.h"
#include "Stone.h"
#include "Human.h"

/******************************************//*
	@brief　	| コンストラクタ
*//******************************************/
CStoneGatherer_Job::CStoneGatherer_Job()
	: CGatherer_Strategy()
{
	m_Status.m_fWorkPower = 10.0f;
}

/******************************************//*
	@brief　	| 標的を探す処理
	@return		| 採取対象オブジェクトのポインタ
	@note		| 石オブジェクトを探す処理を実装
*//******************************************/
CCollectTarget* CStoneGatherer_Job::SearchTarget(std::vector<ObjectID> vNotTargetIDs)
{
	// 石オブジェクトを探す
	return GetScene()->GetGameObject<CStone>(m_pOwner->GetPos(), vNotTargetIDs);
}
