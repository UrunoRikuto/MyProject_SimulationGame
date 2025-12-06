/**************************************************//*
	@file	| JobManager.h
	@brief	| 職業管理クラスのhファイル
	@note	| 職業管理クラスの処理を定義
*//**************************************************/
#pragma once
#include <string>
#include <memory>

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

/*********************//*
	加工職
*//*********************/
//-建築
#include "Builder_Job.h"
//-鍛治
#include "Smith_Job.h"
//-料理
#include "Cook_Job.h"

// @brief 職業ストラテジーを名前から生成するファクトリ関数
// @param name：生成する職業の名前
// @param owner：職業を所有するオブジェクトの参照
// @return 生成した職業ストラテジーのユニークポインタ
std::unique_ptr<IJob_Strategy> CreateJobByName(const std::string& name, CHuman& owner);
