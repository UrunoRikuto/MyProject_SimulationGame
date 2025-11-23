/**************************************************//*
	@file	| Builder_Job.cpp
	@brief	| 建築職業クラスのcppファイル
	@note	| 建築職業の処理を実装
			| CJob_Strategyを継承
*//**************************************************/
#include "Builder_Job.h"
#include "FieldManager.h"

/*****************************************//*
	@brief　	| コンストラクタ
*//*****************************************/
CBuilder_Job::CBuilder_Job()
	: CCrafter_Strategy()
	, m_eCurrentState(WorkState::Waiting)
{
	// 建築職業の労働力を設定
	m_Status.m_fWorkPower = 15.0f;
	// 建築職業のスタミナを設定
	m_Status.m_fStamina = 100.0f;
	m_Status.m_fMaxStamina = m_Status.m_fStamina;
}

/*****************************************//*
	@brief	| 仕事処理
*//*****************************************/
void CBuilder_Job::DoWork()
{
	switch (m_eCurrentState)
	{
	case WorkState::Waiting:
	{
		// 使用していない建築可能なフィールドセルを探す
		auto cells = CFieldManager::GetInstance()->GetFieldGrid()->GetFieldCells(CFieldCell::CellType::Build, false);

		// 見つからなかった場合は処理を抜ける
		if (cells.empty())return;

		// ランダムにセルを選択
		int randomIndex = rand() % cells.size();
		CFieldCell* targetCell = cells[randomIndex];
		// 建築対象のフィールドセルインデックスを設定
		m_n2BuildTargetIndex = targetCell->GetIndex();

		break;
	}
	case WorkState::Building:
	{
		// スタミナが0以下になったらスタミナを0に設定し、休憩状態に移行
		if (m_Status.m_fStamina <= 0.0f)
		{
			m_Status.m_fStamina = 0.0f;
			m_eCurrentState = WorkState::Resting;
			return;
		}

		// 待機状態に移行
		m_eCurrentState = WorkState::Waiting;

		break;
	}
	case WorkState::Resting:
	{
		// 本来は休憩所に移動して休憩する処理を実装するが、今回は簡略化の為その場で休憩する
		m_Status.m_fStamina += 2.0f; // スタミナを回復

		// 休憩が完了したら再び待機状態に戻る
		if (m_Status.m_fStamina >= m_Status.m_fMaxStamina)
		{
			m_Status.m_fStamina = m_Status.m_fMaxStamina;
			m_eCurrentState = WorkState::Waiting;
		}

		break;
	}
	}
}
