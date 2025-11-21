/**************************************************//*
	@file	| HumanGenerator.cpp
	@brief	| 人間ジェネレータークラスのcppファイル
	@note	| 人間を生成するジェネレーター
			| IGeneratorを継承
*//**************************************************/
#include "HumanGenerator.h"
#include "Human.h"
#include "Main.h"
#include "ImguiSystem.h"

/*****************************************
	@brief　	| 生成処理
*//*****************************************/
void CHumanGenerator::Generate()
{
	CImguiSystem::GetInstance()->AddDebugLog("WoodGenerator: Generate [Human]");

	GetScene()->AddGameObject<CHuman>(Tag::GameObject, "Human");
}
