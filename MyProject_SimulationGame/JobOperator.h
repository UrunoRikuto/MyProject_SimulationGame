/**************************************************//*
	@file	| JobManager.h
	@brief	| 職業管理クラスのhファイル
	@note	| 職業管理クラスの処理を定義
*//**************************************************/
#pragma once
#include <string>
#include <vector>
#include <memory>

// @brief 職業名を管理する名前空間
namespace JobName
{
	// 無職
	const std::string Neet = "Neet";
	// 木材収集職業
	const std::string WoodGatherer = "WoodGatherer";
	// 石収集職業
	const std::string StoneGatherer = "StoneGatherer";

}

// @brief 登録されている職業名の配列
const std::vector<std::string> JobNames = {
	JobName::Neet,
	JobName::WoodGatherer,
	JobName::StoneGatherer
};

//=== @brief 職業クラスのインクルード ===//
/*********************//*
	インターフェース
*//*********************/
#include "Job_Strategy.h"

/*********************//*
	無職:(初期設定)
*//*********************/
#include "Neet_Job.h"

/*********************//*
	収集職
*//*********************/
//-木材
#include "WoodGatherer_Job.h"
//-石
#include "StoneGatherer_Job.h"

// @brief 職業ストラテジーを名前から生成するファクトリ関数
// @param name：生成する職業の名前
std::unique_ptr<IJob_Strategy> CreateJobByName(const std::string& name, CHuman& owner);