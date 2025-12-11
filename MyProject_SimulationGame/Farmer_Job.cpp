/**************************************************//*
	@file	| Farmer_Job.cpp
	@brief	| 農夫の職業クラス実装
	@note	| 農夫の職業クラスを実装
			| CCrafter_Strategyを継承
*//**************************************************/
#include "Farmer_Job.h"
#include "ImguiSystem.h"
#include "Main.h"
#include "BuildManager.h"
#include "StorageHouse.h"

/*****************************************//*
	@brief　	| コンストラクタ
*//*****************************************/
CFarmer_Job::CFarmer_Job()
	: CCrafter_Strategy()
	, m_eCurrentState(WorkState::TendingCrops)
	, m_ePrevState(WorkState::TendingCrops)
{
	// 労働力の設定
	m_Status.m_fWorkPower = 10.0f;
	// スタミナの設定
	m_Status.m_fStamina = 100.0f;
	m_Status.m_fMaxStamina = m_Status.m_fStamina;
}

/*****************************************//*
	@brief　	| 仕事処理
*//*****************************************/
void CFarmer_Job::DoWork()
{
	switch (m_eCurrentState)
	{
	case CFarmer_Job::WorkState::TendingCrops:
		TendingCropsAction();
		break;
	case CFarmer_Job::WorkState::RetrievingSeeds:
		RetrievingSeedsAction();
		break;
	case CFarmer_Job::WorkState::Harvesting:
		HarvestingAction();
		break;
	case CFarmer_Job::WorkState::TransportingCrops:
		TransportingCropsAction();
		break;
	case CFarmer_Job::WorkState::Resting:
		RestingAction();
		break;
	}
}

/*****************************************//*
	@brief　	| 切り替え処理
*//*****************************************/
void CFarmer_Job::OnChangeJob()
{
	// 仕事状態を種採取中にリセット
	m_eCurrentState = WorkState::TendingCrops;
	m_ePrevState = WorkState::TendingCrops;
}

/*****************************************//*
	@brief　	| インスペクター表示処理
	@param		| isEnd：インスペクター終了フラグ
	@return		| 表示した項目数
*//*****************************************/
int CFarmer_Job::Inspecter(bool isEnd)
{
	int itemCount = 0;

	ImGui::BeginChild("Farmer Job Inspector", ImVec2(0, 150), true);

	// 職業名の表示
	ImGui::Text(std::string(u8"職業名:" + GetJobName()).c_str());
	// ステータスの表示
	ImGui::Text(std::string(u8"労働力:" + std::to_string(m_Status.m_fWorkPower)).c_str());
	ImGui::Text(std::string(u8"スタミナ:" + std::to_string(m_Status.m_fStamina) + "/" + std::to_string(m_Status.m_fMaxStamina)).c_str());
	// 現在の仕事状態の表示
	ImGui::Text(u8"現在の状態: ");
	ImGui::SameLine();
	switch (m_eCurrentState)
	{
	case CFarmer_Job::WorkState::TendingCrops:
		ImGui::Text(u8"作物の世話中");
		break;
	case CFarmer_Job::WorkState::RetrievingSeeds:
		ImGui::Text(u8"種を貯蔵庫に取りに行く");
		break;
	case CFarmer_Job::WorkState::Harvesting:
		ImGui::Text(u8"収穫中");
		break;
	case CFarmer_Job::WorkState::TransportingCrops:
		ImGui::Text(u8"貯蔵庫に運搬中");
		break;
	case CFarmer_Job::WorkState::Resting:
		ImGui::Text(u8"休憩中");
		break;
	}
	ImGui::EndChild();
	itemCount++;
	if (isEnd)
	{
		ImGui::End();
	}
	return itemCount;
}

/*****************************************//*
	@brief　	| 職業ステータスのImGui描画処理
*//*****************************************/
void CFarmer_Job::DrawJobStatusImGui()
{
	// 職業ステータスの基本表示
	IJob_Strategy::DrawJobStatusImGui();

	// 現在の仕事状態の表示
	ImGui::Text(u8"現在状態:");
	switch (m_eCurrentState)
	{
	case CFarmer_Job::WorkState::TendingCrops:
		ImGui::Text(u8"作物の世話中");
		break;
	case CFarmer_Job::WorkState::RetrievingSeeds:
		ImGui::Text(u8"種を貯蔵庫に取りに行く");
		break;
	case CFarmer_Job::WorkState::Harvesting:
		ImGui::Text(u8"収穫中");
		break;
	case CFarmer_Job::WorkState::TransportingCrops:
		ImGui::Text(u8"貯蔵庫に運搬中");
		break;
	case CFarmer_Job::WorkState::Resting:
		ImGui::Text("休憩中");
		break;
	}
}

