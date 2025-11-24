/**************************************************//*
	@file	| Builder_Job.cpp
	@brief	| 建築職業クラスのcppファイル
	@note	| 建築職業の処理を実装
			| CJob_Strategyを継承
*//**************************************************/
#include "Builder_Job.h"
#include "FieldManager.h"
#include "Human.h"
#include "StructMath.h"
#include "Oparation.h"
#include "StorageHouse.h"
#include "RefreshFacility.h"
#include "Main.h"

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

		// 建築対象探索状態に移行
		m_eCurrentState = WorkState::SearchingTarget;

		break;
	}
	case WorkState::SearchingTarget:
	{
		// 目的のフィールドセルの位置を取得
		DirectX::XMFLOAT3 targetPosition = CFieldManager::GetInstance()->GetFieldGrid()->GetFieldCells()[m_n2BuildTargetIndex.x][m_n2BuildTargetIndex.y]->GetPos();

		// 所属しているオブジェクトを目的地に移動させる
		DirectX::XMFLOAT3 ownerPos = m_pOwner->GetPos();

		// オブジェクトとオーナーの位置の距離を計算
		float fDistance = StructMath::Distance(ownerPos, targetPosition);

		// 一定距離以内に到達したら収集状態に移行
		if (fDistance < 1.0f)
		{
			m_eCurrentState = WorkState::Building;
			return;
		}

		DirectX::XMFLOAT3 f3Diff = targetPosition - ownerPos;
		// オーナーからオブジェクトへのベクトルを計算
		DirectX::XMVECTOR f3Direction = DirectX::XMLoadFloat3(&f3Diff);
		f3Direction = DirectX::XMVector3Normalize(f3Direction);

		// オーナーの位置をオブジェクトに向かって少しずつ移動させる
		DirectX::XMFLOAT3 f3Move;
		DirectX::XMStoreFloat3(&f3Move, f3Direction);

		float fSpeed = 0.1f; // 移動速度
		ownerPos += f3Move * fSpeed;

		// オーナーの位置を更新
		m_pOwner->SetPos(ownerPos);

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
		// 休憩施設を取得
		CGameObject* pRefreshFacility = GetScene()->GetGameObject("RefreshFacility");
		// 休憩施設が見つからなかった場合は処理を抜ける
		if (pRefreshFacility == nullptr)return;

		// 休憩所の位置を取得
		DirectX::XMFLOAT3 f3RefreshFacilityPos = pRefreshFacility->GetPos();

		// オーナーの位置を取得
		DirectX::XMFLOAT3 f3OwnerPos = m_pOwner->GetPos();

		// オブジェクトとオーナーの位置の距離を計算
		float fDistance = StructMath::Distance(f3OwnerPos, f3RefreshFacilityPos);

		// 一定距離以内に達したら休憩する
		if (fDistance >= 1.0f)
		{
			DirectX::XMFLOAT3 f3Diff = f3RefreshFacilityPos - f3OwnerPos;
			// オーナーからオブジェクトへのベクトルを計算
			DirectX::XMVECTOR f3Direction = DirectX::XMLoadFloat3(&f3Diff);
			f3Direction = DirectX::XMVector3Normalize(f3Direction);
			// オーナーの位置をオブジェクトに向かって少しずつ移動させる
			DirectX::XMFLOAT3 f3Move;
			DirectX::XMStoreFloat3(&f3Move, f3Direction);
			float fSpeed = 0.1f; // 移動速度
			f3OwnerPos += f3Move * fSpeed;
			// オーナーの位置を更新
			m_pOwner->SetPos(f3OwnerPos);
			return;
		}

		// スタミナを回復
		m_Status.m_fStamina += CRefreshFacility::REFRESH_STAMINA_AMOUNT;

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
