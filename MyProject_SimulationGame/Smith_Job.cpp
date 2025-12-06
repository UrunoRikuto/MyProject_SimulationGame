/**************************************************//*
	@file	| Smith_Job.cpp
	@brief	| 鍛冶職業クラスのcppファイル
	@note	| 鍛冶職業の処理を実装
			| CCrafter_Strategyを継承
*//**************************************************/
#include "Smith_Job.h"
#include "ImguiSystem.h"
#include "BlackSmith.h"
#include "Main.h"
#include "BuildManager.h"
#include "StorageHouse.h"
#include "Item_Material.h"

/*****************************************//*
	@brief　	| コンストラクタ
*//*****************************************/
CSmith_Job::CSmith_Job()
	: CCrafter_Strategy()
	, m_eCurrentState(WorkState::Resting)
	, m_ePrevState(WorkState::Resting)
	, m_pRequest(nullptr)
	, m_fCoolTime(0.0f)
{
	// 労働力の設定
	m_Status.m_fWorkPower = 15.0f;
	// スタミナの設定
	m_Status.m_fStamina = 100.0f;
	m_Status.m_fMaxStamina = m_Status.m_fStamina;
}

/*****************************************//*
	@brief　	| 仕事処理
*//*****************************************/
void CSmith_Job::DoWork()
{
	switch (m_eCurrentState)
	{
	case CSmith_Job::WorkState::Waiting:
		WaitingAction();
		break;
	case CSmith_Job::WorkState::GatheringMaterials:
		GatheringMaterialsAction();
		break;
	case CSmith_Job::WorkState::Crafting:
		CraftingAction();
		break;
	case CSmith_Job::WorkState::TransportingFinishedGoods:
		TransportingFinishedGoodsAction();
		break;
	case CSmith_Job::WorkState::Resting:
		RestingAction();
		break;
	}
}

/*****************************************//*
	@brief　	| 切り替え処理
*//*****************************************/
void CSmith_Job::OnChangeJob()
{
	if(m_pRequest != nullptr)
	{
		// 一番近い鍛冶屋を探す
		CBlackSmith* pNearestBlackSmith = GetScene()->GetGameObject<CBlackSmith>(m_pOwner->GetPos());
		// 依頼をリセット
		pNearestBlackSmith->ResetRequest(m_pRequest);
		// 依頼ポインタをリセット
		m_pRequest = nullptr;
	}

	// 仕事状態を待機中にリセット
	m_eCurrentState = WorkState::Waiting;
	m_ePrevState = WorkState::Waiting;
	// クールタイムをリセット
	m_fCoolTime = 0.0f;
}

/*****************************************//*
	@brief　	| インスペクター表示処理
	@param		| isEnd：インスペクター終了フラグ
	@return		| 表示した項目数
*//*****************************************/
int CSmith_Job::Inspecter(bool isEnd)
{
	int itemCount = 0;

	ImGui::BeginChild("Builder Job Inspector", ImVec2(0, 150), true);

	// 職業名の表示
	ImGui::Text(std::string("JobName:" + GetJobName()).c_str());

	// ステータスの表示
	ImGui::Text(std::string("WorkPower:" + std::to_string(m_Status.m_fWorkPower)).c_str());
	ImGui::Text(std::string("Stamina:" + std::to_string(m_Status.m_fStamina) + "/" + std::to_string(m_Status.m_fMaxStamina)).c_str());

	// 現在の仕事状態の表示
	switch (m_eCurrentState)
	{
	case CSmith_Job::WorkState::Waiting:
		ImGui::Text("CurrentState: Waiting");
		break;
	case CSmith_Job::WorkState::GatheringMaterials:
		ImGui::Text("CurrentState: GatheringMaterials");
		break;
	case CSmith_Job::WorkState::Crafting:
		ImGui::Text("CurrentState: Crafting");
		break;
	case CSmith_Job::WorkState::TransportingFinishedGoods:
		ImGui::Text("CurrentState: TransportingFinishedGoods");
		break;
	case CSmith_Job::WorkState::Resting:
		ImGui::Text("CurrentState: Resting");
		break;
	}

	ImGui::EndChild();
	itemCount++;


	if (isEnd)
	{
		ImGui::EndChild();
		ImGui::End();
		itemCount++;
	}

	return itemCount;
}

