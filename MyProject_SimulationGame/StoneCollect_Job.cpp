/**************************************************//*
	@file	| StoneCollect_Job.cpp
	@brief	| 石収集職業クラスのcppファイル
	@note	| 石収集職業の処理を実装
			| CCollect_Strategyを継承
*//**************************************************/
#include "StoneCollect_Job.h"
#include "ImguiSystem.h"
#include "Main.h"
#include "Oparation.h"
#include "StructMath.h"
#include "Stone.h"

/******************************************//*
	@brief　	| 仕事処理
	@note		| Stoneのオブジェクトタグが付いたオブジェクトを探す処理を実装
				| 見つかったら戦闘処理(AがBの体力を減らすシステム)を利用して石を収集する処理を実装
*//******************************************/
void CStoneCollect_Job::DoWork()
{
	switch (m_eCurrentState)
	{
	case CStoneCollect_Job::WorkState::SearchAndMove:
	{
		// 標的にしている石がなければ石オブジェクトを探す
		if (m_pTargetStone == nullptr)
		{
			m_pTargetStone = GetScene()->GetGameObject<CStone>(m_pOwner->GetPos());
		}
		else
		{
			// 石オブジェクトに向かって移動する処理を実装
			DirectX::XMFLOAT3 f3StonePos = m_pTargetStone->GetPos();
			// オーナーの位置を取得
			DirectX::XMFLOAT3 f3OwnerPos = m_pOwner->GetPos();
			// 石オブジェクトとオーナーの位置の距離を計算
			float fDistance = StructMath::Distance(f3OwnerPos, f3StonePos);
			// 一定距離以内に到達したら収集状態に移行
			if (fDistance < 1.0f)
			{
				m_eCurrentState = WorkState::Collecting;
				return;
			}
			DirectX::XMFLOAT3 f3Diff = f3StonePos - f3OwnerPos;
			// オーナーから石オブジェクトへのベクトルを計算
			DirectX::XMVECTOR f3Direction = DirectX::XMLoadFloat3(&f3Diff);
			f3Direction = DirectX::XMVector3Normalize(f3Direction);
			// オーナーの位置を石オブジェクトに向かって少しずつ移動させる
			DirectX::XMFLOAT3 f3Move;
			DirectX::XMStoreFloat3(&f3Move, f3Direction);
			float fSpeed = 0.1f; // 移動速度
			f3OwnerPos += f3Move * fSpeed;
			// オーナーの位置を更新
			m_pOwner->SetPos(f3OwnerPos);
		}
	}
	break;
	case CStoneCollect_Job::WorkState::Collecting:
	{
		// 石を収集する
		if (m_pTargetStone != nullptr)
		{
			// 戦闘処理を利用して石を収集する処理を実装
			// ここでは単純に石オブジェクトを破棄することで収集を表現
			m_pTargetStone->Destroy();
			m_pTargetStone = nullptr;
		}

		// 収集が完了したら休憩状態に移行
		m_eCurrentState = WorkState::Resting;
	}
	break;
	case CStoneCollect_Job::WorkState::Resting:
	{
		// 休憩が完了したら再び石を探して移動する状態に戻る
		m_eCurrentState = WorkState::SearchAndMove;
	}
	break;
	}
}