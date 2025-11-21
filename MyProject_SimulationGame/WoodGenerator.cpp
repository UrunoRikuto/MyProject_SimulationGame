/**************************************************//*
	@file	| WoodGenerator.cpp
	@brief	| 木材ジェネレータークラスのhファイル
	@note	| 木材を生成するジェネレーター
			| IGeneratorを継承
*//**************************************************/
#include "WoodGenerator.h"
#include "Wood.h"
#include "Main.h"
#include "ImguiSystem.h"

/*****************************************
	@brief　	| 生成処理
*//*****************************************/
void CWoodGenerator::Generate()
{
	CImguiSystem::GetInstance()->AddDebugLog("WoodGenerator: Generate [Wood]", false);

	GetScene()->AddGameObject<CWood>(Tag::GameObject, "Wood");

}
