/**************************************************//*
	@file	| GameTimeManager.h
	@brief	| ゲーム内時間管理システムのhファイル
	@note	| ゲーム内時間の進行や管理を担当するクラスを定義しています。
*//**************************************************/
#pragma once
#include "Defines.h"

// @brief ゲーム内時間管理クラス
class CGameTimeManager
{
private:
	// @brief 1日の時間（秒）
	static constexpr float ONE_DAY_TIME = 120.0f;
private:
	// @brief コンストラクタ
	CGameTimeManager();

	// コピーコンストラクタ・代入演算子の禁止
	CGameTimeManager(const CGameTimeManager&) = delete;
	CGameTimeManager& operator=(const CGameTimeManager&) = delete;
public:

	// @brief デストラクタ
	~CGameTimeManager();

	// @brief シングルトンインスタンスを取得する関数
	static CGameTimeManager* GetInstance();

	// @brief ゲーム内時間を進行させる関数
	void UpdateGameTime();

	// @brief ゲーム内時間を取得する関数
	// @return ゲーム内時間
	const float GetGameTime() const;

	// @brief ゲーム内日数を取得する関数
	// @return ゲーム内日数
	const int GetGameDays() const;

	// @brief 一日の時間進行度を取得する関数
	// @return 一日の時間進行度（0.0f〜1.0f）
	const float GetDayProgress() const;

private:

	// @brief シングルトンインスタンス
	static CGameTimeManager* m_Instance;

	// @brief ゲーム内時間
	float m_fGameTime;
};

