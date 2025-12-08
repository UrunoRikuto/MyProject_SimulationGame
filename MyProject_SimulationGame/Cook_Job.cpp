/**************************************************//*
	@file	| Cook_Job.cpp
	@brief	| 料理職業クラスのcppファイル
	@note	| 料理職業の処理を定義
			| CCrafter_Strategyを継承
*//**************************************************/
#include "Cook_Job.h"
#include "ImguiSystem.h"
#include "StorageHouse.h"
#include "Main.h"
#include "BuildManager.h"
#include "Item_Material.h"

/****************************************//*
	@brief　	| コンストラクタ
*//****************************************/
CCook_Job::CCook_Job()
	: CCrafter_Strategy()
	, m_eCurrentState(WorkState::Resting)
	, m_ePrevState(WorkState::Resting)
	, m_pRequest(nullptr)
	, m_pFoodFactory(nullptr)
	, m_fCoolTime(0.0f)
{
	// 労働力の設定
	m_Status.m_fWorkPower = 15.0f;
	// スタミナの設定
	m_Status.m_fStamina = 100.0f;
	m_Status.m_fMaxStamina = m_Status.m_fStamina;
}

/****************************************//*
	@brief　	| 仕事処理
*//****************************************/
void CCook_Job::DoWork()
{
	// 現在の仕事状態に応じた処理を実行
	switch (m_eCurrentState)
	{
	case CCook_Job::WorkState::Waiting:
		WaitingAction();
		break;
	case CCook_Job::WorkState::GatheringMaterials:
		GatheringMaterialsAction();
		break;
	case CCook_Job::WorkState::Cooking:
		CookingAction();
		break;
	case CCook_Job::WorkState::TransportingFood:
		TransportingFoodAction();
		break;
	case CCook_Job::WorkState::Resting:
		RestingAction();
		break;
	}
}

/****************************************//*
	@brief　	| 切り替え処理
*//****************************************/
void CCook_Job::OnChangeJob()
{
	// クールタイムをリセット
	m_fCoolTime = 0.0f;

	// 依頼をリセット
	if (m_pRequest != nullptr && m_pFoodFactory != nullptr)
	{
		m_pFoodFactory->ResetRequest(m_pRequest);
		m_pRequest = nullptr;
		m_pFoodFactory = nullptr;
	}
}

/****************************************//*
	@brief　	| インスペクター表示処理
	@param isEnd：インスペクター終了フラグ
	@return 表示した項目数
*//****************************************/
int CCook_Job::Inspecter(bool isEnd)
{
	int itemCount = 0;

	// ここに料理職業のインスペクター表示処理を実装
	if (isEnd)
	{
		ImGui::End();
	}

	return itemCount;
}

/****************************************//*
	@brief　	| 職業ステータスのImGui描画処理
*//****************************************/
void CCook_Job::DrawJobStatusImGui()
{
	// 職業ステータスの基本表示
	IJob_Strategy::DrawJobStatusImGui();

	// 受けている依頼の表示
	ImGui::Text("Current Request:");
	if (m_pRequest != nullptr)
	{
		// 依頼内容の表示
		std::string ToolTypeStr = CItem::ITEM_TYPE_TO_STRING(m_pRequest->eMealType);
		std::string ProgressStr = std::to_string(m_pRequest->fCookingProgress);

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

/****************************************//*
	@brief　	| 待機処理
*//****************************************/
void CCook_Job::WaitingAction()
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

	// 一番近い食料加工施設を探す
	m_pFoodFactory = GetScene()->GetGameObject<CFoodFactory>(m_pOwner->GetPos());

	// 食料加工施設が見つからなかった場合
	if (m_pFoodFactory == nullptr)
	{
		// 食料加工施設の建設を依頼
		CBuildManager::GetInstance()->AddBuildRequest(CBuildManager::BuildType::FoodFactory);

		// 処理終了
		return;
	}

	// 食料加工施設まで移動
	if (!m_pOwner->MoveToTarget(m_pFoodFactory, Human_Move_Speed))return;

	// 料理依頼を取得
	m_pRequest = m_pFoodFactory->TakeRequest();

	// 料理依頼がなかった場合は処理終了
	if (m_pRequest != nullptr)
	{
		// 仕事状態を素材収集中に変更
		m_ePrevState = m_eCurrentState;
		m_eCurrentState = WorkState::GatheringMaterials;
	}
}

/****************************************//*
	@brief　	| 素材収集処理
*//****************************************/
void CCook_Job::GatheringMaterialsAction()
{
	// 貯蔵庫の取得
	CStorageHouse* pStorageHouse = GetScene()->GetGameObject<CStorageHouse>(m_pOwner->GetPos());

	// 貯蔵庫の位置まで移動
	if (!m_pOwner->MoveToTarget(pStorageHouse, Human_Move_Speed))return;

	// 必要素材の取得
	const std::vector<CItem::Material> requiredMaterials = CookMaterials::GetCookMaterials(m_pRequest->eMealType);

	// 素材収集処理
	std::vector<CItem::Material> HasMaterials;
	// 所持素材リストのサイズを必要素材リストのサイズに合わせる
	HasMaterials.resize(requiredMaterials.size());

	// 所持しているアイテムリストを取得
	const std::vector<CItem*> ownedItems = m_pOwner->GetItemList();

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
				auto Item = pStorageHouse->TakeOutItem(requiredMaterials[i].eItemType);

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
		// 持っているアイテムを全て手放す
		while (m_pOwner->HasItem())
		{
			m_pOwner->TakeOutItem();
		}

		// 仕事状態を製作中に変更
		m_ePrevState = m_eCurrentState;
		m_eCurrentState = WorkState::Cooking;
	}
}

