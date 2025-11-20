/**************************************************//*
	@file	| WoodCollect_Job.cpp
	@brief	| 木材収集職業クラスのcppファイル
	@note	| 木材収集職業の処理を実装
			| CCollect_Strategyを継承
*//**************************************************/
#include "WoodCollect_Job.h"
#include "ImguiSystem.h"
#include "Main.h"
#include "Oparation.h"
#include "StructMath.h"

/******************************************//*
	@brief　	| 仕事処理
	@note		| Woodのオブジェクトタグが付いたオブジェクトを探す処理を実装
				| 見つかったら戦闘処理(AがBの体力を減らすシステム)を利用して木材を収集する処理を実装
*//******************************************/
void CWoodCollect_Job::DoWork()
{
	switch (m_eCurrentState)
	{
	case CWoodCollect_Job::WorkState::SearchAndMove:
	{
		// 標的にしている木がなければ木オブジェクトを探す
		if (m_pTargetWood == nullptr)
		{
			m_pTargetWood = GetScene()->GetGameObject<CWood>(m_pOwner->GetPos());
		}
		else
		{
			// 木オブジェクトに向かって移動する処理を実装
			DirectX::XMFLOAT3 f3WoodPos = m_pTargetWood->GetPos();
			// オーナーの位置を取得
			DirectX::XMFLOAT3 f3OwnerPos = m_pOwner->GetPos();

			// 木オブジェクトとオーナーの位置の距離を計算
			float fDistance = StructMath::Distance(f3OwnerPos, f3WoodPos);
			// 一定距離以内に到達したら収集状態に移行
			if (fDistance < 1.0f)
			{
				m_eCurrentState = WorkState::Collecting;
				return;
			}

			DirectX::XMFLOAT3 f3Diff = f3WoodPos - f3OwnerPos;

			// オーナーから木オブジェクトへのベクトルを計算
			DirectX::XMVECTOR f3Direction = DirectX::XMLoadFloat3(&f3Diff);
			f3Direction = DirectX::XMVector3Normalize(f3Direction);

			// オーナーの位置を木オブジェクトに向かって少しずつ移動させる
			DirectX::XMFLOAT3 f3Move;
			DirectX::XMStoreFloat3(&f3Move, f3Direction);
			float fSpeed = 0.1f; // 移動速度
			f3OwnerPos += f3Move * fSpeed;

			// オーナーの位置を更新
			m_pOwner->SetPos(f3OwnerPos);
		}
	}
	break;
	case CWoodCollect_Job::WorkState::Collecting:
	{
		GetScene()->GetGameObject(m_pTargetWood->GetID())->Destroy();
		m_pTargetWood = nullptr;
		m_eCurrentState = WorkState::Resting;
	}
	break;
	case CWoodCollect_Job::WorkState::Resting:
	{
		m_eCurrentState = WorkState::SearchAndMove;
	}
	break;
	}

	if (m_pTargetWood != nullptr)
	{
		ObjectID objID = m_pTargetWood->GetID();
		std::string objName = objID.m_sName + std::to_string(objID.m_nSameCount);
		// ImGuiデバッグログに出力
		CImguiSystem::GetInstance()->AddDebugLog(("WoodCollect_Job: 【Target：%s】", objName));
	}

}