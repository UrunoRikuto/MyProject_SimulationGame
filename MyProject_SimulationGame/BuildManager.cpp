/**************************************************//*
	@file	| BuildManager.cpp
	@brief	| 建築物関係の管理システムのcppファイル
	@note	| 建築物関係の管理システムを実装
			| シングルトンパターンで作成
*//**************************************************/
#include "BuildManager.h"
#include "Main.h"
#include "FieldManager.h"
#include "RefreshFacility.h"
#include "HumanHouse.h"
#include "BlackSmith.h"

// 静的メンバ変数の初期化
CBuildManager* CBuildManager::m_pInstance = nullptr;

/*****************************************//*
	@brief　	| コンストラクタ
*//*****************************************/
CBuildManager::CBuildManager()
	: m_BuildRequestList()
{
}

/*****************************************//*
	@brief	| デストラクタ
*//*****************************************/
CBuildManager::~CBuildManager()
{
}

/*****************************************//*
	@brief	| インスタンスを取得
	@return	| インスタンスのポインタ
*//*****************************************/
CBuildManager* CBuildManager::GetInstance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = new CBuildManager();
	}
	return m_pInstance;
}

/*****************************************//*
	@brief	| インスタンスを解放
*//*****************************************/
void CBuildManager::ReleaseInstance()
{
	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

/*****************************************//*
	@brief	| 建築依頼を追加
	@param	| In_eRequestType：建築物タイプ
*//*****************************************/
void CBuildManager::AddBuildRequest(const BuildType In_eRequestType)
{
	// 同じ建築物タイプの依頼が既にある場合は追加しない
	for (const auto& request : m_BuildRequestList)
	{
		if (request.eBuildType == In_eRequestType)
		{
			return;
		}
	}

	// 建築依頼構造体を作成
	BuildRequest newRequest = BuildRequest();
	
	// 生成予定場所の設定
	newRequest.n2BuildIndex = DecideRandomBuildPosition();

	// シーンの取得
	CScene* pScene = GetScene();

	switch (In_eRequestType)
	{
		// 休憩所
	case BuildType::RefreshFacility:
	{
		// 今あるリフレッシュ施設を取得
		auto buildlist = pScene->GetGameObjects<CRefreshFacility>();

		// リフレッシュ施設が無ければ建築依頼を追加
		if (buildlist.empty())
		{
			newRequest.eRequestType = RequestType::Build;
			break;
		}

		// リフレッシュ施設がある場合は強化依頼を追加
		for (auto build : buildlist)
		{
			if (!build->IsMaxBuildLevel())
			{
				newRequest.eRequestType = RequestType::Upgrade;
				newRequest.n2BuildIndex = build->GetFieldCellIndex();
				break;
			}
		}

		// 全てのリフレッシュ施設が最大レベルの場合は建築依頼を追加
		if (newRequest.eRequestType != RequestType::Upgrade)
		{
			newRequest.eRequestType = RequestType::Build;
		}
	}
	break;
	case BuildType::HumanHouse:
	{
		// 今ある人間の家を取得
		auto buildlist = pScene->GetGameObjects<CHumanHouse>();

		// 人間の家が無ければ建築依頼を追加
		if (buildlist.empty())
		{
			newRequest.eRequestType = RequestType::Build;
			break;
		}

		// 人間の家がある場合は強化依頼を追加
		for (auto build : buildlist)
		{
			if (!build->IsMaxBuildLevel())
			{
				newRequest.eRequestType = RequestType::Upgrade;
				newRequest.n2BuildIndex = build->GetFieldCellIndex();
				break;
			}
		}

		// 全ての人間の家が最大レベルの場合は建築依頼を追加
		if (newRequest.eRequestType != RequestType::Upgrade)
		{
			newRequest.eRequestType = RequestType::Build;
		}
	}
	break;
	case BuildType::BlackSmith:
	{
		// 今ある鍛冶屋を取得
		auto buildlist = pScene->GetGameObjects<CBlackSmith>();

		// 鍛冶屋が無ければ建築依頼を追加
		if (buildlist.empty())
		{
			newRequest.eRequestType = RequestType::Build;
			break;
		}

		// 鍛冶屋がある場合は強化依頼を追加
		for (auto build : buildlist)
		{
			if (!build->IsMaxBuildLevel())
			{
				newRequest.eRequestType = RequestType::Upgrade;
				newRequest.n2BuildIndex = build->GetFieldCellIndex();
				break;
			}
		}

		// 全ての鍛冶屋が最大レベルの場合は建築依頼を追加
		if (newRequest.eRequestType != RequestType::Upgrade)
		{
			newRequest.eRequestType = RequestType::Build;
		}
	}
	break;
	}
	newRequest.eBuildType = In_eRequestType;
	newRequest.eRequestState = RequestState::Unprocessed;

	if(newRequest.n2BuildIndex.x == -1 && newRequest.n2BuildIndex.y == -1)
	{
		// 建築可能な位置が無い場合は依頼を追加しない
		return;
	}

	// 建築依頼リストに追加
	m_BuildRequestList.push_back(newRequest);
}

/*****************************************//*
	@brief	| 建築依頼を受ける
	@return	| 建築依頼構造体のポインタ、無ければnullptr
*//*****************************************/
CBuildManager::BuildRequest* CBuildManager::TakeBuildRequest()
{
	// 未処理の依頼を探す
	for (auto& request : m_BuildRequestList)
	{
		// 未処理の依頼が見つかった場合は状態を処理中に変更して返す
		if (request.eRequestState == RequestState::Unprocessed)
		{
			// 状態を処理中に変更
			request.eRequestState = RequestState::InProcess;

			// 依頼構造体のポインタを返す
			return &request;
		}
	}
	return nullptr;
}

/*****************************************//*
	@brief	| 建築依頼を未処理状態に設定
	@param	| pRequest：建築依頼構造体のポインタ
*//*****************************************/
void CBuildManager::ResetBuildRequest(BuildRequest* pRequest)
{
	// ポインタが無効な場合は何もしない
	if (pRequest == nullptr)return;

	// 依頼状態を未処理に設定
	pRequest->eRequestState = RequestState::Unprocessed;
}

/*****************************************//*
	@brief	| 建築依頼を完了状態に設定
	@param	| pRequest：建築依頼構造体のポインタ
*//*****************************************/
void CBuildManager::CompleteBuildRequest(BuildRequest* pRequest)
{
	// ポインタが無効な場合は何もしない
	if (pRequest == nullptr)return;

	// 建築依頼リストから該当の依頼を削除
	m_BuildRequestList.remove_if([pRequest](const BuildRequest& request)
	{
		return &request == pRequest;
		});
}

/*****************************************//*
	@brief	| ランダムに建築位置を決定
	@return	| 建築位置のフィールドセルインデックス
*//*****************************************/
DirectX::XMINT2 CBuildManager::DecideRandomBuildPosition()
{
	// 建築可能なフィールドセルを取得
	auto cells = CFieldManager::GetInstance()->GetFieldGrid()->GetFieldCells(CFieldCell::CellType::Build, false);

	// 建築可能なセルが無い場合は無効なインデックスを返す
	if (cells.empty())return DirectX::XMINT2(-1, -1);

	// ランダムにセルを選択
	int randomIndex = rand() % cells.size();

	// 選択したセルのインデックスを返す
	return cells[randomIndex]->GetIndex();
}