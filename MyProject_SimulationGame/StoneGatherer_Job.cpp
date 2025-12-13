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
	m_fWorkPower = 10.0f;
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

/******************************************//*
	@brief　	| 採取ツールを持っているかどうかを取得
	@return		| true:持っている false:持っていない
	@note		| 石を集めるためのツールを持っているかどうかを判定する処理を実装
*//******************************************/
bool CStoneGatherer_Job::HasCollectTool()
{
	// 収集ツールを持っていなければfalseを返す
	if (m_pOwner->GetToolItem() == nullptr)return false;

	// 収集ツールがつるはしであればtrueを返す
	if (m_pOwner->GetToolItem()->GetItemType() == CItem::ITEM_TYPE::Pickaxe)return true;

	// それ以外はfalseを返す
	return false;
}
