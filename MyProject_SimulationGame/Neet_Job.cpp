/**************************************************//*
	@file	| Neet_Job.cpp
	@brief	| 無職クラスのcppファイル
	@note	| 無職の処理を実装
			| IJob_Strategyを継承
*//**************************************************/
#include "Neet_Job.h"
#include "ImguiSystem.h"

/******************************************//* 
	@brief　	| コンストラクタ
*//******************************************/
CNeet_Job::CNeet_Job()
	: IJob_Strategy()
{
	m_Status.m_fWorkPower = 0.0f;
}

/******************************************//*
	@brief　	| 仕事処理
	@note		| 無職なので何もしない
*//******************************************/
void CNeet_Job::DoWork()
{
	// ImGuiデバッグログに出力
	CImguiSystem::GetInstance()->AddDebugLog("Neet_Job: Doing nothing...");
}