/*****************************************//*
	@brief　	| 職業ステータスのImGui描画処理
*//*****************************************/
void CSmith_Job::DrawJobStatusImGui()
{
	ImGui::Separator();
	ImGui::Text("[JobStatus]");

	// 受けている依頼の表示
	ImGui::Text("Current Request:");
	if (m_pRequest != nullptr)
	{
		// 依頼内容の表示
		std::string ToolTypeStr = CItem::ITEM_TYPE_TO_STRING(m_pRequest->eToolType);
		std::string ProgressStr = std::to_string(m_pRequest->fProductionProgress);

		ImGui::Text("%s (Progress: %s)", ToolTypeStr.c_str(), ProgressStr.c_str());
	}
	else
	{
		ImGui::Text("None");
	}

	// クールタイムの表示
	if (m_fCoolTime > 0.0f)
	{
		ImGui::Text("Cool Time: %.2f", m_fCoolTime);
	}
}

/*****************************************//*
	@brief　	| 待機処理
*//*****************************************/
void CSmith_Job::WaitingAction()
{
	// クールタイムがある場合
	if (m_fCoolTime > 0.0f)
	{
		// クールタイムを減少
		m_fCoolTime -= 1.0f; // 仮に1フレームごとに1秒減少とする
		if (m_fCoolTime < 0.0f)
		{
			m_fCoolTime = 0.0f;
		}
		return;
	}

	// 一番近い鍛冶屋を探す
	CBlackSmith* pNearestBlackSmith = GetScene()->GetGameObject<CBlackSmith>(m_pOwner->GetPos());
	// 鍛冶屋が見つからなかった場合
	if (pNearestBlackSmith == nullptr)
	{
		// 建築マネージャーに鍛冶屋の建築を依頼
		CBuildManager::GetInstance()->AddBuildRequest(CBuildManager::BuildType::BlackSmith); // 仮に人間の家を鍛冶屋とする

		// 処理終了
		return;
	}

	// 鍛冶屋に向かう
	if (!m_pOwner->MoveToTarget(pNearestBlackSmith, Human_Move_Speed))return;

	// 依頼がある場合、受ける
	m_pRequest = pNearestBlackSmith->TakeRequest();

	if (m_pRequest != nullptr)
	{
		// 仕事状態を素材収集中に変更
		m_ePrevState = m_eCurrentState;
		m_eCurrentState = WorkState::GatheringMaterials;
	}
}

/*****************************************//*
	@brief　	| 素材収集処理
*//*****************************************/
void CSmith_Job::GatheringMaterialsAction()
{
	if(m_pRequest == nullptr)
	{
		// 仕事状態を待機中に変更
		m_ePrevState = m_eCurrentState;
		m_eCurrentState = WorkState::Waiting;

		// 一番近い鍛冶屋を探す
		CBlackSmith* pNearestBlackSmith = GetScene()->GetGameObject<CBlackSmith>(m_pOwner->GetPos());
		// 依頼をリセット
		pNearestBlackSmith->ResetRequest(m_pRequest);

		// 処理終了
		return;
	}

	// 必要素材の取得
	auto requiredMaterials = ToolMaterials::GetToolMaterials(m_pRequest->eToolType);

	// 所持している素材の確認
	bool hasAllMaterials = true;
	
	// 貯蔵庫を取得
	CStorageHouse* storageHouses = GetScene()->GetGameObject<CStorageHouse>();

	// 貯蔵庫に移動
	if (!m_pOwner->MoveToTarget(storageHouses, Human_Move_Speed))return;

	// 素材収集処理
	std::vector<CItem::Material> HasMaterials;
	// 所持素材リストのサイズを必要素材リストのサイズに合わせる
	HasMaterials.resize(requiredMaterials.size());

	// 所持しているアイテムリストを取得
	const auto& ownedItems = m_pOwner->GetItemList();

	// 所持素材リストの初期化
	if (!ownedItems.empty())
	{
		// 所持しているアイテムリストから建築素材のみを抽出
		for (int i = 0; i < requiredMaterials.size(); i++)
		{
			// 所持素材リストを走査
			for (const auto& item : ownedItems)
			{
				// 素材タイプが一致する場合
				if (item->GetItemType() == requiredMaterials[i].eItemType)
				{
					// 所持素材リストに追加
					HasMaterials[i].eItemType = requiredMaterials[i].eItemType;
					HasMaterials[i].nRequiredAmount++;
				}
			}
		}
	}

	// 全ての素材が集まったかどうかのフラグ
	bool bAllMaterialsGathered = true;

	// 必要素材リストをループ
	for (int i = 0; i < requiredMaterials.size(); i++)
	{
		// 素材が不足しているかどうかをチェック
		if (HasMaterials[i].nRequiredAmount < requiredMaterials[i].nRequiredAmount)
		{
			// 必要数を求める
			int nNeeded = requiredMaterials[i].nRequiredAmount - HasMaterials[i].nRequiredAmount;
			if (nNeeded <= 0)continue;
			// 素材を収集
			for (int n = 0; n < nNeeded; ++n)
			{
				// 貯蔵庫から素材を取り出す
				auto Item = storageHouses->TakeOutItem(requiredMaterials[i].eItemType);

				// 取り出せた場合は収集数を増やす
				if (Item != nullptr)
				{
					m_pOwner->HoldItem(Item);
					HasMaterials[i].nRequiredAmount--;
					// 一つずつ取り出す
					return;
				}
				else
				{
					// 素材が足りなかった場合はフラグを折る
					bAllMaterialsGathered = false;
					// ループを抜ける
					break;
				}
			}

		}
	}

	// 全ての素材が集まった場合は次の状態に移行
	if (bAllMaterialsGathered)
	{
		// 仕事状態を製作中に変更
		m_ePrevState = m_eCurrentState;
		m_eCurrentState = WorkState::Crafting;
	}

}

