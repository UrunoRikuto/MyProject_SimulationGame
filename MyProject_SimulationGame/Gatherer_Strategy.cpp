/**************************************************//*
	@file	| CGatherer_Strategy.cpp
	@brief	| 採集職業クラスのcppファイル
	@note	| 探して採取する職の処理を実装
			| CCollect_Strategyを継承
*//**************************************************/
#include "Gatherer_Strategy.h"
#include "Main.h"
#include "Oparation.h"
#include "StructMath.h"
#include "ImguiSystem.h"
#include "CollectTarget.h"
#include "Human.h"
#include "StorageHouse.h"
#include "RefreshFacility.h"

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

			// 標的オブジェクトを探すループ
			// ターゲティングIDが設定されているオブジェクトは避けて探す
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
			// 標的オブジェクトの耐久値を労働力分減少させる
			m_pTarget->DecreaseHp(m_Status.m_fWorkPower);
			m_Status.m_fStamina -= 10.0f; // スタミナを消費

			// 標的オブジェクトが破壊された場合はオブジェクトを破棄し、標的ポインタをnullptrに設定
			if (m_pTarget->IsDead())
			{
				// ドロップアイテムを取得してインベントリに追加する処理を実装
				auto droppedItems = m_pTarget->DropItem();
				for (CItem* item : droppedItems)
				{
					m_pOwner->HoldItem(item);
				}

				m_pTarget->Destroy();
				m_pTarget = nullptr;
			}

			// アイテムが所持できる上限に達した場合は運搬状態に移行
			if (m_pOwner->GetItemCount() >= MaxHoldItem)
			{
				m_eCurrentState = WorkState::Transporting;
				return;
			}

			// スタミナが0以下になったらスタミナを0に設定し、休憩状態に移行
			if (m_Status.m_fStamina <= 0.0f)
			{
				m_Status.m_fStamina = 0.0f;
				m_eCurrentState = WorkState::Resting;
				return;
			}
		}

		// 収集が完了したら再び標的オブジェクトを探して移動する状態に戻る
		m_eCurrentState = WorkState::SearchAndMove;
	}
	break;
	case WorkState::Transporting:
	{
		// 一番近い貯蔵庫を探す
		CGameObject* pStorageHouse = GetScene()->GetGameObject("StorageHouse");

		// 貯蔵庫が見つからなかった場合は処理を抜ける
		if (pStorageHouse == nullptr)
		{
			// スタミナが0以下になったらスタミナを0に設定し、休憩状態に移行
			if (m_Status.m_fStamina <= 0.0f)
			{
				m_Status.m_fStamina = 0.0f;
				m_eCurrentState = WorkState::Resting;
				return;
			}

			// 再び標的オブジェクトを探して移動する状態に戻る
			m_eCurrentState = WorkState::SearchAndMove;
			return;
		}

		// 貯蔵庫に向かって移動する
		DirectX::XMFLOAT3 f3StorageHousePos = pStorageHouse->GetPos();

		// オーナーの位置を取得
		DirectX::XMFLOAT3 f3OwnerPos = m_pOwner->GetPos();

		// オブジェクトとオーナーの位置の距離を計算
		float fDistance = StructMath::Distance(f3OwnerPos, f3StorageHousePos);

		// 一定距離以内に到達したら収集状態に移行
		if (fDistance < 1.0f)
		{
			m_eCurrentState = WorkState::Storing;
			return;
		}

		// オーナーからオブジェクトへのベクトルを計算
		DirectX::XMFLOAT3 f3Diff = f3StorageHousePos - f3OwnerPos;

		// 正規化
		DirectX::XMFLOAT3 f3Move = StructMath::Normalize(f3Diff);

		float fSpeed = 0.1f; // 移動速度
		f3OwnerPos += f3Move * fSpeed;

		// オーナーの位置を更新
		m_pOwner->SetPos(f3OwnerPos);
	}
	break;
	case WorkState::Storing:
	{
		// 所持している素材アイテムを貯蔵庫に運搬する
		if (m_pOwner->HasItem())
		{

			// 一番近い貯蔵庫を探す
			CGameObject* pStorageHouse = GetScene()->GetGameObject("StorageHouse");

			// 貯蔵庫にアイテムを収納する
			dynamic_cast<CStorageHouse*>(pStorageHouse)->StoreItem(m_pOwner->TakeOutItem());
		}
		else
		{
			// スタミナが0以下になったらスタミナを0に設定し、休憩状態に移行
			if (m_Status.m_fStamina <= 0.0f)
			{
				m_Status.m_fStamina = 0.0f;
				m_eCurrentState = WorkState::Resting;
				return;
			}

			// 運搬が完了したら再び標的オブジェクトを探して移動する状態に戻る
			m_eCurrentState = WorkState::SearchAndMove;
		}
	}
	break;
	case WorkState::Resting:
	{
		// 休憩が完了したら再び標的オブジェクトを探して移動する状態に戻る
		if (RestAction())
		{
			m_Status.m_fStamina = m_Status.m_fMaxStamina;
			m_eCurrentState = WorkState::SearchAndMove;
		}
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
