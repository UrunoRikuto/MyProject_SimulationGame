/**************************************************//*
	@file	| WoodCollect_Job.cpp
	@brief	| 木材収集職業クラスのcppファイル
	@note	| 木材収集職業の処理を実装
			| CCollect_Strategyを継承
*//**************************************************/
#include "WoodCollect_Job.h"
#include "Main.h"
#include "Wood.h"

/******************************************//*
	@brief　	| 標的を探す処理
	@note		| 木オブジェクトを探す処理を実装
*//******************************************/
void CWoodCollect_Job::SearchTarget()
{
	// 標的にしている木がなければ木オブジェクトを探す
	if (m_pTarget == nullptr)
	{
		m_pTarget = GetScene()->GetGameObject<CWood>(m_pOwner->GetPos());
	}
}
