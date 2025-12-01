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
#include "CivLevelManager.h"

/****************************************//*
	@brief　	|　仕事処理
*//****************************************/
void CGatherer_Strategy::DoWork()
{
	switch (m_eCurrentState)
	{
		// 探して移動
	case WorkState::SearchAndMove:	
		SearchAndMoveAction();		
		break;

		// 採取中
	case WorkState::Gathering:		
		GatheringAction();			
		break;

		// 運搬中
	case WorkState::Transporting:	
		TransportingAction();		
		break;

		// 貯蔵中
	case WorkState::Storing:		
		StoringAction();			
		break;

		// 休憩中
	case WorkState::Resting:		
		RestingAction();			
		break;

	}
}

/****************************************//*
	@brief　	|　職業切り替え時の処理
*//****************************************/
void CGatherer_Strategy::OnChangeJob()
{
	// 標的オブジェクトのターゲティングIDを解除
	if (m_pTarget != nullptr)
	{
		m_pTarget->SetTargetingID(ObjectID{ "", -1 });
		m_pTarget = nullptr;
	}

	// 仕事状態を初期化
	m_eCurrentState = WorkState::SearchAndMove;
	m_ePrevState = WorkState::SearchAndMove;
}

/****************************************//*
	@brief　	|　インスペクター表示処理
	@param　	|	isEnd：true:ImGuiのEnd()を呼ぶ false:呼ばない
	@return		|	表示した項目数
*//****************************************/
int CGatherer_Strategy::Inspecter(bool isEnd)
{
	int nItemCount = 0;

	// インスペクターの子ウィンドウを開始
	ImGui::BeginChild("Gatherer Job Inspecter");

	// 職業名の表示
	ImGui::Text(std::string("JobName:"+ GetJobName()).c_str());

	// ステータスの表示
	ImGui::Text(std::string("WorkPower:" + std::to_string(m_Status.m_fWorkPower)).c_str());
	ImGui::Text(std::string("Stamina:" + std::to_string(m_Status.m_fStamina) + "/" + std::to_string(m_Status.m_fMaxStamina)).c_str());

	// 現在の仕事状態の表示
	std::string sWorkState;
	switch (m_eCurrentState)
	{
	case WorkState::SearchAndMove:
		sWorkState = "SearchAndMove";
		break;
	case WorkState::Gathering:
		sWorkState = "Gathering";
		break;
	case WorkState::Transporting:
		sWorkState = "Transporting";
		break;
	case WorkState::Storing:
		sWorkState = "Storing";
		break;
	case WorkState::Resting:
		sWorkState = "Resting";
		break;
	}
	ImGui::Text(std::string("CurrentWorkState:" + sWorkState).c_str());

	ImGui::EndChild();
	nItemCount++;


	if (isEnd)
	{
		ImGui::EndChild();
		ImGui::End();
		nItemCount++;
	}
	return nItemCount;
}

/****************************************//*
	@brief　	|　探索＆移動中の処理
*//****************************************/
void CGatherer_Strategy::SearchAndMoveAction()
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
			m_ePrevState = m_eCurrentState;
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

		f3OwnerPos += f3Move * Human_Move_Speed;

		// オーナーの位置を更新
		m_pOwner->SetPos(f3OwnerPos);
	}
}

/****************************************//*
	@brief　	|　採取中の処理
*//****************************************/
void CGatherer_Strategy::GatheringAction()
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

			//　文明経験値を加算
			CCivLevelManager::GetInstance()->AddExp(CCivLevelManager::ExpType::Gathering);

			m_pTarget->Destroy();
			m_pTarget = nullptr;
		}

		// アイテムが所持できる上限に達した場合は運搬状態に移行
		if (m_pOwner->GetItemCount() >= MaxHoldItem)
		{
			m_ePrevState = m_eCurrentState;
			m_eCurrentState = WorkState::Transporting;
			return;
		}

		// スタミナが0以下になったらスタミナを0に設定し、休憩状態に移行
		if (m_Status.m_fStamina <= 0.0f)
		{
			m_Status.m_fStamina = 0.0f;
			m_ePrevState = m_eCurrentState;
			m_eCurrentState = WorkState::Resting;
			return;
		}
	}

	// 収集が完了したら再び標的オブジェクトを探して移動する状態に戻る
	m_eCurrentState = WorkState::SearchAndMove;
}

