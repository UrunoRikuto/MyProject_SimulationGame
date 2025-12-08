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
#include "ImguiSystem.h"
#include "Main.h"
#include "CivLevelManager.h"
#include "Item_Material.h"

/*****************************************//*
	@brief　	| コンストラクタ
*//*****************************************/
CBuilder_Job::CBuilder_Job()
	: CCrafter_Strategy()
	, m_eCurrentState(WorkState::Resting)
	, m_ePrevState(WorkState::Resting)
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
		// 待機中
	case WorkState::Waiting:			
		WaitingAction();			
		break;

		// 素材収集中
	case WorkState::GatheringMaterials:
		GatherMaterialsAction();	
		break;

		// 対象探索中
	case WorkState::SearchingTarget:	
		SearchingTargetAction();	
		break;

		// 建築中
	case WorkState::Building:			
		BuildingAction();			
		break;
	
		// 強化中
	case WorkState::Upgrading:			
		UpgradingAction();			
		break;
	
		// 休憩中
	case WorkState::Resting:			
		RestingAction();			
		break;
	}
}

/*****************************************//*
	@brief	| 職業切り替え時の処理
*//*****************************************/
void CBuilder_Job::OnChangeJob()
{
	// 建築中の建築オブジェクトが存在する場合
	if (m_pBuildingObject != nullptr)
	{
		// 建築オブジェクトの建築進行度をリセット
		m_pBuildingObject = nullptr;
	}
	// 受けている建築依頼をリセット
	if (m_pCurrentBuildRequest != nullptr)
	{
		CBuildManager::GetInstance()->ResetBuildRequest(m_pCurrentBuildRequest);
		m_pCurrentBuildRequest = nullptr;
	}

	// 仕事状態を待機中にリセット
	m_eCurrentState = WorkState::Waiting;
	m_ePrevState = WorkState::Waiting;
}

