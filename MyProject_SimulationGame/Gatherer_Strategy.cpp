/**************************************************//*
	@file	| CGatherer_Strategy.cpp
	@brief	| 採集職業クラスのcppファイル
	@note	| 探して採取する職の処理を実装
			| CCollect_Strategyを継承
*//**************************************************/
#include "Gatherer_Strategy.h"
#include "Main.h"
#include "Oparation.h"
#include "Structmath.h"
#include "ImguiSystem.h"
#include "CollectTarget.h"


/****************************************//*
	@brief　	|　仕事処理
*//****************************************/
void CGatherer_Strategy::DoWork()
{
	switch (m_eCurrentState)
	{
	case WorkState::SearchAndMove:
	{
		// 標的にしているオブジェクトがない場合
		if (m_pTarget == nullptr)
		{
			std::vector<ObjectID> vNotTargetIDs;

			while (1)
			{
				// オブジェクトを探す処理を実装
				m_pTarget = SearchTarget(vNotTargetIDs);

				// 木オブジェクトが見つからなかった場合はループを抜ける
				if (m_pTarget == nullptr)return;

				// 標的オブジェクトに設定されているターゲティングIDを取得
				ObjectID targetingID = m_pTarget->GetTargetingID();

				// もしターゲティングIDが設定されているか確認
				if (targetingID.m_nSameCount != -1)
				{
					// ターゲティングIDが設定されている場合は、そのIDをリストに追加
					vNotTargetIDs.push_back(m_pTarget->GetID());
				}
				// ターゲティングIDが設定されていない場合はループを抜ける
				else break;
			}
			// 自身のIDを標的オブジェクトに設定
			m_pTarget->SetTargetingID(m_pOwner->GetID());
		}
		else
		{
			// オブジェクトに向かって移動する処理を実装
			DirectX::XMFLOAT3 f3StonePos = m_pTarget->GetPos();

			// オーナーの位置を取得
			DirectX::XMFLOAT3 f3OwnerPos = m_pOwner->GetPos();

			// オブジェクトとオーナーの位置の距離を計算
			float fDistance = StructMath::Distance(f3OwnerPos, f3StonePos);

			// 一定距離以内に到達したら収集状態に移行
			if (fDistance < 1.0f)
			{
				m_eCurrentState = WorkState::Gathering;
				return;
			}

			DirectX::XMFLOAT3 f3Diff = f3StonePos - f3OwnerPos;
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
		}
	}
	break;
	case WorkState::Gathering:
	{
		// 素材を収集する
		if (m_pTarget != nullptr)
		{
			// 戦闘処理を利用して石を収集する処理を実装
			// ここでは単純にオブジェクトを破棄することで収集を表現
			m_pTarget->Destroy();
			m_pTarget = nullptr;
		}

		// 収集が完了したら休憩状態に移行
		m_eCurrentState = WorkState::Resting;
	}
	break;
	case WorkState::Resting:
	{
		// 休憩が完了したら再び石を探して移動する状態に戻る
		m_eCurrentState = WorkState::SearchAndMove;
	}
	break;
	}

	if (m_pTarget)
	{
		// デバックログで標的の名前を表示
		std::string sTargetName = m_pTarget->GetID().m_sName;
		sTargetName = "TargetObject: " + sTargetName + std::to_string(m_pTarget->GetID().m_nSameCount);
		CImguiSystem::GetInstance()->AddDebugLog(sTargetName, true);
	}
}
