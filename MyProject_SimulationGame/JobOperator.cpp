/**************************************************//*
	@file	| JobManager.cpp
	@brief	| 職業管理クラスのcppファイル
	@note	| 職業管理クラスの処理を定義
*//**************************************************/
#include "JobOperator.h"

/****************************************//*
	@brief　	| 職業名から職業ストラテジーオブジェクトを生成する関数
	@param		| name：職業名の文字列
	@param		| owner：職業を所有するオブジェクトの参照
　	@return　　	| 生成した職業ストラテジーポインタ
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
		pJob = std::make_unique<CWoodGatherer_Job>();
	}
	else if (name == JobName::StoneGatherer)
	{
		pJob = std::make_unique<CStoneGatherer_Job>();
	}
	else if (name == JobName::Builder)
	{
		pJob = std::make_unique<CBuilder_Job>();
	}
	else if (name == JobName::Smith)
	{
		pJob = std::make_unique<CSmith_Job>();
	}
	else if (name == JobName::Cook)
	{
		pJob = std::make_unique<CCook_Job>();
	}
	else if (name == JobName::Farmer)
	{
		pJob = std::make_unique<CFarmer_Job>();
	}
	else if(name == JobName::GrassGatherer)
	{
		pJob = std::make_unique<CGrassGatherer_Job>();
	}

	// 職業が見つからなかった場合、無職を設定
	if (pJob == nullptr)
	{
		MessageBox(nullptr, "指定された職業名が見つかりません。無職を設定します。", "職業エラー", MB_OK | MB_ICONWARNING);
		// デフォルトで無職を設定
		pJob = std::make_unique<CNeet_Job>();
	}

	// 所有オブジェクトを設定
	pJob->SetOwner(owner);

	// 生成した職業ストラテジーポインタを返す
	return pJob;
}