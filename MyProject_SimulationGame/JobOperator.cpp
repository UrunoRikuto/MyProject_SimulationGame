/**************************************************//*
	@file	| JobOperator.cpp
	@brief	| 職業管理クラスのcppファイル
	@note	| 職業管理クラスの処理を実装
*//**************************************************/
#include "JobOperator.h"

/****************************************//*
	@brief　	| 職業ストラテジーを名前から生成するファクトリ関数
	@param		| name：生成する職業の名前
	@return		| 生成した職業ストラテジーポインタ
*//****************************************/
std::unique_ptr<IJob_Strategy> CreateJobByName(const std::string& name)
{
	if (name == JobName::Neet)  return std::make_unique<CNeet_Job>();
	else if (name == JobName::WoodCollect)  return std::make_unique<CWoodCollect_Job>();
	else return nullptr;
}