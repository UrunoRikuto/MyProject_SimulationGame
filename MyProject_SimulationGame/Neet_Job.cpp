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
}

/******************************************//*
	@brief　	| 切り替え処理
	@note		| 無職なので特に何もしない
*//******************************************/
void CNeet_Job::OnChangeJob()
{
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

	ImGui::BeginChild(u8"職業インスペクター", ImVec2(0, 150), true);

	ImGui::Separator();
	ImGui::Text(u8"[職業ステータス]");

	// 職業名の表示
	ImGui::Text(std::string(u8"職業名:" + GetJobName()).c_str());

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

/******************************************//*
	@brief	| 職業ステータスのImGui描画処理
*//******************************************/
void CNeet_Job::DrawJobStatusImGui()
{
	ImGui::BeginChild(u8"無職の職業ステータス", ImVec2(300, 200), true);
	ImGui::Text(u8"虚無");
	ImGui::EndChild();
}
