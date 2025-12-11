/**************************************************//*
	@file	| Skill.cpp
	@brief	| スキルクラス実装
	@note	| 人間が生成時にもつスキル
*//**************************************************/
#include "Skill.h"
#include "Main.h"
#include <vector>
#include "JobOperator.h"
#include "ImguiSystem.h"

/****************************************//*
	@brief　	| コンストラクタ
*//****************************************/
CSkill::CSkill()
	: m_eSkillTarget(SkillTarget::None)
	, m_eSkillEffect(SkillEffect::None)
{
	// スキル効果をランダムに設定
	m_eSkillTarget = static_cast<SkillTarget>(rand() % static_cast<int>(SkillTarget::MAX));

	// スキル対象に応じてスキル効果を設定
	std::vector<SkillEffect> randomEffects;

	switch (m_eSkillTarget)
	{
	case CSkill::SkillTarget::None:
		randomEffects.push_back(SkillEffect::None);
		break;
		// 基礎ステータス
	case CSkill::SkillTarget::MaxStamina:
	case CSkill::SkillTarget::MaxHealth:
		randomEffects.push_back(SkillEffect::Multiplier_1_5);
		randomEffects.push_back(SkillEffect::Multiplier_2_0);
		break;
		// 汎用職業
	case CSkill::SkillTarget::Collector:
	case CSkill::SkillTarget::Crafter:
		randomEffects.push_back(SkillEffect::Multiplier_2_0);
		break;
		// 個別職業
	case CSkill::SkillTarget::WoodGatherer:
	case CSkill::SkillTarget::StoneGatherer:
	case CSkill::SkillTarget::GrassGatherer:
	case CSkill::SkillTarget::Builder:
	case CSkill::SkillTarget::Cook:
	case CSkill::SkillTarget::Smith:
		randomEffects.push_back(SkillEffect::Multiplier_1_5);
		break;
	default:
		MessageBox(nullptr, "タイプケースが存在しません。", "CSkill::CSkill()", MB_OK | MB_ICONERROR);
		break;
	}

	// ランダムにスキル効果を選択
	if (randomEffects.empty())m_eSkillEffect = SkillEffect::None;
	else m_eSkillEffect = randomEffects[rand() % randomEffects.size()];

}

/****************************************//*
	@brief　	| デストラクタ
*//****************************************/
CSkill::~CSkill()
{
}

/****************************************//*
	@brief　	| スキルの効果を適応
	@param		| Out_StatusValue：ステータス値
	@param		| In_Target：スキル対象
*//****************************************/
void CSkill::ApplySkillBuff(float& Out_StatusValue, SkillTarget In_Target)
{
	// スキル対象かどうか確認
	bool IsTarget = (m_eSkillTarget == In_Target);

	// スキル対象でなければ処理終了
	if (!IsTarget)return;

	// スキル効果を適応
	switch (m_eSkillEffect)
	{
	case CSkill::SkillEffect::Multiplier_1_5:
		Out_StatusValue *= 1.5f;
		break;
	case CSkill::SkillEffect::Multiplier_2_0:
		Out_StatusValue *= 2.0f;
		break;
	}
}

