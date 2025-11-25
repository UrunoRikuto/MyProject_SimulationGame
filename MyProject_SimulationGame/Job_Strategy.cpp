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
	@brief　	| 休憩処理
*//*****************************************/
bool IJob_Strategy::RestAction()
{
	std::vector<ObjectID> vNotRefreshFacilityIDs;

	CRefreshFacility* pRefreshFacility = nullptr;
	while (1)
	{
		// 休憩施設を探すループ
		pRefreshFacility = GetScene()->GetGameObject<CRefreshFacility>(m_pOwner->GetPos(), vNotRefreshFacilityIDs);

		// 休憩施設が見つからなかった場合
		if (pRefreshFacility == nullptr)
		{
			// 建築管理システムに休憩施設の建築リクエストを送る
			CBuildManager::GetInstance()->AddBuildRequest(CBuildManager::BuildType::RefreshFacility);

			// 休憩施設が見つからなかった場合は処理を抜ける
			return false;
		}

		// 休憩施設が使用可能かどうか確認
		if (!pRefreshFacility->CanUseRefreshFacility())
		{
			// 使用不可の場合は、除外リストに追加して再度探す
			vNotRefreshFacilityIDs.push_back(pRefreshFacility->GetID());
		}
		else
		{
			// 使用可能な休憩施設が見つかった場合はループを抜ける
			break;
		}
	}

	// 休憩所の位置を取得
	DirectX::XMFLOAT3 f3RefreshFacilityPos = pRefreshFacility->GetPos();

	// オーナーの位置を取得
	DirectX::XMFLOAT3 f3OwnerPos = m_pOwner->GetPos();

	// オブジェクトとオーナーの位置の距離を計算
	float fDistance = StructMath::Distance(f3OwnerPos, f3RefreshFacilityPos);

	// 一定距離以内に達したら休憩する
	if (fDistance >= 1.0f)
	{
		DirectX::XMFLOAT3 f3Diff = f3RefreshFacilityPos - f3OwnerPos;
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
		return false;
	}

	// 休憩施設を使用していない場合の処理
	if (!pRefreshFacility->IsUsingRefreshFacility(*m_pOwner))
	{
		// 休憩施設が使用可能かどうか確認
		if (!pRefreshFacility->CanUseRefreshFacility())
		{
			// 建築管理システムに休憩施設の建築リクエストを送る
			CBuildManager::GetInstance()->AddBuildRequest(CBuildManager::BuildType::RefreshFacility);

			// 休憩施設が使用不可の場合は休憩できないのでfalseを返す
			return false;
		}

		// 休憩施設を使用
		pRefreshFacility->UseRefreshFacility(*m_pOwner);
	}

	// スタミナを回復
	m_Status.m_fStamina += pRefreshFacility->GetRefreshStaminaAmount();

	// 休憩が完了したらtrueを返す
	if (m_Status.m_fStamina >= m_Status.m_fMaxStamina)
	{
		m_Status.m_fStamina = m_Status.m_fMaxStamina;
		// 休憩施設の使用を解除
		pRefreshFacility->ReleaseRefreshFacility(*m_pOwner);
		return true;
	}

	// 休憩中
	return false;
}