/*****************************************//*
	@brief　	| 作物の世話をする処理
*//*****************************************/
void CFarmer_Job::TendingCropsAction()
{
	// シーンの取得
	CScene* pScene = GetScene();

	// すべての農作施設を取得
	auto FarmFacilities = pScene->GetGameObjects<CFarmFacility>();

	// 農作施設が一つもない場合は建築マネージャーに農作施設の建築を依頼し、処理を抜ける
	if (FarmFacilities.empty())
	{
		// 建築マネージャーに農作施設の建築を依頼
		CBuildManager::GetInstance()->AddBuildRequest(CBuildManager::BuildType::FarmFacility);

		// 処理を抜ける
		return;
	}

	// 貯蔵庫を取得
	CStorageHouse* storageHouse = pScene->GetGameObject<CStorageHouse>();

	// 所持している種アイテムを取得
	std::list<CItem*> hasSeedItems;

	// 所持アイテムリストを走査
	for (const auto& item : m_pOwner->GetItemList())
	{
		// 種アイテムの場合
		if (item->GetItemCategory() == CItem::ITEM_CATEGORY::Seed)
		{
			// リストに追加
			hasSeedItems.push_back(item);
			break;
		}
	}

	// 貯蔵庫に種カテゴリーのアイテムがある場合
	if (storageHouse->HasItemCategory(CItem::ITEM_CATEGORY::Seed))
	{

		// 所持している種アイテムがない場合は処理を抜ける
		if (hasSeedItems.empty())
		{
			// 種を倉庫に取りに行く状態に移行
			m_ePrevState = m_eCurrentState;
			m_eCurrentState = WorkState::RetrievingSeeds;
			return;
		}
	}
	// 目標農作施設が未設定の場合
	if (m_pTargetFarmFacility == nullptr)
	{
		// 取得した農作施設を走査
		for (auto FarmFacility : FarmFacilities)
		{
			// 農作施設に完成した作物がある場合
			if (FarmFacility->HasCompletedCrops())
			{
				m_pTargetFarmFacility = FarmFacility;
				m_eCurrentState = WorkState::Harvesting;
				m_ePrevState = m_eCurrentState;
				return;
			}

			// 所持している種アイテムがある場合
			if (!hasSeedItems.empty())
			{
				// 農作施設が作物を追加可能かどうかを確認
				if (FarmFacility->CanAddCrop())
				{
					m_pTargetFarmFacility = FarmFacility;
				}
			}
		}
	}
	else
	{
		// 目標施設に向かう
		if (!m_pOwner->MoveToTarget(m_pTargetFarmFacility, Human_Move_Speed))return;

		// 所持している種アイテムがある場合
		if (!hasSeedItems.empty())
		{
			// 目標施設に到着したら種を植える
			CItem* pSeedItem = hasSeedItems.front();
			m_pTargetFarmFacility->AddCrop(pSeedItem->GetItemType());
			// 空腹値を消費
			m_pOwner->DecreaseHunger(Work_Hunger_Decrease);
			// スタミナを消費
			m_Status.m_fStamina -= Job_Work_Stamina_Decrease;
			// スタミナが0以下になった場合は休憩状態に移行
			if (m_Status.m_fStamina <= 0.0f)
			{
				m_ePrevState = m_eCurrentState;
				m_eCurrentState = WorkState::Resting;
			}

			// 所持アイテムリストから種アイテムを削除
			m_pOwner->TakeOutItem(pSeedItem->GetItemType());

			// 目標農作施設が作物を追加できない場合
			if (!m_pTargetFarmFacility->CanAddCrop())
			{
				// 目標農作施設をリセット
				m_pTargetFarmFacility = nullptr;
				return;
			}
		}

		// 目標農作施設に完成した作物がある場合
		if (m_pTargetFarmFacility->HasCompletedCrops())
		{
			m_ePrevState = m_eCurrentState;
			m_eCurrentState = WorkState::Harvesting;
		}
	}
}