/*****************************************//*
	@brief　	| 製作処理
*//*****************************************/
void CSmith_Job::CraftingAction()
{
	// 一番近い鍛冶屋を探す
	CBlackSmith* pNearestBlackSmith = GetScene()->GetGameObject<CBlackSmith>(m_pOwner->GetPos());

	// 鍛冶屋に移動
	if (!m_pOwner->MoveToTarget(pNearestBlackSmith, Human_Move_Speed))return;

	// 生産依頼を進めて、完成品を受け取る
	CItem* pProducedItem = pNearestBlackSmith->ProgressRequest(m_pRequest);

	// スタミナ値を減少
	m_Status.m_fStamina -= Job_Work_Stamina_Decrease;

	// 空腹度を消費
	m_pOwner->DecreaseHunger(Work_Hunger_Decrease);

	// スタミナが0以下になったらスタミナを0に設定し、休憩状態に移行
	if (m_Status.m_fStamina <= 0.0f)
	{
		m_Status.m_fStamina = 0.0f;
		m_ePrevState = m_eCurrentState;
		m_eCurrentState = WorkState::Resting;
		return;
	}

	// 生産が完了した場合
	if (pProducedItem != nullptr)
	{
		// 所持しているアイテムを全て手放す
		while (!m_pOwner->HasItem())
		{
			m_pOwner->TakeOutItem();
		}

		// 生産品を所持する
		m_pOwner->SetToolItem(pProducedItem);

		// 仕事状態を完成品運搬中に変更
		m_ePrevState = m_eCurrentState;
		m_eCurrentState = WorkState::TransportingFinishedGoods;
	}
}

/*****************************************//*
	@brief　	| 完成品運搬処理
*//*****************************************/
void CSmith_Job::TransportingFinishedGoodsAction()
{
	// 一番近い貯蔵庫を探す
	CStorageHouse* pNearestStorageHouse = GetScene()->GetGameObject<CStorageHouse>(m_pOwner->GetPos());

	// 貯蔵庫に移動
	if (!m_pOwner->MoveToTarget(pNearestStorageHouse, Human_Move_Speed))return;

	// 所持している完成品を貯蔵庫に預ける
	pNearestStorageHouse->StoreItem(m_pOwner->TakeOutToolItem());

	// 依頼の完了報告
	CBlackSmith* pNearestBlackSmith = GetScene()->GetGameObject<CBlackSmith>(m_pOwner->GetPos());
	pNearestBlackSmith->CompleteRequest(m_pRequest);
	// 依頼ポインタをリセット
	m_pRequest = nullptr;

	// クールタイムを設定
	m_fCoolTime = COOL_TIME_DURATION;

	// 仕事状態を休憩中に変更
	m_ePrevState = m_eCurrentState;
	m_eCurrentState = WorkState::Resting;
}

/*****************************************//*
	@brief　	| 休憩処理
*//*****************************************/
void CSmith_Job::RestingAction()
{
	// 休憩が完了したら再び待機状態に戻る
	if (RestAction())
	{
		// スタミナを最大に回復
		m_Status.m_fStamina = m_Status.m_fMaxStamina;

		if (m_ePrevState != m_eCurrentState)
		{
			// 前の状態を保存
			m_ePrevState = m_eCurrentState;
			m_eCurrentState = m_ePrevState;
		}
		else
		{
			// 前の状態を保存
			m_ePrevState = m_eCurrentState;
			m_eCurrentState = WorkState::Waiting;
		}
	}
}