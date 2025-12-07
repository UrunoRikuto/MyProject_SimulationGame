/**************************************************//*
	@file	| CivLevelManager.cpp
	@brief	| 文明レベル管理クラスのcppファイル
	@note	| 文明レベル管理クラスの処理を定義
			| 文明レベル = 人間の数
			| シングルトンパターンで作成
*//**************************************************/
#include "CivLevelManager.h"
#include <new>
#include "JobOperator.h"
#include "ImguiSystem.h"
#include "BuildManager.h"
#include "GeneratorManager.h"

/*****************************************//*
	@brief　	| コンストラクタ
*//*****************************************/
CCivLevelManager::CCivLevelManager()
	: m_nCivLevel(1)
	, m_nExperience(0.0f)
{
}

/*****************************************//*
	@brief　	| デストラクタ
*//*****************************************/
CCivLevelManager::~CCivLevelManager()
{
}

/*****************************************//*
	@brief　	| 経験値を加算
	@param		| fExp：加算する経験値
	@note 		| 必要経験値を超えている場合、レベルアップを行う
*//*****************************************/
void CCivLevelManager::AddExp(ExpType In_eType)
{
	// 経験値を加算
	// In_eTypeに対応する経験値を加算
	m_nExperience += ExpTypeToValueMap[In_eType];

	// レベルアップ判定
	const float expThreshold = CIV_LEVEL_UP_EXP_BASE * m_nCivLevel; // レベルアップに必要な経験値の閾値
	
	// レベルアップ可能な限りレベルアップを繰り返す
	while (m_nExperience >= expThreshold)
	{
		// レベルアップ処理
		m_nExperience -= expThreshold;

		// 文明レベルを1上げる
		m_nCivLevel++;

		// ジェネレーターマネージャーに通知してオブジェクトを生成させる
		CGeneratorManager::GetInstance()->NotifyObservers();

		// ビルドマネージャーに通知して建築依頼の更新を促す
		CBuildManager::GetInstance()->AddBuildRequest(CBuildManager::BuildType::HumanHouse);
	}
}

/*****************************************//*
	@brief　	| 解放している職業の名前リストを取得
	@return		| 解放している職業の名前リスト
*//*****************************************/
std::vector<std::string> CCivLevelManager::GetUnlockJobNames()
{
	std::vector<std::string> pUnlockJobNames;

	// レベルに応じて解放される職業を追加
	if (m_nCivLevel >=  1)pUnlockJobNames.push_back(JobName::Neet);				// 無職
	if (m_nCivLevel >=  1)pUnlockJobNames.push_back(JobName::WoodGatherer);		// 木材収集職業
	if (m_nCivLevel >=  1)pUnlockJobNames.push_back(JobName::StoneGatherer);	// 石収集職業
	if (m_nCivLevel >=  3)pUnlockJobNames.push_back(JobName::Builder);			// 建築職業
	if (m_nCivLevel >=  5)pUnlockJobNames.push_back(JobName::Cook);				// 料理職業
	if (m_nCivLevel >=  8)pUnlockJobNames.push_back(JobName::GrassGatherer);	// 草収集職業
	if (m_nCivLevel >= 10)pUnlockJobNames.push_back(JobName::Farmer);			// 農業職業
	if (m_nCivLevel >= 12)pUnlockJobNames.push_back(JobName::Smith);			// 鍛冶職業

	return pUnlockJobNames;
}