/*****************************************//*
	@brief	| インスペクター表示処理
	@param	| isEnd：インスペクター終了フラグ
	@return	| 表示した項目数
*//*****************************************/
int CBuilder_Job::Inspecter(bool isEnd)
{
	int nItemCount = 0;

	ImGui::BeginChild("Builder Job Inspector", ImVec2(0, 150), true);

	// 職業名の表示
	ImGui::Text(std::string("JobName:" + GetJobName()).c_str());

	// ステータスの表示
	ImGui::Text(std::string("WorkPower:" + std::to_string(m_Status.m_fWorkPower)).c_str());
	ImGui::Text(std::string("Stamina:" + std::to_string(m_Status.m_fStamina) + "/" + std::to_string(m_Status.m_fMaxStamina)).c_str());

	// 現在の仕事状態を表示
	switch (m_eCurrentState)
	{
	case WorkState::Waiting:
		ImGui::Text("Current State: Waiting");
		break;
	case WorkState::GatheringMaterials:
		ImGui::Text("Current State: Gathering Materials");
		break;
	case WorkState::SearchingTarget:
		ImGui::Text("Current State: Searching Target");
		break;
	case WorkState::Building:
		ImGui::Text("Current State: Building");
		break;
	case WorkState::Upgrading:
		ImGui::Text("Current State: Upgrading");
		break;
	case WorkState::Resting:
		ImGui::Text("Current State: Resting");
		break;
	}

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

/*****************************************//*
	@brief	| 職業ステータスのImGui描画処理
*//*****************************************/
void CBuilder_Job::DrawJobStatusImGui()
{
	// 職業ステータスの基本表示
	IJob_Strategy::DrawJobStatusImGui();

	// 受けている建築依頼の表示
	ImGui::Text("Request: ");
	ImGui::SameLine();
	if (ImGui::Button("Detail"))
	{
		m_isShowRequestDetail = !m_isShowRequestDetail;
	}
	if (m_pCurrentBuildRequest != nullptr)
	{
		// 依頼タイプの文字列
		std::string requestTypeStr = CBuildManager::REQUEST_TYPE_TO_STRING(m_pCurrentBuildRequest->eRequestType);
		// 建築物タイプの文字列
		std::string buildTypeStr = CBuildManager::BUILD_TYPE_TO_STRING(m_pCurrentBuildRequest->eBuildType);

		ImGui::Text(buildTypeStr.c_str());
		ImGui::Text(">>%s", requestTypeStr.c_str());

		if (m_isShowRequestDetail)
		{
			DrawBuildRequestDetailImGui();
		}
	}
	else
	{
		ImGui::Text("NoRequest");
	}
}

/*****************************************//*
	@brief	| 建築依頼詳細のImGui描画処理
*//*****************************************/
void CBuilder_Job::DrawBuildRequestDetailImGui()
{
	if (m_pCurrentBuildRequest == nullptr)return;

	// ウィンドウ背景色の設定
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

	ImGui::Begin("Build Request Detail", &m_isShowRequestDetail, ImGuiWindowFlags_AlwaysAutoResize);

	// 依頼タイプの文字列
	std::string requestTypeStr = CBuildManager::REQUEST_TYPE_TO_STRING(m_pCurrentBuildRequest->eRequestType);
	ImGui::Text("Request Type: %s", requestTypeStr.c_str());

	// 建築物タイプの文字列
	std::string buildTypeStr = CBuildManager::BUILD_TYPE_TO_STRING(m_pCurrentBuildRequest->eBuildType);
	ImGui::Text("Build Type: %s", buildTypeStr.c_str());

	// 必要素材の表示
	ImGui::Text("Required Materials:");
	const auto requiredMaterials = BuildMaterials::GetBuildMaterials(m_pCurrentBuildRequest->eBuildType,
		(m_pCurrentBuildRequest->eRequestType == CBuildManager::RequestType::Build) ? 0 : m_pBuildingObject->GetBuildLevel());
	for (const auto& material : requiredMaterials)
	{
		std::string itemTypeStr = CItem::ITEM_TYPE_TO_STRING(material.eItemType);
		ImGui::Text("- %s: %d", itemTypeStr.c_str(), material.nRequiredAmount);
	}

	ImGui::End();
	ImGui::PopStyleColor();
}

/*****************************************//*
	@brief	| 待機中の処理
*//*****************************************/
void CBuilder_Job::WaitingAction()
{
	// 建築依頼がない場合は処理を抜ける
	if (!CBuildManager::GetInstance()->HasBuildRequest())return;

	// 建築依頼を受け取る
	m_pCurrentBuildRequest = CBuildManager::GetInstance()->TakeBuildRequest();

	// 建築依頼がない場合は処理を抜ける
	if (m_pCurrentBuildRequest == nullptr)return;

	// インデックスを取得
	DirectX::XMINT2 buildIndex = m_pCurrentBuildRequest->n2BuildIndex;

	// セルを取得
	auto cell = CFieldManager::GetInstance()->GetFieldGrid()->GetFieldCells()[buildIndex.x][buildIndex.y];

	// セルが使用中の場合
	if (cell->IsUse())
	{
		CBuildObject* pObj = dynamic_cast<CBuildObject*>(cell->GetObject());
		if (pObj != nullptr)
		{
			// 建築完成度を初期化
			pObj->InitializeBuildProgress();

			// 建築オブジェクトのポインタを設定
			m_pBuildingObject = pObj;
		}
	}

	// 前の状態を保存
	m_ePrevState = m_eCurrentState;
	// 建築対象探索状態に移行
	m_eCurrentState = WorkState::GatheringMaterials;
}

/*****************************************//*
	@brief	| 素材収集中の処理
*//*****************************************/
void CBuilder_Job::GatherMaterialsAction()
{
	// 建築物の依頼レベル
	int nRequiredLevel = 1;

	switch (m_pCurrentBuildRequest->eRequestType)
	{
	case CBuildManager::RequestType::Build:
	{
		// 建築物の依頼レベルを設定
		nRequiredLevel = 0;
		break;
	}
	case CBuildManager::RequestType::Upgrade:
	{
		// 建築物の依頼レベルを設定
		nRequiredLevel = m_pBuildingObject->GetBuildLevel();
		break;
	}
	}

	// 必要素材の取得
	const auto requiredMaterials = ::BuildMaterials::GetBuildMaterials(m_pCurrentBuildRequest->eBuildType, nRequiredLevel);

	// 貯蔵庫を取得
	auto storageHouses = GetScene()->GetGameObject<CStorageHouse>();

	// 貯蔵庫が存在しない場合は待機状態に戻る
	if (storageHouses == nullptr)
	{
		// 前の状態を保存
		m_ePrevState = m_eCurrentState;
		m_eCurrentState = WorkState::Waiting;
		return;
	}

	// 貯蔵庫へ移動
	// 目的地へ到達していない場合は処理を抜ける
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
	for (int i = 0;i < requiredMaterials.size(); i++)
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
		// 前の状態を保存
		m_ePrevState = m_eCurrentState;
		// 目的地探索状態に移行
		m_eCurrentState = WorkState::SearchingTarget;
	}
}

