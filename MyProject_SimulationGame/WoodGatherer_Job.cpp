/**************************************************//*
	@file	| WoodGatherer_Job.cpp
	@brief	| 木を探して収集する職業クラスのcppファイル
	@note	| 木を探して収集する職業の処理を実装
			| CCollect_Strategyを継承
*//**************************************************/
#include "WoodGatherer_Job.h"
#include "Main.h"
#include "Wood.h"
#include "Human.h"

/******************************************//*
	@brief　	| コンストラクタ
*//******************************************/
CWoodGatherer_Job::CWoodGatherer_Job()
	: CGatherer_Strategy()
{
	m_Status.m_fWorkPower = 10.0f;
}

/******************************************//*
	@brief　	| 標的を探す処理
	@return		| 採取対象オブジェクトのポインタ
	@note		| 木オブジェクトを探す処理を実装
*//******************************************/
CCollectTarget* CWoodGatherer_Job::SearchTarget(std::vector<ObjectID> vNotTargetIDs)
{
	// 木オブジェクトを探す
	return GetScene()->GetGameObject<CWood>(m_pOwner->GetPos(), vNotTargetIDs);
}
