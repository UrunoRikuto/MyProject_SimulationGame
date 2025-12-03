/**************************************************//*
	@file	| Job_Strategy.cpp
	@brief	| 職業クラスのcppファイル
	@note	| 職業インターフェースの処理を実装
			| ストラテジーパターンで作成
*//**************************************************/
#include "Job_Strategy.h"
#include "Main.h"
#include "Human.h"
#include "StructMath.h"
#include "Oparation.h"
#include "RefreshFacility.h"
#include "BuildManager.h"

/*****************************************//*
	@brief　	| デストラクタ
*//*****************************************/
IJob_Strategy::~IJob_Strategy()
{
	// 休憩施設を使用している場合
	if (m_UsingRefreshFacility != nullptr)
	{
		// 休憩施設の使用を解除
		m_UsingRefreshFacility->ReleaseRefreshFacility(*m_pOwner);
	}
}

/*****************************************//*
	@brief　	| スタミナの増加処理
	@param　	| fAmount：増加量
*//*****************************************/
void IJob_Strategy::ChangeStamina(float fAmount)
{
	// スタミナを増加
	m_Status.m_fStamina += fAmount;

	// スタミナが最大値を超えた場合は最大値に設定
	if (m_Status.m_fStamina > m_Status.m_fMaxStamina)
	{
		m_Status.m_fStamina = m_Status.m_fMaxStamina;
	}
}

/*****************************************//*
	@brief　	| 休憩処理
*//*****************************************/
bool IJob_Strategy::RestAction()
{
	// スタミナが最大の場合は休憩完了
	if(m_Status.m_fStamina >= m_Status.m_fMaxStamina)
	{
		return true;
	}

	// 除外する休憩施設IDリスト
	std::vector<ObjectID> vNotRefreshFacilityIDs;

	// 休憩施設をまだ使用していない場合
	while (m_UsingRefreshFacility == nullptr)
	{
		// 休憩施設を探すループ
		m_UsingRefreshFacility = GetScene()->GetGameObject<CRefreshFacility>(m_pOwner->GetPos(), vNotRefreshFacilityIDs);

		// 休憩施設が見つからなかった場合
		if (m_UsingRefreshFacility == nullptr)
		{
			// 建築管理システムに休憩施設の建築リクエストを送る
			CBuildManager::GetInstance()->AddBuildRequest(CBuildManager::BuildType::RefreshFacility);

			// 休憩施設が見つからなかった場合は処理を抜ける
			return false;
		}

		// 休憩施設が使用可能かどうか確認
		if (!m_UsingRefreshFacility->CanUseRefreshFacility())
		{
			// 使用不可の場合は、除外リストに追加して再度探す
			vNotRefreshFacilityIDs.push_back(m_UsingRefreshFacility->GetID());
			m_UsingRefreshFacility = nullptr;
		}
		else break;
	}

	if(m_UsingRefreshFacility == nullptr)
	{
		// 休憩施設が見つからなかった場合は休憩できないのでfalseを返す
		return false;
	}


	// 休憩所の位置を取得
	DirectX::XMFLOAT3 f3RefreshFacilityPos = m_UsingRefreshFacility->GetPos();

	// オーナーの位置を取得
	DirectX::XMFLOAT3 f3OwnerPos = m_pOwner->GetPos();

	// オブジェクトとオーナーの位置の距離を計算
	float fDistance = StructMath::Distance(f3OwnerPos, f3RefreshFacilityPos);

	// 休憩施設に近づく処理
	if (fDistance >= 1.0f)
	{
		DirectX::XMFLOAT3 f3Diff = f3RefreshFacilityPos - f3OwnerPos;
		// オーナーからオブジェクトへのベクトルを計算
		DirectX::XMVECTOR f3Direction = DirectX::XMLoadFloat3(&f3Diff);
		f3Direction = DirectX::XMVector3Normalize(f3Direction);
		// オーナーの位置をオブジェクトに向かって少しずつ移動させる
		DirectX::XMFLOAT3 f3Move;
		DirectX::XMStoreFloat3(&f3Move, f3Direction);
		f3OwnerPos += f3Move * Human_Move_Speed;
		// オーナーの位置を更新
		m_pOwner->SetPos(f3OwnerPos);
		return false;
	}

	// 休憩施設を使用していない場合の処理
	if (!m_UsingRefreshFacility->IsUsingRefreshFacility(*m_pOwner))
	{
		// 休憩施設が使用可能かどうか確認
		if (!m_UsingRefreshFacility->CanUseRefreshFacility())
		{
			// 建築管理システムに休憩施設の建築リクエストを送る
			CBuildManager::GetInstance()->AddBuildRequest(CBuildManager::BuildType::RefreshFacility);

			// 休憩施設が使用不可の場合は休憩できないのでfalseを返す
			return false;
		}

		// 休憩施設を使用
		m_UsingRefreshFacility->UseRefreshFacility(*m_pOwner);
	}

	// スタミナを回復
	m_Status.m_fStamina += m_UsingRefreshFacility->GetRefreshStaminaAmount();

	// 休憩が完了したらtrueを返す
	if (m_Status.m_fStamina >= m_Status.m_fMaxStamina)
	{
		m_Status.m_fStamina = m_Status.m_fMaxStamina;
		// 休憩施設の使用を解除
		m_UsingRefreshFacility->ReleaseRefreshFacility(*m_pOwner);
		// 休憩施設IDをリセット
		m_UsingRefreshFacility = nullptr;
		return true;
	}

	// 休憩中
	return false;
}
