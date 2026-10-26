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
	CImguiSystem::GetInstance()->AddDebugLog(std::string(u8"石を") + std::to_string(createNum) + std::string(u8"個生成しました"), false);
}

/*****************************************//*
	@brief　	| 指定したセルへの生成処理
	@param　	| n2Cell：生成先のセルのインデックス
	@note		| 時間経過による再生成で使用する
				| 開拓などで石の生える土地ではなくなった場合や、既に何かが置かれている場合は生成しない
*//*****************************************/
void CStoneGenerator::GenerateAt(const DirectX::XMINT2& n2Cell)
{
	CFieldCell* pCell = GetGeneratableCell(n2Cell, CFieldCell::CellType::ROCK);
	if (pCell == nullptr) return;

	// 石オブジェクトを生成し、セルに配置
	GetScene()->AddGameObject<CStone>(Tag::GameObject, u8"石")->SetCreatePos(pCell);
}
