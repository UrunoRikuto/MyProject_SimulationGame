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

	// 草の最大生成数
	int createNum = 0;
	for(auto cell : cells)
	{
		// 草オブジェクトを生成し、選出したセルに配置
		GetScene()->AddGameObject<CGrass>(Tag::GameObject, u8"草")->SetCreatePos(cell);
		createNum++;
	}

	// デバッグログの追加
	CImguiSystem::GetInstance()->AddDebugLog(std::string(u8"草を%n生成しました", createNum), false);
}