/*****************************************//*
	@brief　	| 種を倉庫に取りに行く処理
*//*****************************************/
void CFarmer_Job::RetrievingSeedsAction()
{
	// 貯蔵庫を探す
	CStorageHouse* storageHouse = GetScene()->GetGameObject<CStorageHouse>();

	// 貯蔵庫の位置まで移動
	if (!m_pOwner->MoveToTarget(storageHouse, Human_Move_Speed))return;

	// 種を取り出す
	CItem* pSeedItem = storageHouse->TakeOutItem(CItem::ITEM_CATEGORY::Seed);

	// 種を所持アイテムリストに追加
	if (pSeedItem != nullptr)
	{
		// 所持アイテムリストに追加
		m_pOwner->HoldItem(pSeedItem);
	}

	// 種カテゴリーのアイテムがない場合は作物の世話状態に戻る
	if (!storageHouse->HasItemCategory(CItem::ITEM_CATEGORY::Seed))
	{
		// 作物の世話状態に戻る
		m_ePrevState = m_eCurrentState;
		m_eCurrentState = WorkState::TendingCrops;
		return;
	}
}

/*****************************************//*
	@brief　	| 収穫する処理
*//*****************************************/
void CFarmer_Job::HarvestingAction()
{
	// 目標農作施設が未設定の場合は待機状態に戻る
	if (m_pTargetFarmFacility == nullptr)
	{
		m_ePrevState = m_eCurrentState;
		m_eCurrentState = WorkState::TendingCrops;
		return;
	}

	// 目標施設に向かう
	if (!m_pOwner->MoveToTarget(m_pTargetFarmFacility, Human_Move_Speed))return;

	// 目標施設に到着したら完成した作物を収穫する
	CItem* pCompletedCrop = m_pTargetFarmFacility->TakeCompletedCrop();

	// 収穫した作物を所持アイテムリストに追加
	if (pCompletedCrop != nullptr)
	{
		// 所持アイテムリストに追加
		m_pOwner->HoldItem(pCompletedCrop);
		// 空腹値を消費
		m_pOwner->DecreaseHunger(Work_Hunger_Decrease);
		// スタミナを消費
		m_Status.m_fStamina -= Job_Work_Stamina_Decrease;

		// スタミナが0以下になった場合は休憩状態に移行
		if (m_Status.m_fStamina <= 0.0f)
		{
			m_ePrevState = m_eCurrentState;
			m_eCurrentState = WorkState::Resting;
		}
	}

	// 目標農作施設に完成した作物が無くなった場合
	if (!m_pTargetFarmFacility->HasCompletedCrops())
	{
		// 目標農作施設をリセット
		m_pTargetFarmFacility = nullptr;
		// 作物の世話状態に戻る
		m_ePrevState = m_eCurrentState;
		m_eCurrentState = WorkState::TransportingCrops;
	}
}

/*****************************************//*
	@brief　	| 作物を運搬する処理
*//*****************************************/
void CFarmer_Job::TransportingCropsAction()
{
	// アイテムを所持していない場合は待機状態に戻る
	if (!m_pOwner->HasItem())
	{
		m_ePrevState = m_eCurrentState;
		m_eCurrentState = WorkState::TendingCrops;
		return;
	}

	// 倉庫を探して向かう
	CStorageHouse* storageHouse = GetScene()->GetGameObject<CStorageHouse>();

	// 倉庫の位置まで移動
	if (!m_pOwner->MoveToTarget(storageHouse, Human_Move_Speed))return;

	// 倉庫に到着したら所持しているアイテムを収納する
	storageHouse->StoreItem(m_pOwner->TakeOutItem());
}

/*****************************************//*
	@brief　	| 休憩する処理
*//*****************************************/
void CFarmer_Job::RestingAction()
{
	// 休憩が完了したら再び待機状態に戻る
	if (RestAction())
	{
		// スタミナを最大に回復
		m_Status.m_fStamina = m_Status.m_fMaxStamina;

		// 前回の状態に戻る
		// ※もし前回の状態と現在の状態が同じになってしまっている場合は作物の世話状態に戻る
		if (m_ePrevState != m_eCurrentState)
		{
			m_eCurrentState = m_ePrevState;
			m_ePrevState = WorkState::Resting;
		}
		else
		{
			m_ePrevState = m_eCurrentState;
			m_eCurrentState = WorkState::TendingCrops;
		}
	}
}
