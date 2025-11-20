/**************************************************//*
	@file	| StoneGatherer_Job.cpp
	@brief	| 石を探して収集する職業クラスのcppファイル
	@note	| 石を探して収集する職業の処理を実装
			| CCollect_Strategyを継承
*//**************************************************/
#include "StoneGatherer_Job.h"
#include "Main.h"
#include "Stone.h"

/******************************************//*
	@brief　	| 標的を探す処理
	@note		| 石オブジェクトを探す処理を実装
*//******************************************/
void CStoneGatherer_Job::SearchTarget()
{
	// 石オブジェクトを探す
	m_pTarget = GetScene()->GetGameObject<CStone>(m_pOwner->GetPos());
}
