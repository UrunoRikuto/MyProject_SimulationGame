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
*//****************************************/
const float CGameTimeManager::GetGameTime() const
{
	return m_fGameTime;
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
