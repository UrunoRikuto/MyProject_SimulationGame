/**************************************************//*
	@file	| StoneGenerator.cpp
	@brief	| 石ジェネレータークラスのcppファイル
	@note	| 石を生成するジェネレーター
			| IGeneratorを継承
*//**************************************************/
#include "StoneGenerator.h"
#include "Stone.h"
#include "Main.h"
#include "ImguiSystem.h"

/*****************************************
	@brief　	| 生成処理
*//*****************************************/
void CStoneGenerator::Generate()
{
	CImguiSystem::GetInstance()->AddDebugLog("StoneGenerator: Generate [Stone]", false);

	GetScene()->AddGameObject<CStone>(Tag::GameObject, "Stone");
}
