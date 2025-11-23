/**************************************************//*
	@file	| JobOperator.cpp
	@brief	| 職業管理クラスのcppファイル
	@note	| 職業管理クラスの処理を実装
*//**************************************************/
#include "JobOperator.h"
#include "ImguiSystem.h"
#include "Human.h"

/****************************************//*
	@brief　	| 職業ストラテジーを名前から生成するファクトリ関数
	@param		| name：生成する職業の名前
	@param		| owner：職業を所有するゲームオブジェクト
	@return		| 生成した職業ストラテジーポインタ
*//****************************************/
std::unique_ptr<IJob_Strategy> CreateJobByName(const std::string& name, CHuman& owner)
{
	std::unique_ptr<IJob_Strategy> pJob = nullptr;

	if (name == JobName::Neet)
	{
		pJob = std::make_unique<CNeet_Job>();
	}
	else if (name == JobName::WoodGatherer)
	{
		pJob =  std::make_unique<CWoodGatherer_Job>();
	}
	else if (name == JobName::StoneGatherer)
	{
		pJob =  std::make_unique<CStoneGatherer_Job>();
	}

	// 職業が見つからなかった場合、無職を設定
	if(pJob == nullptr)
	{
		CImguiSystem::GetInstance()->AddDebugLog(("CreateJobByName:Not Find case　[" + name + "]"), false);

		// デフォルトで無職を設定
		pJob = std::make_unique<CNeet_Job>();
	}

	// 所有オブジェクトを設定
	pJob->SetOwner(owner);

	// 生成した職業ストラテジーポインタを返す
	return pJob;
}