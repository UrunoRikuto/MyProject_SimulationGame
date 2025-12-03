/**************************************************//*
	@file	| GameTimeManager.cpp
	@brief	| ゲーム内時間管理システムのcppファイル
	@note	| ゲーム内時間の進行や管理を担当するクラスを定義しています。
*//**************************************************/
#include "GameTimeManager.h"
#include <new>

// 静的メンバ変数の定義
CGameTimeManager* CGameTimeManager::m_Instance = nullptr;

/****************************************//*
	@brief　	| コンストラクタ
*//****************************************/
CGameTimeManager::CGameTimeManager()
	: m_fGameTime(0.0f)
{
}

/****************************************//*
	@brief　	| デストラクタ
*//****************************************/
CGameTimeManager::~CGameTimeManager()
{
}

/****************************************//*
	@brief　	| シングルトンインスタンスを取得する関数
	@return		| シングルトンインスタンス
*//****************************************/
CGameTimeManager* CGameTimeManager::GetInstance()
{
	if (m_Instance == nullptr)
	{
		m_Instance = new(std::nothrow) CGameTimeManager();
	}
	return m_Instance;
}

/****************************************//*
	@brief　	| シングルトンインスタンスを解放する関数
*//****************************************/
void CGameTimeManager::ReleaseInstance()
{
	if (m_Instance != nullptr)
	{
		delete m_Instance;
		m_Instance = nullptr;
	}
}

/****************************************//*
	@brief　	| ゲーム内時間を進行させる関数
*//****************************************/
void CGameTimeManager::UpdateGameTime()
{
	// ゲーム内時間を1秒進行させる
	m_fGameTime += 1.0f / fFPS;
}

/****************************************//*
	@brief　	| ゲーム内時間を取得する関数
	@return		| ゲーム内時間
	@note		| 一日の時間は0.0f〜ONE_DAY_TIMEの範囲でループする
*//****************************************/
const float CGameTimeManager::GetGameTime() const
{
	return fmodf(m_fGameTime, ONE_DAY_TIME);
}

/****************************************//*
	@brief　	| ゲーム内日数を取得する関数
	@return		| ゲーム内日数
*//****************************************/
const int CGameTimeManager::GetGameDays() const
{
	return static_cast<int>(m_fGameTime / ONE_DAY_TIME);
}

/****************************************//*
	@brief　	| 一日の時間進行度を取得する関数
	@return		| 一日の時間進行度（0.0f〜1.0f）
*//****************************************/
const float CGameTimeManager::GetDayProgress() const
{
	return fmodf(m_fGameTime, ONE_DAY_TIME) / ONE_DAY_TIME;
}

/****************************************//*
	@brief　	| 現在の時間帯を取得する関数
	@return		| 現在の時間帯
*//****************************************/
const CGameTimeManager::DAY_TIME CGameTimeManager::GetCurrentDayTime() const
{
	float dayProgress = GetDayProgress();
	if (dayProgress < 0.25f)
	{
		return DAY_TIME::MORNING; // 朝
	}
	else if (dayProgress < 0.5f)
	{
		return DAY_TIME::NOON; // 昼
	}
	else if (dayProgress < 0.75f)
	{
		return DAY_TIME::EVENING; // 夕方
	}
	else
	{
		return DAY_TIME::NIGHT; // 夜
	}
}

/****************************************//*
	@brief　	| 現在の時間帯の文字列を取得する関数
	@return		| 現在の時間帯の文字列
*//****************************************/
const std::string CGameTimeManager::GetCurrentDayTimeString() const
{
	DAY_TIME currentDayTime = GetCurrentDayTime();
	switch (currentDayTime)
	{
	case DAY_TIME::MORNING:
		return "Morning";
	case DAY_TIME::NOON:
		return "Noon";
	case DAY_TIME::EVENING:
		return "Evening";
	case DAY_TIME::NIGHT:
		return "Night";
	default:
		return "Unknown";
	}
}

/****************************************//*
	@brief　	| 時間帯別の進行度を取得する関数
	@return		| 時間帯別の進行度（0.0f〜1.0f）
*//****************************************/
const float CGameTimeManager::GetTimeOfDayProgress() const
{
	float dayProgress = GetDayProgress();
	CGameTimeManager::DAY_TIME currentDayTime = GetCurrentDayTime();
	switch (currentDayTime)
	{
	case DAY_TIME::MORNING:
		return dayProgress / 0.25f; // 0.0f〜0.25f
	case DAY_TIME::NOON:
		return (dayProgress - 0.25f) / 0.25f; // 0.25f〜0.5f
	case DAY_TIME::EVENING:
		return (dayProgress - 0.5f) / 0.25f; // 0.5f〜0.75f
	case DAY_TIME::NIGHT:
		return (dayProgress - 0.75f) / 0.25f; // 0.75f〜1.0f
	default:
		return 0.0f;
	}
}