/****************************************//*
	@brief　	| スキルの効果を適応(職業)
	@param   	| Out_WorkPower：労働力
	@param	    | In_Job：対象の職業ポインタ
*//****************************************/
void CSkill::ApplySkillBuff(float& Out_WorkPower, IJob_Strategy* In_Job)
{
	// Castを用いて確認
	bool IsTargetJob = false;
	switch (m_eSkillTarget)
	{
	case CSkill::SkillTarget::Collector:
		// 収集職かどうか確認
		if (dynamic_cast<CCollect_Strategy*>(In_Job) != nullptr)IsTargetJob = true;
		break;
	case CSkill::SkillTarget::Crafter:
		// 加工職かどうか確認
		if (dynamic_cast<CCrafter_Strategy*>(In_Job) != nullptr)IsTargetJob = true;
		break;
	case CSkill::SkillTarget::WoodGatherer:
		// 木の採取職かどうか確認
		if (dynamic_cast<CWoodGatherer_Job*>(In_Job) != nullptr)IsTargetJob = true;
		break;
	case CSkill::SkillTarget::StoneGatherer:
		// 石の採取職かどうか確認
		if (dynamic_cast<CStoneGatherer_Job*>(In_Job) != nullptr)IsTargetJob = true;
		break;
	case CSkill::SkillTarget::GrassGatherer:
		// 草の採取職かどうか確認
		if (dynamic_cast<CGrassGatherer_Job*>(In_Job) != nullptr)IsTargetJob = true;
		break;
	case CSkill::SkillTarget::Builder:
		// 建築職かどうか確認
		if (dynamic_cast<CBuilder_Job*>(In_Job) != nullptr)IsTargetJob = true;
		break;
	case CSkill::SkillTarget::Cook:
		// 料理職かどうか確認
		if (dynamic_cast<CCook_Job*>(In_Job) != nullptr)IsTargetJob = true;
		break;
	case CSkill::SkillTarget::Smith:
		// 鍛冶職かどうか確認
		if (dynamic_cast<CSmith_Job*>(In_Job) != nullptr)IsTargetJob = true;
		break;
	}

	// スキル対象でなければ処理終了
	if (!IsTargetJob)return;

	// スキル効果を適応
	switch (m_eSkillEffect)
	{
	case CSkill::SkillEffect::Multiplier_1_5:
		Out_WorkPower *= 1.5f;
		break;
	case CSkill::SkillEffect::Multiplier_2_0:
		Out_WorkPower *= 2.0f;
		break;
	}
}

/****************************************//*
	@brief　	| スキルのImGui表示処理
*//****************************************/
void CSkill::DrawSkillImGui()
{
	ImGui::Separator();
	ImGui::Text(u8"[スキル]:");

	// スキル対象の表示
	std::string skillTargetStr;
	switch (m_eSkillTarget)
	{
	case CSkill::SkillTarget::MaxStamina:
		skillTargetStr = u8"最大スタミナ";
		break;
	case CSkill::SkillTarget::MaxHealth:
		skillTargetStr = u8"最大体力";
		break;
	case CSkill::SkillTarget::Collector:
		skillTargetStr = u8"採取職";
		break;
	case CSkill::SkillTarget::Crafter:
		skillTargetStr = u8"生産職";
		break;
	case CSkill::SkillTarget::WoodGatherer:
		skillTargetStr = JobName::WoodGatherer;
		break;
	case CSkill::SkillTarget::StoneGatherer:
		skillTargetStr = JobName::StoneGatherer;
		break;
	case CSkill::SkillTarget::GrassGatherer:
		skillTargetStr = JobName::GrassGatherer;
		break;
	case CSkill::SkillTarget::Builder:
		skillTargetStr = JobName::Builder;
		break;
	case CSkill::SkillTarget::Cook:
		skillTargetStr = JobName::Cook;
		break;
	case CSkill::SkillTarget::Smith:
		skillTargetStr = JobName::Smith;
		break;
	case CSkill::SkillTarget::None:
		skillTargetStr = u8"なし";
		break;
	default:
		skillTargetStr = "Unknown";
		break;
	}

	ImGui::Text("%s", skillTargetStr.c_str());
	ImGui::SameLine();

	// スキル効果の表示
	std::string skillEffectStr;
	switch (m_eSkillEffect)
	{
	case CSkill::SkillEffect::Multiplier_1_5:
		skillEffectStr = u8"1.5倍";
		break;
	case CSkill::SkillEffect::Multiplier_2_0:
		skillEffectStr = u8"2.0倍";
		break;
	case CSkill::SkillEffect::None:
		skillEffectStr = u8"なし";
		break;
	default:
		skillEffectStr = "Unknown";
		break;
	}
	ImGui::Text(": %s", skillEffectStr.c_str());
}
