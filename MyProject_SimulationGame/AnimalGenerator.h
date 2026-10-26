/**************************************************//*
	@file	| AnimalGenerator.h
	@brief	| 動物ジェネレータークラスのhファイル
	@note	| 動物を生成するジェネレーター
			| IGeneratorを継承
*//**************************************************/
#pragma once
#include "Generator.h"
#include <vector>

// @brief 動物ジェネレータークラス
class CAnimalGenerator : public IGenerator
{
public:
	// @brief 更新処理
	// @note 初期生成した数を目標数として、足りない動物を一定間隔で1頭ずつ補充する
	void Update() override;

private:
	// @brief 生成処理(初期生成)
	void Generate() override;

	// @brief 鹿を1頭補充する処理
	void RespawnDeer();

	// @brief 狼を1頭補充する処理
	void RespawnWolf();

	// @brief 動物を補充するセルを1つ選ぶ処理
	// @param eTerritory：補充する動物の縄張りタイプ
	// @return 補充先のセル(人間から離れた、縄張り内の未使用の空き地)、見つからなかった場合はnullptr
	// @note 全セルを調べると補充のたびに処理が止まるため、ランダムなセルを一定回数だけ調べて、最初に条件に合ったものを選ぶ
	CFieldCell* PickSpawnCell(CFieldCell::TerritoryType eTerritory) const;

private:
	// @brief 鹿の目標数
	int m_nTargetDeer = 0;

	// @brief 狼の目標数
	int m_nTargetWolf = 0;

	// @brief 補充の確認タイマー(秒)
	float m_fRespawnTimer = 0.0f;
};
