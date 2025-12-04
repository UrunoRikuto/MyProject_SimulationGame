/**************************************************//*
	@file	| Job_Strategy.cpp
	@brief	| E‹ÆƒNƒ‰ƒX‚Ìcppƒtƒ@ƒCƒ‹
	@note	| E‹ÆƒCƒ“ƒ^[ƒtƒF[ƒX‚Ìˆ—‚ğÀ‘•
			| ƒXƒgƒ‰ƒeƒW[ƒpƒ^[ƒ“‚Åì¬
*//**************************************************/
#include "Job_Strategy.h"
#include "Main.h"
#include "Human.h"
#include "StructMath.h"
#include "Oparation.h"
#include "RefreshFacility.h"
#include "BuildManager.h"

/*****************************************//*
	@brief@	| ƒfƒXƒgƒ‰ƒNƒ^
*//*****************************************/
IJob_Strategy::~IJob_Strategy()
{
	// ‹xŒe{İ‚ğg—p‚µ‚Ä‚¢‚éê‡
	if (m_UsingRefreshFacility != nullptr)
	{
		// ‹xŒe{İ‚Ìg—p‚ğ‰ğœ
		m_UsingRefreshFacility->ReleaseRefreshFacility(*m_pOwner);
	}
}

/*****************************************//*
	@brief@	| ƒXƒ^ƒ~ƒi‚Ì‘‰Áˆ—
	@param@	| fAmountF‘‰Á—Ê
*//*****************************************/
void IJob_Strategy::ChangeStamina(float fAmount)
{
	// ƒXƒ^ƒ~ƒi‚ğ‘‰Á
	m_Status.m_fStamina += fAmount;

	// ƒXƒ^ƒ~ƒi‚ªÅ‘å’l‚ğ’´‚¦‚½ê‡‚ÍÅ‘å’l‚Éİ’è
	if (m_Status.m_fStamina > m_Status.m_fMaxStamina)
	{
		m_Status.m_fStamina = m_Status.m_fMaxStamina;
	}
}

/*****************************************//*
	@brief@	| ‹xŒeˆ—
*//*****************************************/
bool IJob_Strategy::RestAction()
{
	// ƒXƒ^ƒ~ƒi‚ªÅ‘å‚Ìê‡‚Í‹xŒeŠ®—¹
	if(m_Status.m_fStamina >= m_Status.m_fMaxStamina)
	{
		return true;
	}

	// œŠO‚·‚é‹xŒe{İIDƒŠƒXƒg
	std::vector<ObjectID> vNotRefreshFacilityIDs;

	// ‹xŒe{İ‚ğ‚Ü‚¾g—p‚µ‚Ä‚¢‚È‚¢ê‡
	while (m_UsingRefreshFacility == nullptr)
	{
		// ‹xŒe{İ‚ğ’T‚·ƒ‹[ƒv
		m_UsingRefreshFacility = GetScene()->GetGameObject<CRefreshFacility>(m_pOwner->GetPos(), vNotRefreshFacilityIDs);

		// ‹xŒe{İ‚ªŒ©‚Â‚©‚ç‚È‚©‚Á‚½ê‡
		if (m_UsingRefreshFacility == nullptr)
		{
			// Œš’zŠÇ—ƒVƒXƒeƒ€‚É‹xŒe{İ‚ÌŒš’zƒŠƒNƒGƒXƒg‚ğ‘—‚é
			CBuildManager::GetInstance()->AddBuildRequest(CBuildManager::BuildType::RefreshFacility);

			// ‹xŒe{İ‚ªŒ©‚Â‚©‚ç‚È‚©‚Á‚½ê‡‚Íˆ—‚ğ”²‚¯‚é
			return false;
		}

		// ‹xŒe{İ‚ªg—p‰Â”\‚©‚Ç‚¤‚©Šm”F
		if (!m_UsingRefreshFacility->CanUseRefreshFacility())
		{
			// g—p•s‰Â‚Ìê‡‚ÍAœŠOƒŠƒXƒg‚É’Ç‰Á‚µ‚ÄÄ“x’T‚·
			vNotRefreshFacilityIDs.push_back(m_UsingRefreshFacility->GetID());
			m_UsingRefreshFacility = nullptr;
		}
		else break;
	}

	if(m_UsingRefreshFacility == nullptr)
	{
		// ‹xŒe{İ‚ªŒ©‚Â‚©‚ç‚È‚©‚Á‚½ê‡‚Í‹xŒe‚Å‚«‚È‚¢‚Ì‚Åfalse‚ğ•Ô‚·
		return false;
	}

	// ‹xŒe{İ‚ÖˆÚ“®
	if (m_pOwner->MoveToTarget(m_UsingRefreshFacility, Human_Move_Speed))
	{

		// ‹xŒe{İ‚ğg—p‚µ‚Ä‚¢‚È‚¢ê‡‚Ìˆ—
		if (!m_UsingRefreshFacility->IsUsingRefreshFacility(*m_pOwner))
		{
			// ‹xŒe{İ‚ªg—p‰Â”\‚©‚Ç‚¤‚©Šm”F
			if (!m_UsingRefreshFacility->CanUseRefreshFacility())
			{
				// Œš’zŠÇ—ƒVƒXƒeƒ€‚É‹xŒe{İ‚ÌŒš’zƒŠƒNƒGƒXƒg‚ğ‘—‚é
				CBuildManager::GetInstance()->AddBuildRequest(CBuildManager::BuildType::RefreshFacility);

				// ‹xŒe{İ‚ªg—p•s‰Â‚Ìê‡‚Í‹xŒe‚Å‚«‚È‚¢‚Ì‚Åfalse‚ğ•Ô‚·
				return false;
			}

			// ‹xŒe{İ‚ğg—p
			m_UsingRefreshFacility->UseRefreshFacility(*m_pOwner);
		}

		// ƒXƒ^ƒ~ƒi‚ğ‰ñ•œ
		m_Status.m_fStamina += m_UsingRefreshFacility->GetRefreshStaminaAmount();

		// ‹xŒe‚ªŠ®—¹‚µ‚½‚çtrue‚ğ•Ô‚·
		if (m_Status.m_fStamina >= m_Status.m_fMaxStamina)
		{
			m_Status.m_fStamina = m_Status.m_fMaxStamina;
			// ‹xŒe{İ‚Ìg—p‚ğ‰ğœ
			m_UsingRefreshFacility->ReleaseRefreshFacility(*m_pOwner);
			// ‹xŒe{İID‚ğƒŠƒZƒbƒg
			m_UsingRefreshFacility = nullptr;
			return true;
		}
	}

	// ‹xŒe’†
	return false;
}
