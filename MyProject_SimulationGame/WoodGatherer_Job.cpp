/**************************************************//*
	@file	| WoodGatherer_Job.cpp
	@brief	| 木を探して収集する職業クラスのcppファイル
	@note	| 木を探して収集する職業の処理を実装
			| CCollect_Strategyを継承
*//**************************************************/
#include "WoodGatherer_Job.h"
#include "Main.h"
#include "Wood.h"

/******************************************//*
	@brief　	| 標的を探す処理
	@note		| 木オブジェクトを探す処理を実装
*//******************************************/
void CWoodGatherer_Job::SearchTarget()
{
	// 木オブジェクトを探す
	m_pTarget = GetScene()->GetGameObject<CWood>(m_pOwner->GetPos());
}
