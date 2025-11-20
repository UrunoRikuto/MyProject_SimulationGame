/**************************************************//*
	@file	| WoodCollect_Job.cpp
	@brief	| 木材収集職業クラスのcppファイル
	@note	| 木材収集職業の処理を実装
			| CCollect_Strategyを継承
*//**************************************************/
#include "WoodCollect_Job.h"
#include "ImguiSystem.h"

/******************************************//*
	@brief　	| 仕事処理
	@note		| Woodのオブジェクトタグが付いたオブジェクトを探す処理を実装
				| 見つかったら戦闘処理(AがBの体力を減らすシステム)を利用して木材を収集する処理を実装
*//******************************************/
void CWoodCollect_Job::DoWork()
{

	// ImGuiデバッグログに出力
	CImguiSystem::GetInstance()->AddDebugLog("WoodCollect_Job: Collecting wood...");
}