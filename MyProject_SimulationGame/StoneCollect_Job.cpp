/**************************************************//*
	@file	| StoneCollect_Job.cpp
	@brief	| 石収集職業クラスのcppファイル
	@note	| 石収集職業の処理を実装
			| CCollect_Strategyを継承
*//**************************************************/
#include "StoneCollect_Job.h"
#include "Main.h"
#include "Stone.h"

/******************************************//*
	@brief　	| 標的を探す処理
	@note		| 石オブジェクトを探す処理を実装
*//******************************************/
void CStoneCollect_Job::SearchTarget()
{
	// 標的にしている石がなければ石オブジェクトを探す
	if (m_pTarget == nullptr)
	{
		m_pTarget = GetScene()->GetGameObject<CStone>(m_pOwner->GetPos());
	}
}
