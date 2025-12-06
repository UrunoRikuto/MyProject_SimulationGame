/**************************************************//*
	@file	| GrassGatherer.cpp
	@brief	| 草を探して収集する職業クラスのcppファイル
	@note	| 草を探して収集する職業の処理を実装
			| CGatherer_Strategyを継承
*//**************************************************/
#include "GrassGatherer_Job.h"
#include "Main.h"
#include "Grass.h"
#include "Human.h"

/******************************************//*
	@brief　	| コンストラクタ
*//******************************************/
CGrassGatherer_Job::CGrassGatherer_Job()
{
	m_Status.m_fWorkPower = 10.0f;
}

/******************************************//*
	@brief　	| 標的を探す処理
	@return		| 採取対象オブジェクトのポインタ
	@note		| 草オブジェクトを探す処理を実装
*//******************************************/
CCollectTarget* CGrassGatherer_Job::SearchTarget(std::vector<ObjectID> vNotTargetIDs)
{
	// 草オブジェクトを探す
	return GetScene()->GetGameObject<CGrass>(m_pOwner->GetPos(), vNotTargetIDs);
}

/******************************************//*
	@brief　	| 採取ツールを持っているかどうかを取得
	@return		| true:持っている false:持っていない
	@note		| 草を集めるためのツールを持っているかどうかを判定する処理を実装
*//******************************************/
bool CGrassGatherer_Job::HasCollectTool()
{
	// 収集ツールを持っていなければfalseを返す
	if (m_pOwner->GetToolItem() == nullptr)return false;

	// 収集ツールが鎌であればtrueを返す
	if (m_pOwner->GetToolItem()->GetItemType() == CItem::ITEM_TYPE::Sickle)return true;

	// それ以外はfalseを返す
	return false;
}