/****************************************//*
	@brief　	| 料理処理
*//****************************************/
void CCook_Job::CookingAction()
{
	// 食料加工施設まで移動
	if (!m_pOwner->MoveToTarget(m_pFoodFactory, Human_Move_Speed))return;

	// 料理依頼を進める
	std::vector<CItem*> pCookedItems = m_pFoodFactory->ProgressRequest(m_pRequest);

	// 空腹値の減少
	m_pOwner->DecreaseHunger(Work_Hunger_Decrease);
	// スタミナを減少
	m_Status.m_fStamina -= Job_Work_Stamina_Decrease;

	// 料理が完成した場合
	if (!pCookedItems.empty())
	{
		for (auto Item : pCookedItems)
		{
			// 料理を所持する
			m_pOwner->HoldItem(Item);
		}

		// 仕事状態を料理運搬中に変更
		m_ePrevState = m_eCurrentState;
		m_eCurrentState = WorkState::TransportingFood;
	}

	// スタミナが0以下になったらスタミナを0に設定し、休憩状態に移行
	if (m_Status.m_fStamina <= 0.0f)
	{
		m_Status.m_fStamina = 0.0f;
		m_ePrevState = m_eCurrentState;
		m_eCurrentState = WorkState::Resting;
	}
}

/****************************************//*
	@brief　	| 料理運搬処理
*//****************************************/
void CCook_Job::TransportingFoodAction()
{
	// 貯蔵庫の取得
	CStorageHouse* pStorageHouse = GetScene()->GetGameObject<CStorageHouse>(m_pOwner->GetPos());

	// 貯蔵庫の位置まで移動
	if (!m_pOwner->MoveToTarget(pStorageHouse, Human_Move_Speed))return;

	// 所持しているアイテムを貯蔵庫に収納
	while (m_pOwner->HasItem())
	{
		CItem* pItem = m_pOwner->TakeOutItem();
		pStorageHouse->StoreItem(pItem);
	}

	// 依頼を完了状態にする
	m_pFoodFactory->CompleteRequest(m_pRequest);
	m_pRequest = nullptr;
	m_pFoodFactory = nullptr;

	// クールタイムを設定
	m_fCoolTime = COOL_TIME_DURATION;

	// 仕事状態を待機中に変更
	m_ePrevState = m_eCurrentState;
	m_eCurrentState = WorkState::Waiting;
}

/****************************************//*
	@brief　	| 休憩処理
*//****************************************/
void CCook_Job::RestingAction()
{
	// 休憩が完了したら再び待機状態に戻る
	if (RestAction())
	{
		// スタミナを最大に回復
		m_Status.m_fStamina = m_Status.m_fMaxStamina;

		if (m_ePrevState != m_eCurrentState)
		{
			// 前の状態を保存
			m_eCurrentState = m_ePrevState;
			m_ePrevState = WorkState::Resting;
		}
		else
		{
			// 前の状態を保存
			m_ePrevState = m_eCurrentState;
			m_eCurrentState = WorkState::Waiting;
		}
	}
}
