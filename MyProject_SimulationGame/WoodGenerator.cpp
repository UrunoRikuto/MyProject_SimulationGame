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

#undef min

/*****************************************
	@brief　	| 生成処理
*//*****************************************/
void CWoodGenerator::Generate()
{
	// 木を配置可能な未使用セルを取得
	auto cells = CFieldManager::GetInstance()->GetFieldGrid()->GetFieldCells(CFieldCell::CellType::TREE, false);

	// 配置可能なセルがなければ終了
	if (cells.empty()) return;

	// 木の最大生成数
	int createNum = 0;
	for(auto cell : cells)
	{
		// 木オブジェクトを生成し、選出したセルに配置
		GetScene()->AddGameObject<CWood>(Tag::GameObject, u8"木")->SetCreatePos(cell);
		createNum++;
	}

	// デバッグログの追加
	CImguiSystem::GetInstance()->AddDebugLog(std::string(u8"木を%n生成しました", createNum), false);
}
