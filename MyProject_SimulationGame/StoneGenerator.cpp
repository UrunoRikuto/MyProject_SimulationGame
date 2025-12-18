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

#undef min

/*****************************************
	@brief　	| 生成処理
*//*****************************************/
void CStoneGenerator::Generate()
{
	// 岩を配置可能な未使用セルを取得
	auto cells = CFieldManager::GetInstance()->GetFieldGrid()->GetFieldCells(CFieldCell::CellType::ROCK, false);

	// 配置可能なセルがなければ終了
	if (cells.empty()) return;

	int createNum = 0;
	for(auto cell : cells)
	{
		// 石オブジェクトを生成し、選出したセルに配置
		GetScene()->AddGameObject<CStone>(Tag::GameObject, u8"石")->SetCreatePos(cell);
		createNum++;
	}

	// デバッグログの追加
	CImguiSystem::GetInstance()->AddDebugLog(std::string(u8"石を%n生成しました", createNum), false);
}
