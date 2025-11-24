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

	// 生成数を決定（最大5つまで）
	int nCreateCount = std::min(5, static_cast<int>(cells.size()));
	for (int i = 0; i < nCreateCount; ++i)
	{
		// ランダムにセルを選出
		int randIndex = rand() % cells.size();
		// 石オブジェクトを生成し、選出したセルに配置
		GetScene()->AddGameObject<CStone>(Tag::GameObject, "Stone")->SetCreatePos(cells[randIndex]);
		// デバッグログの追加
		CImguiSystem::GetInstance()->AddDebugLog("StoneGenerator: Generate [Stone]", false);
	}

	// デバッグログの追加
	CImguiSystem::GetInstance()->AddDebugLog("StoneGenerator: Generate [Stone]", false);
}
