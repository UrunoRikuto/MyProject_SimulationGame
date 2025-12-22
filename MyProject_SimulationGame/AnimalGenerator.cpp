/**************************************************//*
	@file	| AnimalGenerator.cpp
	@brief	| 動物ジェネレータークラスのcppファイル
	@note	| 動物を生成するジェネレーター
			| IGeneratorを継承
*//**************************************************/
#include "AnimalGenerator.h"
#include "FieldManager.h"
#include "Main.h"

#include "Wolf_Animal.h"
#include "Deer_Animal.h"

/*****************************************//*
	@brief　	| 生成処理
*//*****************************************/
void CAnimalGenerator::Generate()
{
	// フィールドセルの2次元配列を取得
	std::vector<std::vector<CFieldCell*>> pFieldCells = CFieldManager::GetInstance()->GetFieldGrid()->GetFieldCells();

	// 生成した動物のリスト
	std::vector<CWolf_Animal*> wolfList;
	std::vector<CDeer_Animal*> deerList;

	// フィールドセルを走査して動物を生成
	for (std::vector<CFieldCell*> cells : pFieldCells)
	{
		for (CFieldCell* cell : cells)
		{
			// オオカミ
			if (cell->GetTerritoryType() == CFieldCell::TerritoryType::Wolf && !cell->IsUse())
			{
				CWolf_Animal* pWolf = GetScene()->AddGameObject<CWolf_Animal>(Tag::GameObject, u8"狼");
				pWolf->SetPos(cell->GetPos());
				pWolf->RegisterToCell(cell->GetIndex());
				wolfList.push_back(pWolf);
				cell->SetUse(true);
			}
			// 鹿
			else if (cell->GetTerritoryType() == CFieldCell::TerritoryType::Deer && !cell->IsUse())
			{
				CDeer_Animal* pDeer = GetScene()->AddGameObject<CDeer_Animal>(Tag::GameObject, u8"鹿");
				pDeer->SetPos(cell->GetPos());
				pDeer->RegisterToCell(cell->GetIndex());
				deerList.push_back(pDeer);
				cell->SetUse(true);
			}
		}
	}
	// 群れの形成
	for (CWolf_Animal* wolf : wolfList)
	{
		wolf->RegisterToFlock(wolfList);
	}
	for (CDeer_Animal* deer : deerList)
	{
		deer->RegisterToFlock(deerList);
	}
}