/*****************************************//*
	@brief	| 対象探索中の処理
*//*****************************************/
void CBuilder_Job::SearchingTargetAction()
{
	// 建築依頼が無効な場合は待機状態に戻る
	if (m_pCurrentBuildRequest == nullptr)
	{
		// 前の状態を保存
		m_ePrevState = m_eCurrentState;
		m_eCurrentState = WorkState::Waiting;
		return;
	}
	// 目的位置のインデックスを取り出す
	DirectX::XMINT2 targetIndex = m_pCurrentBuildRequest->n2BuildIndex;

	// 目的のフィールドセルの位置を取得
	DirectX::XMFLOAT3 targetPosition = CFieldManager::GetInstance()->GetFieldGrid()->GetFieldCells()[targetIndex.x][targetIndex.y]->GetPos();

	// 所属しているオブジェクトを目的地に移動させる
	DirectX::XMFLOAT3 ownerPos = m_pOwner->GetPos();

	// オブジェクトとオーナーの位置の距離を計算
	float fDistance = StructMath::Distance(ownerPos, targetPosition);

	// 一定距離以内に到達したら収集状態に移行
	if (fDistance < 1.0f)
	{
		switch (m_pCurrentBuildRequest->eRequestType)
		{
		case CBuildManager::RequestType::Build:
		{
			// 前の状態を保存
			m_ePrevState = m_eCurrentState;
			m_eCurrentState = WorkState::Building;
			break;
		}
		case CBuildManager::RequestType::Upgrade:
		{
			// 前の状態を保存
			m_ePrevState = m_eCurrentState;
			m_eCurrentState = WorkState::Upgrading;
			break;
		}
		}
		return;
	}

	DirectX::XMFLOAT3 f3Diff = targetPosition - ownerPos;
	// オーナーからオブジェクトへのベクトルを計算
	DirectX::XMVECTOR f3Direction = DirectX::XMLoadFloat3(&f3Diff);
	f3Direction = DirectX::XMVector3Normalize(f3Direction);

	// オーナーの位置をオブジェクトに向かって少しずつ移動させる
	DirectX::XMFLOAT3 f3Move;
	DirectX::XMStoreFloat3(&f3Move, f3Direction);

	ownerPos += f3Move * Human_Move_Speed;

	// オーナーの位置を更新
	m_pOwner->SetPos(ownerPos);

}

/*****************************************//*
	@brief	| 建築中の処理
*//*****************************************/
void CBuilder_Job::BuildingAction()
{
	// 建築オブジェクトのポインタを取得
	if (m_pBuildingObject == nullptr)
	{
		// 建築進行度が100%に達していない場合は進行度を増加させる
		if (m_fWorkProgress <= 100.0f)
		{
			m_fWorkProgress += m_Status.m_fWorkPower / fFPS;
			return;
		}
		// 建築オブジェクトをシーンに追加
		m_pBuildingObject = CBuildManager::CreateBuildObjectByType(m_pCurrentBuildRequest->eBuildType);

		if (m_pBuildingObject == nullptr)
		{
			// 前の状態を保存
			m_ePrevState = m_eCurrentState;
			// 建築オブジェクトが見つからなかった場合は待機状態に戻る
			m_eCurrentState = WorkState::Waiting;

			// 建築依頼を未処理状態に設定
			m_pCurrentBuildRequest->eRequestState = REQUEST_STATE::Unprocessed;
			// 建築依頼のポインタをリセット
			m_pCurrentBuildRequest = nullptr;

			return;
		}

		// 建築オブジェクトの位置を設定
		DirectX::XMINT2 buildIndex = m_pCurrentBuildRequest->n2BuildIndex;
		auto cell = CFieldManager::GetInstance()->GetFieldGrid()->GetFieldCells()[buildIndex.x][buildIndex.y];
		m_pBuildingObject->SetPos(cell->GetPos());
		m_pBuildingObject->SetFieldCellIndex(buildIndex);
		cell->SetUse(true);
		cell->SetObject(m_pBuildingObject);
	}

	// 目的地に到達していない場合は処理を抜ける
	if(!m_pOwner->MoveToTarget(m_pBuildingObject, Human_Move_Speed))return;

	// 建築進行度を増加させる
	m_pBuildingObject->ProgressBuild(m_Status.m_fWorkPower);
	// 空腹度を減少させる
	m_pOwner->DecreaseHunger(Work_Hunger_Decrease);
	// スタミナを減少させる
	m_Status.m_fStamina -= Job_Work_Stamina_Decrease;

	// スタミナが0以下になったらスタミナを0に設定し、休憩状態に移行
	if (m_Status.m_fStamina <= 0.0f)
	{
		m_Status.m_fStamina = 0.0f;
		// 前の状態を保存
		m_ePrevState = m_eCurrentState;
		// 休憩状態に移行
		m_eCurrentState = WorkState::Resting;
		return;
	}

	// 完成した場合は待機状態に移行
	if (m_pBuildingObject->IsCompleted())
	{
		// 文明経験値を加算
		CCivLevelManager::GetInstance()->AddExp(CCivLevelManager::ExpType::Building);

		// 建築依頼を完了状態に設定
		CBuildManager::GetInstance()->CompleteBuildRequest(m_pCurrentBuildRequest);

		// 所持しているアイテムを全て手放す
		while (!m_pOwner->GetItemList().empty())
		{
			m_pOwner->TakeOutItem();
		}

		// 建築オブジェクトのポインタをリセット
		m_pBuildingObject = nullptr;

		// 建築依頼のポインタをリセット
		m_pCurrentBuildRequest = nullptr;

		// 前の状態を保存
		m_ePrevState = m_eCurrentState;
		// 待機状態に移行
		m_eCurrentState = WorkState::Waiting;
	}
}

