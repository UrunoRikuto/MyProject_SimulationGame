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
#include "ImguiSystem.h"

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
	@brief　	| 所有者の設定
	@param　	| pOwner：所有者の参照
*//*****************************************/
void IJob_Strategy::SetOwner(CHuman& pOwner)
{
	m_pOwner = &pOwner;

	// 労働力のスキルバフ
	m_pOwner->GetSkill()->ApplySkillBuff(m_fWorkPower, this);
}

/*****************************************//*
	@brief　	| 休憩処理
*//*****************************************/
bool IJob_Strategy::RestAction()
{
	// スタミナが最大の場合は休憩完了
	if(m_pOwner->IsMaxStamina())
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

	// 休憩施設へ移動
	if (m_pOwner->MoveToTarget(m_UsingRefreshFacility, Human_Move_Speed))
	{

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
		m_pOwner->RecoverStamina(m_UsingRefreshFacility->GetRefreshStaminaAmount());

		// 休憩が完了したらtrueを返す
		if (m_pOwner->IsMaxStamina())
		{
			// 休憩施設の使用を解除
			m_UsingRefreshFacility->ReleaseRefreshFacility(*m_pOwner);
			// 休憩施設IDをリセット
			m_UsingRefreshFacility = nullptr;
			return true;
		}
	}

	// 休憩中
	return false;
}

/*****************************************//*
	@brief　	| 職業ステータスのImGui描画処理
*//*****************************************/
void IJob_Strategy::DrawJobStatusImGui()
{
	ImGui::Separator();
	ImGui::Text(u8"[職業ステータス]");

	// 労働力の描画
	ImGui::Text(u8"労働力: %.2f", m_fWorkPower);
}