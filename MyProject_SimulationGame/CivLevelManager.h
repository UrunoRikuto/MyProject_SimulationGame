/**************************************************//*
	@file	| CivLevelManager.h
	@brief	| 文明レベル管理クラスのhファイル
	@note	| 文明レベル管理クラスの処理を定義
			| シングルトンパターンで作成
*//**************************************************/
#pragma once
#include "Singleton.h"
#include <string>
#include <vector>
#include <unordered_map>

// @brief 文明レベル管理クラス
class CCivLevelManager : public ISingleton<CCivLevelManager>
{
private:
	// @brief 文明レベルアップに必要な経験値の基準
	// @note レベルが上がるごとに必要経験値が増加する場合、この値を基準に計算する
	static constexpr float CIV_LEVEL_UP_EXP_BASE = 100.0f;

public:

	// @brief 経験値タイプ列挙型
	enum class ExpType
	{
		// 採取時
		Gathering,
		// 建築時
		Building,
		// 生産時
		Production,
		// 農作時
		Farming,

		MAX
	};

	// @brief 経験値タイプごとの加算値マップ
	std::unordered_map<ExpType, float> ExpTypeToValueMap =
	{
		{ ExpType::Gathering,	20.0f },	// 採取時の加算経験値
		{ ExpType::Building,	50.0f },	// 建築時の加算経験値
		{ ExpType::Production,	10.0f },	// 生産時の加算経験値
		{ ExpType::Farming,		10.0f },	// 農作時の加算経験値
	};

private:
	// @brief コンストラクタ
	CCivLevelManager();

	friend class ISingleton<CCivLevelManager>;

public:

	// @brief デストラクタ
	~CCivLevelManager();

	// @brief 文明レベル取得
	int GetCivLevel() const { return m_nCivLevel; }

	// @brief 文明レベル設定
	void SetCivLevel(int In_nLevel) { m_nCivLevel = In_nLevel; }

	// @brief 経験値を加算
	// @param fExp：加算する経験値
	void AddExp(ExpType In_eType);

	// @brief 経験値取得
	float GetExp() const { return m_nExperience; }

	// @brief 文明レベルアップに必要な経験値を取得
	float GetExpThreshold() const { return CIV_LEVEL_UP_EXP_BASE * m_nCivLevel; }

	// @brief 解放している職業の名前リストを取得
	std::vector<std::string> GetUnlockJobNames();

private:

	// @brief 文明レベル
	int m_nCivLevel;

	// @brief 経験値
	float m_nExperience;

};