/****************************************//*
	@brief　	|　運搬中の処理
*//****************************************/
void CGatherer_Strategy::TransportingAction()
{
	// 一番近い貯蔵庫を探す
	CStorageHouse* pStorageHouse = GetScene()->GetGameObject<CStorageHouse>();

	// 貯蔵庫が見つからなかった場合は処理を抜ける
	if (pStorageHouse == nullptr)
	{
		// スタミナが0以下になったらスタミナを0に設定し、休憩状態に移行
		if (m_Status.m_fStamina <= 0.0f)
		{
			m_Status.m_fStamina = 0.0f;
			m_ePrevState = m_eCurrentState;
			m_eCurrentState = WorkState::Resting;
			return;
		}

		// 再び標的オブジェクトを探して移動する状態に戻る
		m_ePrevState = m_eCurrentState;
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
		m_ePrevState = m_eCurrentState;
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

/****************************************//*
	@brief　	|　貯蔵中の処理
*//****************************************/
void CGatherer_Strategy::StoringAction()
{
	// 所持している素材アイテムを貯蔵庫に運搬する
	if (m_pOwner->HasItem())
	{

		// 一番近い貯蔵庫を探す
		CStorageHouse* pStorageHouse = GetScene()->GetGameObject<CStorageHouse>();

		// 貯蔵庫に向かって移動する
		DirectX::XMFLOAT3 f3StorageHousePos = pStorageHouse->GetPos();

		// オーナーの位置を取得
		DirectX::XMFLOAT3 f3OwnerPos = m_pOwner->GetPos();

		// オブジェクトとオーナーの位置の距離を計算
		float fDistance = StructMath::Distance(f3OwnerPos, f3StorageHousePos);

		// 一定距離以内に到達したら素材アイテムを貯蔵庫に収納する
		if (fDistance < 1.0f)
		{
			// 貯蔵庫にアイテムを収納する
			dynamic_cast<CStorageHouse*>(pStorageHouse)->StoreItem(m_pOwner->TakeOutItem());
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
	else
	{
		// スタミナが0以下になったらスタミナを0に設定し、休憩状態に移行
		if (m_Status.m_fStamina <= 0.0f)
		{
			m_Status.m_fStamina = 0.0f;
			m_ePrevState = m_eCurrentState;
			m_eCurrentState = WorkState::Resting;
			return;
		}

		// 運搬が完了したら再び標的オブジェクトを探して移動する状態に戻る
		m_ePrevState = m_eCurrentState;
		m_eCurrentState = m_ePrevState;
	}
}

/****************************************//*
	@brief　	|　休憩中の処理
*//****************************************/
void CGatherer_Strategy::RestingAction()
{
	if (m_pOwner->HasItem())
	{
		// アイテムを所持している場合は先に貯蔵庫に運搬する
		m_ePrevState = m_eCurrentState;
		m_eCurrentState = WorkState::Transporting;

		return;
	}

	// 休憩が完了したら再び標的オブジェクトを探して移動する状態に戻る
	if (RestAction())
	{
		m_Status.m_fStamina = m_Status.m_fMaxStamina;

		if (m_ePrevState != m_eCurrentState)
		{
			m_ePrevState = m_eCurrentState;
			m_eCurrentState = m_eCurrentState;
		}
		else
		{
			m_ePrevState = m_eCurrentState;
			m_eCurrentState = WorkState::SearchAndMove;
		}
	}
}
