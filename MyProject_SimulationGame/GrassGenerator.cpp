/**************************************************//*
	@file	| GrassGenerator.cpp
	@brief	| 草ジェネレータークラスのcppファイル
	@note	| 草を生成するジェネレーター
			| IGeneratorを継承
*//**************************************************/
#include "GrassGenerator.h"
#include "Grass.h"
#include "Main.h"
#include "ImguiSystem.h"

#undef min

/*****************************************
	@brief　	| 生成処理
*//*****************************************/
void CGrassGenerator::Generate()
{
	// 草を配置可能な未使用セルを取得
	auto cells = CFieldManager::GetInstance()->GetFieldGrid()->GetFieldCells(CFieldCell::CellType::GRASS, false);

	// 配置可能なセルがなければ終了
	if (cells.empty()) return;

	// ランダムにセルを選出
	int randIndex = rand() % cells.size();

	// 草オブジェクトを生成し、選出したセルに配置
	GetScene()->AddGameObject<CGrass>(Tag::GameObject, u8"草")->SetCreatePos(cells[randIndex]);

	// デバッグログの追加
	CImguiSystem::GetInstance()->AddDebugLog(u8"草を生成しました", false);
}