/*****************************************//*
	@brief	| 強化中の処理
*//*****************************************/
void CBuilder_Job::UpgradingAction()
{
	// 建築オブジェクトのポインタを取得
	if (m_pBuildingObject == nullptr)
	{
		// 前の状態を保存
		m_ePrevState = m_eCurrentState;
		// 建築オブジェクトが見つからなかった場合は待機状態に戻る
		m_eCurrentState = WorkState::Waiting;
		// 建築依頼を未処理状態に設定
		m_pCurrentBuildRequest->eRequestState = REQUEST_STATE::Unprocessed;
		// 建築依頼のポインタをリセット
		m_pCurrentBuildRequest = nullptr;

		return;
	}

	// 目的地に到達していない場合は処理を抜ける
	if (!m_pOwner->MoveToTarget(m_pBuildingObject, Human_Move_Speed))return;

	// 建築進行度を増加させる
	m_pBuildingObject->ProgressBuild(m_Status.m_fWorkPower);
	// 空腹度を減少させる
	m_pOwner->DecreaseHunger(Work_Hunger_Decrease);
	// スタミナを減少させる
	m_Status.m_fStamina -= Job_Work_Stamina_Decrease;

	// スタミナが0以下になったらスタミナを0に設定し、休憩状態に移行
	if (m_Status.m_fStamina <= 0.0f)
	{
		m_Status.m_fStamina = 0.0f;
		// 前の状態を保存
		m_ePrevState = m_eCurrentState;
		// 休憩状態に移行
		m_eCurrentState = WorkState::Resting;
		return;
	}

	// 建築進行度が最大値に達した場合は待機状態に移行
	if (m_pBuildingObject->IsCompleted())
	{
		// 建築物のレベルをアップグレード
		m_pBuildingObject->UpgradeBuildLevel();

		// 文明経験値を加算
		CCivLevelManager::GetInstance()->AddExp(CCivLevelManager::ExpType::Building);

		// 建築依頼を完了状態に設定
		CBuildManager::GetInstance()->CompleteBuildRequest(m_pCurrentBuildRequest);

		// 所持しているアイテムを全て手放す
		while (!m_pOwner->GetItemList().empty())
		{
			m_pOwner->TakeOutItem();
		}

		// 建築オブジェクトのポインタをリセット
		m_pBuildingObject = nullptr;
		// 建築依頼のポインタをリセット
		m_pCurrentBuildRequest = nullptr;
		// 前の状態を保存
		m_ePrevState = m_eCurrentState;
		// 待機状態に移行
		m_eCurrentState = WorkState::Waiting;
	}
}

/*****************************************//*
	@brief	| 休憩中の処理
*//*****************************************/
void CBuilder_Job::RestingAction()
{
	// 休憩が完了したら再び待機状態に戻る
	if (RestAction())
	{
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