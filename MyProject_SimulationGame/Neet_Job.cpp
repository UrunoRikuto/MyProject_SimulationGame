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

/******************************************//*
	@brief　	| インスペクター表示処理
	@param		| isEnd：true:ImGuiのEnd()を呼ぶ false:呼ばない
	@return		| 表示した項目数
	@note		| ImGuiを使用してオブジェクトのパラメータを表示、編集する
*//******************************************/
int CNeet_Job::Inspecter(bool isEnd)
{
	int nItemCount = 0;

	ImGui::BeginChild("Builder Job Inspector", ImVec2(0, 150), true);

	// 職業名の表示
	ImGui::Text(std::string("JobName:" + GetJobName()).c_str());

	ImGui::EndChild();
	nItemCount++;

	if(isEnd)
	{
		ImGui::EndChild();
		ImGui::End();
		// 表示項目のカウントを増やす
		nItemCount++;
	}

	return nItemCount;
}
