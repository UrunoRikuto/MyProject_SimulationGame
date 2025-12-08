/**************************************************//*
	@file	| Skill.h
	@brief	| スキルクラスヘッダ
	@note	| 人間が生成時にもつスキル
*//**************************************************/
#pragma once
#include "Job_Strategy.h"

// @brief スキルクラス
class CSkill
{
public:
	// @brief スキル対象列挙型
	enum class SkillTarget
	{
		MaxStamina,		// 最大スタミナ
		MaxHealth,		// 最大体力
		Collector,		// 採取全般
		Crafter,		// 加工全般
		WoodGatherer,	// 木の採取職
		StoneGatherer,	// 石の採取職
		GrassGatherer,	// 草の採取職
		Builder,		// 建築職
		Cook,			// 料理職
		Smith,			// 鍛冶職

		MAX,
		None			// スキル対象なし
	};
	// @brief スキルの効果
	enum class SkillEffect
	{
		Multiplier_1_5,		// 1.5倍
		Multiplier_2_0,		// 2.0倍

		MAX,
		None				// 効果なし
	};

public:
	// @brief コンストラクタ
	CSkill();

	// @brief デストラクタ
	~CSkill();

	// @brief スキルの効果を適応(体力)
	// @param Out_StatusValue：ステータス値の参照渡し
	// @param In_Target：スキル対象
	void ApplySkillBuff(float& Out_StatusValue,SkillTarget In_Target);

	// @brief スキルの効果を適応(職業)
	// @param Out_WorkPower：労働力の参照渡し
	// @param In_Job：対象の職業ポインタ
	void ApplySkillBuff(float& Out_WorkPower, IJob_Strategy* In_Job);

	// @brief スキルのImGui表示処理
	void DrawSkillImGui();

private:
	SkillTarget m_eSkillTarget;	// スキル対象
	SkillEffect m_eSkillEffect;	// スキル効果
};

