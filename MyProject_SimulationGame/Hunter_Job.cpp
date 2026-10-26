/**************************************************//*
	@file	| Hunter_Job.cpp
	@brief	| ë—ÂE‹ÆƒNƒ‰ƒX‚Ìcppƒtƒ@ƒCƒ‹
	@note	| “®•¨‚ğë‚Á‚Ä“÷‚ğ“¾‚éE‹Æ‚Ìˆ—‚ğÀ‘•
			| CCollect_Strategy‚ğŒp³
*//**************************************************/
#include "Hunter_Job.h"
#include "Main.h"
#include "Human.h"
#include "Animal.h"
#include "HerbivorousAnimal.h"
#include "CarnivorousAnimal.h"
#include "StorageHouse.h"
#include "BlackSmith.h"
#include "BuildManager.h"
#include "CivLevelManager.h"
#include "StructMath.h"
#include "ImguiSystem.h"
#include <algorithm>
#include <cfloat>

namespace
{
	// @brief Šl•¨‚ğ’T‚·”ÍˆÍ
	constexpr float Hunt_Search_Radius = 80.0f;
	// @brief Šl•¨‚ÌüˆÍ‚ÌŠëŒ¯“x‚ğ‘ª‚é”ÍˆÍ
	constexpr float Hunt_Danger_Radius = 15.0f;
	// @brief Šl•¨‚ğ’Ç‚¤‚ÌˆÚ“®‘¬“x‚Ì”{—¦
	constexpr float Hunt_Chase_Speed_Rate = 2.5f;
	// @brief Šl•¨‚ğ’Ç‚¤‚ÌƒXƒ^ƒ~ƒiŒ¸­’l(–ˆƒtƒŒ[ƒ€)
	constexpr float Hunt_Chase_Stamina_Decrease = 0.05f;
	// @brief UŒ‚‚ÌŠÔŠu(•b)
	constexpr float Hunt_Attack_Interval = 0.6f;
	// @brief Šl•¨‚ğ’ú‚ß‚é‚Ü‚Å‚Ì’ÇÕŠÔ(•b)
	constexpr float Hunt_Chase_Limit_Time = 20.0f;
	// @brief Šl•¨’Tõ‚ÌŠÔŠu(•b)
	constexpr float Hunt_Search_Interval = 0.5f;
	// @brief Šl•¨1•C‚©‚ç“¾‚ç‚ê‚é“÷‚Ì”
	constexpr int Hunt_Meat_Drop_Count = 5;
	// @brief W’c‚Åë‚è‚ğ‚·‚él”‚ª1l‘‚¦‚é•¶–¾ƒŒƒxƒ‹‚ÌŠÔŠu
	constexpr int Hunt_Group_Level_Step = 4;
}

/*****************************************//*
	@brief@	| ƒRƒ“ƒXƒgƒ‰ƒNƒ^
*//*****************************************/
CHunter_Job::CHunter_Job()
	: CCollect_Strategy()
{
	// ë—ÂE‹Æ‚Ì˜J“­—Í‚ğİ’è
	m_fWorkPower = 10.0f;
}

/*****************************************//*
	@brief	| d–ˆ—
*//*****************************************/
void CHunter_Job::DoWork()
{
	switch (m_eCurrentState)
	{
		// Šl•¨‚ğ’T‚·
	case WorkState::SearchPrey:
		SearchPreyAction();
		break;

		// Šl•¨‚ğ’Ç‚Á‚ÄUŒ‚‚·‚é
	case WorkState::Hunting:
		HuntingAction();
		break;

		// “÷‚ğ‰^‚Ô
	case WorkState::Transporting:
		TransportingAction();
		break;

		// “÷‚ğ—a‚¯‚é
	case WorkState::Storing:
		StoringAction();
		break;

		// ‹xŒe’†
	case WorkState::Resting:
		RestingAction();
		break;
	}
}

/*****************************************//*
	@brief	| E‹ÆØ‚è‘Ö‚¦‚Ìˆ—
*//*****************************************/
void CHunter_Job::OnChangeJob()
{
	// Šl•¨‚Ìİ’è‚ğ‰ğœ
	ClearPrey();

	// d–ó‘Ô‚ğ‰Šú‰»
	m_eCurrentState = WorkState::SearchPrey;
	m_ePrevState = WorkState::SearchPrey;
	m_fAttackCooldown = 0.0f;
	m_fSearchTimer = 0.0f;
}

/*****************************************//*
	@brief	| ƒCƒ“ƒXƒyƒNƒ^[•\¦ˆ—
	@param	| isEndFƒCƒ“ƒXƒyƒNƒ^[I—¹ƒtƒ‰ƒO
	@return	| •\¦‚µ‚½€–Ú”
*//*****************************************/
int CHunter_Job::Inspecter(bool isEnd)
{
	int nItemCount = 0;

	ImGui::BeginChild("Hunter Job Inspector", ImVec2(0, 150), true);

	// E‹Æ–¼‚Ì•\¦
	ImGui::Text(u8"E‹Æ–¼:%s", GetJobName().c_str());

	// ƒXƒe[ƒ^ƒX‚Ì•\¦
	ImGui::Text(u8"˜J“­—Í:%.2f", m_fWorkPower);

	// ‘„‚ÌŠó‹µ‚Ì•\¦
	ImGui::Text(u8"‘„:%s", HasCollectTool() ? u8"‚ ‚è" : u8"‚È‚µ(‘fè)");

	// Œ»İ‚Ìd–ó‘Ô‚Ì•\¦
	ImGui::Text(u8"Œ»İ‚Ìó‘Ô:%s", GetStateName());

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
	@brief	| E‹ÆƒXƒe[ƒ^ƒX‚ÌImGui•`‰æˆ—
*//*****************************************/
void CHunter_Job::DrawJobStatusImGui()
{
	// E‹ÆƒXƒe[ƒ^ƒX‚ÌŠî–{•\¦
	IJob_Strategy::DrawJobStatusImGui();

	// Œ»İ‚Ìd–ó‘Ô‚Ì•\¦
	ImGui::Text(u8"ó‘Ô: %s", GetStateName());

	// Šl•¨‚Ì•\¦
	ImGui::Text(u8"Šl•¨‚ÌID: ");
	if (m_bHasPrey)
	{
		ImGui::Text("%s_%d", m_PreyID.m_sName.c_str(), m_PreyID.m_nSameCount);
	}
	else
	{
		ImGui::Text(u8"Šl•¨‚È‚µ");
	}
}

/*****************************************//*
	@brief	| ûWƒc[ƒ‹(‘„)‚ğ‚Á‚Ä‚¢‚é‚©‚Ç‚¤‚©‚ğæ“¾
	@return	| true:‚Á‚Ä‚¢‚é false:‚Á‚Ä‚¢‚È‚¢
*//*****************************************/
bool CHunter_Job::HasCollectTool()
{
	// ûWƒc[ƒ‹‚ğ‚Á‚Ä‚¢‚È‚¯‚ê‚Îfalse‚ğ•Ô‚·
	if (m_pOwner->GetToolItem() == nullptr) return false;

	// ûWƒc[ƒ‹‚ª‘„‚Å‚ ‚ê‚Îtrue‚ğ•Ô‚·
	return m_pOwner->GetToolItem()->GetItemType() == CItem::ITEM_TYPE::Spear;
}

/*****************************************//*
	@brief	| Šl•¨‚ğ’T‚·ˆ—
*//*****************************************/
void CHunter_Job::SearchPreyAction()
{
	// ‘„‚ğ‚Á‚Ä‚¢‚È‚¢ê‡‚Í—pˆÓ‚·‚é(‘„‚ğæ‚è‚És‚Á‚Ä‚¢‚éŠÔ‚ÍŠl•¨‚ğ’T‚³‚È‚¢)
	if (!HasCollectTool() && !PrepareTool()) return;

	// Šl•¨‚Ì’Tõ‚Í•‰‰×ŒyŒ¸‚Ì‚½‚ßˆê’èŠÔŠu‚Ås‚¤
	m_fSearchTimer -= fDeltaTime;
	if (m_fSearchTimer > 0.0f) return;
	m_fSearchTimer = Hunt_Search_Interval;

	// P‚¦‚éŠl•¨‚ğ’T‚·
	CAnimal* pPrey = SelectPrey();
	if (pPrey == nullptr) return;

	// Šl•¨‚ğİ’è‚µ‚Äë‚è‚ğŠJn‚·‚é
	SetPrey(pPrey);
	m_fChaseTime = 0.0f;
	ChangeState(WorkState::Hunting);
}

/*****************************************//*
	@brief	| Šl•¨‚ğ’Ç‚Á‚ÄUŒ‚‚·‚éˆ—
*//*****************************************/
void CHunter_Job::HuntingAction()
{
	// Šl•¨‚ğæ“¾(‘¼‚Ì“®•¨‚É“|‚³‚ê‚½ê‡‚Ínullptr)
	CAnimal* pPrey = ResolvePrey();
	if (pPrey == nullptr)
	{
		ClearPrey();
		ChangeState(WorkState::SearchPrey);
		return;
	}

	// UŒ‚‚ÌƒN[ƒ‹ƒ_ƒEƒ“‚ği‚ß‚é
	if (m_fAttackCooldown > 0.0f) m_fAttackCooldown -= fDeltaTime;

	// ’ÇÕŠÔ‚ª’·‚·‚¬‚éA‚Ü‚½‚Í‰“‚­‚Ö—£‚ê‚·‚¬‚½ê‡‚Í’ú‚ß‚é
	m_fChaseTime += fDeltaTime;
	const float fDistance = StructMath::Distance(m_pOwner->GetPos(), pPrey->GetPos());
	if (m_fChaseTime > Hunt_Chase_Limit_Time || fDistance > Hunt_Search_Radius * 1.5f)
	{
		ClearPrey();
		ChangeState(WorkState::SearchPrey);
		return;
	}

	// Šl•¨‚ÉŒü‚©‚Á‚Ä‹ì‚¯Šñ‚é
	if (!m_pOwner->MoveToTarget(pPrey, Human_Move_Speed * Hunt_Chase_Speed_Rate))
	{
		// ‘–‚Á‚Ä‚¢‚éŠÔ‚ÍƒXƒ^ƒ~ƒi‚ğÁ”ï‚·‚é
		m_pOwner->DecreaseStamina(Hunt_Chase_Stamina_Decrease);
		if (m_pOwner->IsZeroStamina())
		{
			ClearPrey();
			ChangeState(WorkState::Resting);
		}
		return;
	}

	// UŒ‚‚ÌŠÔŠu‚ª‹ó‚¢‚Ä‚¢‚È‚¢ê‡‚Í‘Ò‚Â
	if (m_fAttackCooldown > 0.0f) return;

	// UŒ‚‚·‚é(‘„‚ğ‚Á‚Ä‚¢‚ê‚Îƒ_ƒ[ƒW‚ª2”{)
	const float fDamage = HasCollectTool() ? m_fWorkPower * 2.0f : m_fWorkPower;
	pPrey->TakeDamage(fDamage);
	m_fAttackCooldown = Hunt_Attack_Interval;

	// ‹ó• “x‚ÆƒXƒ^ƒ~ƒi‚ğŒ¸­‚³‚¹‚é
	m_pOwner->DecreaseHunger(Work_Hunger_Decrease);
	m_pOwner->DecreaseStamina(Job_Work_Stamina_Decrease);

	// Šl•¨‚ğ“|‚µ‚½ê‡
	if (pPrey->IsDead())
	{
		// “÷‚ğè‚É“ü‚ê‚é
		for (int i = 0; i < Hunt_Meat_Drop_Count && m_pOwner->GetItemCount() < MaxHoldItem; ++i)
		{
			m_pOwner->HoldItem(new CItem(CItem::ITEM_TYPE::Meat));
		}

		// •¶–¾ŒoŒ±’l‚ğ‰ÁZ
		CCivLevelManager::GetInstance()->AddExp(CCivLevelManager::ExpType::Gathering);

		// Šl•¨‚Ìİ’è‚ğ‰ğœ‚µ‚Ä“÷‚ğ‰^‚Ô
		ClearPrey();
		ChangeState(WorkState::Transporting);
		return;
	}

	// ƒXƒ^ƒ~ƒi‚ª0‚É‚È‚Á‚½‚ç‹xŒe‚·‚é
	if (m_pOwner->IsZeroStamina())
	{
		ClearPrey();
		ChangeState(WorkState::Resting);
	}
}

/*****************************************//*
	@brief	| “÷‚ğ’™‘ ŒÉ‚É‰^‚Ôˆ—
*//*****************************************/
void CHunter_Job::TransportingAction()
{
	// ’™‘ ŒÉ‚ğ’T‚·
	CStorageHouse* pStorageHouse = GetScene()->GetGameObject<CStorageHouse>();

	// ’™‘ ŒÉ‚ªŒ©‚Â‚©‚ç‚È‚©‚Á‚½ê‡‚Íë‚è‚É–ß‚é
	if (pStorageHouse == nullptr)
	{
		ChangeState(m_pOwner->IsZeroStamina() ? WorkState::Resting : WorkState::SearchPrey);
		return;
	}

	// ’™‘ ŒÉ‚Ìˆê’è‹——£ˆÈ“à‚É“’B‚µ‚½‚ç—a‚¯‚éó‘Ô‚ÉˆÚs
	if (m_pOwner->MoveToTarget(pStorageHouse, Human_Move_Speed))
	{
		ChangeState(WorkState::Storing);
	}
}

/*****************************************//*
	@brief	| ’™‘ ŒÉ‚É“÷‚ğ—a‚¯‚éˆ—
*//*****************************************/
void CHunter_Job::StoringAction()
{
	// —a‚¯‚é•¨‚ª‚ ‚éê‡‚Í1‚Â‚¸‚Â—a‚¯‚é
	if (m_pOwner->HasItem())
	{
		CStorageHouse* pStorageHouse = GetScene()->GetGameObject<CStorageHouse>();

		// ’™‘ ŒÉ‚ªŒ©‚Â‚©‚ç‚È‚©‚Á‚½ê‡‚Í‰^‚Ñ’¼‚·
		if (pStorageHouse == nullptr)
		{
			ChangeState(WorkState::Transporting);
			return;
		}

		// ’™‘ ŒÉ‚Ìˆê’è‹——£ˆÈ“à‚É“’B‚µ‚Ä‚¢‚ê‚Î—a‚¯‚é
		if (m_pOwner->MoveToTarget(pStorageHouse, Human_Move_Speed))
		{
			pStorageHouse->StoreItem(m_pOwner->TakeOutItem());
		}
		return;
	}

	// —a‚¯I‚í‚Á‚½‚çAƒXƒ^ƒ~ƒi‚ª‚È‚¯‚ê‚Î‹xŒeA‚ ‚ê‚Îë‚è‚É–ß‚é
	ChangeState(m_pOwner->IsZeroStamina() ? WorkState::Resting : WorkState::SearchPrey);
}

/*****************************************//*
	@brief	| ‹xŒe’†‚Ìˆ—
*//*****************************************/
void CHunter_Job::RestingAction()
{
	// “÷‚ğ‚Á‚Ä‚¢‚éê‡‚Íæ‚É’™‘ ŒÉ‚Ö‰^‚Ô
	if (m_pOwner->HasItem())
	{
		ChangeState(WorkState::Transporting);
		return;
	}

	// ‹xŒe‚ªŠ®—¹‚µ‚½‚çë‚è‚É–ß‚é
	if (RestAction())
	{
		ChangeState(WorkState::SearchPrey);
	}
}

/*****************************************//*
	@brief	| ‘„‚ğ—pˆÓ‚·‚éˆ—
	@return	| true:‘„‚Ì—pˆÓ‚ğ‘Ò‚½‚¸‚ÉŠl•¨’Tõ‚Öi‚ß‚é false:‘„‚ğæ‚è‚ÉˆÚ“®’†
	@note	| ’™‘ ŒÉ‚É‘„‚ª‚ ‚ê‚Îæ‚è‚És‚«A–³‚¯‚ê‚Î’b–è‰®‚É¶Y‚ğˆË—Š‚·‚é
			| ‘„‚ª—pˆÓ‚Å‚«‚È‚¢ŠÔ‚Í‘fè‚Åë‚è‚ğs‚¤
*//*****************************************/
bool CHunter_Job::PrepareTool()
{
	CScene* pScene = GetScene();

	// ’™‘ ŒÉ‚É‘„‚ª‚ ‚éê‡‚Íæ‚è‚És‚­
	CStorageHouse* pStorageHouse = pScene->GetGameObject<CStorageHouse>();
	if (pStorageHouse != nullptr && pStorageHouse->HasItemType(GetRequiredCollectToolType()))
	{
		// ’™‘ ŒÉ‚É“’B‚·‚é‚Ü‚Å‚Í‘Ò‚Â
		if (!m_pOwner->MoveToTarget(pStorageHouse, Human_Move_Speed)) return false;

		// “’B‚µ‚½‚ç‘„‚ğó‚¯æ‚é
		m_pOwner->SetToolItem(pStorageHouse->TakeOutItem(GetRequiredCollectToolType()));
		return true;
	}

	// ’™‘ ŒÉ‚É‘„‚ª–³‚¢ê‡‚Í’b–è‰®‚É¶Y‚ğˆË—Š‚·‚é
	CBlackSmith* pBlacksmith = pScene->GetGameObject<CBlackSmith>();
	if (pBlacksmith != nullptr)
	{
		if (pBlacksmith->CanAddRequest())
		{
			// Šù‚É“¯‚¶ˆË—Š‚ªo‚Ä‚¢‚È‚¯‚ê‚ÎˆË—Š‚ğo‚·
			if (!pBlacksmith->HasRequest(GetRequiredCollectToolType()))
			{
				pBlacksmith->AddRequest(GetRequiredCollectToolType());
			}
		}
		// ˆË—Š‚ğo‚¹‚È‚¢ê‡‚Í’b–è‰®‚Ì‹­‰»‚ğŒš’zŠÇ—‚ÉˆË—Š‚·‚é
		else
		{
			CBuildManager::GetInstance()->AddBuildRequest(CBuildManager::BuildType::BlackSmith);
		}
	}

	// ‘„‚ª—pˆÓ‚Å‚«‚é‚Ü‚Å‚Í‘fè‚Åë‚è‚ğs‚¤
	return true;
}

/*****************************************//*
	@brief	| P‚¤Šl•¨‚ğ‘I‚Ôˆ—
	@return	| Šl•¨‚Ìƒ|ƒCƒ“ƒ^AP‚¦‚éŠl•¨‚ª‚¢‚È‚¢ê‡‚Ínullptr
	@note	| ’Tõ”ÍˆÍ“à‚Ì‘H“®•¨‚Ì‚¤‚¿AüˆÍ‚ÌŠëŒ¯“x‚ªW’c‚Ìí—ÍˆÈ‰º‚Ì‚à‚Ì‚ğ‘I‚ÑAÅ‚à‹ß‚¢Šl•¨‚ğ•Ô‚·
*//*****************************************/
CAnimal* CHunter_Job::SelectPrey()
{
	const DirectX::XMFLOAT3 f3MyPos = m_pOwner->GetPos();

	// W’c‚Åí‚¦‚éí—Í(©•ª‚ğŠÜ‚Ş)
	const float fGroupPower = CalcGroupPower();

	CAnimal* pBestPrey = nullptr;
	float fBestDistance = FLT_MAX;

	for (CHerbivorousAnimal* pCandidate : GetScene()->GetGameObjects<CHerbivorousAnimal>(f3MyPos, Hunt_Search_Radius))
	{
		// “|‚³‚ê‚Ä‚¢‚éŠl•¨‚Í‘ÎÛŠO
		if (pCandidate->IsDestroy() || pCandidate->IsDead()) continue;

		// Šl•¨‚ÌüˆÍ‚ÌŠëŒ¯“x‚ªW’c‚Ìí—Í‚ğã‰ñ‚éê‡‚ÍP‚í‚È‚¢
		if (CalcDanger(pCandidate) > fGroupPower) continue;

		// Å‚à‹ß‚¢Šl•¨‚ğ‘I‚Ô
		const float fDistance = StructMath::Distance(f3MyPos, pCandidate->GetPos());
		if (fDistance < fBestDistance)
		{
			fBestDistance = fDistance;
			pBestPrey = pCandidate;
		}
	}

	return pBestPrey;
}

/*****************************************//*
	@brief	| Šl•¨‚ÌüˆÍ‚ÌŠëŒ¯“x‚ğŒvZ‚·‚éˆ—
	@param	| pPreyFŠl•¨‚Ìƒ|ƒCƒ“ƒ^
	@return	| üˆÍ‚É‚¢‚é“÷H“®•¨‚ÌUŒ‚—Í‚Ì‡Œv
*//*****************************************/
float CHunter_Job::CalcDanger(CAnimal* pPrey) const
{
	float fDanger = 0.0f;

	// Šl•¨‚ÌüˆÍ‚É‚¢‚é“÷H“®•¨(ŒQ‚ê‚Ìê‡‚ÍŒQ‚ê‘S‘Ì)‚ÌUŒ‚—Í‚ğ‡Œv‚·‚é
	for (CCarnivorousAnimal* pCarnivore : GetScene()->GetGameObjects<CCarnivorousAnimal>(pPrey->GetPos(), Hunt_Danger_Radius))
	{
		if (pCarnivore->IsDestroy() || pCarnivore->IsDead()) continue;
		fDanger += pCarnivore->GetAttack();
	}

	return fDanger;
}

/*****************************************//*
	@brief	| W’c‚Åí‚¦‚éí—Í‚ğŒvZ‚·‚éˆ—
	@return	| ë—ÂE‚Ì•½‹Ï˜J“­—Í ~ W’c‚Ìl”
	@note	| W’c‚Ìl”‚Íë—ÂE‚Ìl”‚ÆA•¶–¾ƒŒƒxƒ‹‚É‰‚¶‚½ãŒÀ‚Ì¬‚³‚¢•û
*//*****************************************/
float CHunter_Job::CalcGroupPower() const
{
	float fTotalPower = 0.0f;
	int nHunterCount = 0;

	// ƒV[ƒ““à‚Ìë—ÂE‚ÌlŠÔ‚ğ”‚¦‚é
	for (CHuman* pHuman : GetScene()->GetGameObjects<CHuman>())
	{
		if (pHuman->IsDestroy()) continue;

		CHunter_Job* pJob = dynamic_cast<CHunter_Job*>(pHuman->GetHumanJob());
		if (pJob == nullptr) continue;

		fTotalPower += pJob->GetWorkPower();
		nHunterCount++;
	}

	// ë—ÂE‚ªŒ©‚Â‚©‚ç‚È‚©‚Á‚½ê‡‚Í©•ª1l•ª‚Ìí—Í
	if (nHunterCount == 0) return m_fWorkPower;

	// •¶–¾ƒŒƒxƒ‹‚ªã‚ª‚é‚Ù‚Ç‘å‚«‚ÈW’c‚Ås“®‚Å‚«‚é
	const int nGroupLimit = 1 + CCivLevelManager::GetInstance()->GetCivLevel() / Hunt_Group_Level_Step;
	const int nGroupSize = (nHunterCount < nGroupLimit) ? nHunterCount : nGroupLimit;

	return (fTotalPower / static_cast<float>(nHunterCount)) * static_cast<float>(nGroupSize);
}

/*****************************************//*
	@brief	| Œ»İ‚ÌŠl•¨‚ğæ“¾‚·‚éˆ—
	@return	| Šl•¨‚Ìƒ|ƒCƒ“ƒ^A“|‚³‚ê‚½/‚¢‚È‚­‚È‚Á‚½ê‡‚Ínullptr
*//*****************************************/
CAnimal* CHunter_Job::ResolvePrey()
{
	if (!m_bHasPrey) return nullptr;

	// Šl•¨‚Í‘¼‚Ì“®•¨‚É“|‚³‚ê‚Ä”jŠü‚³‚ê‚é‚±‚Æ‚ª‚ ‚é‚½‚ßAID‚©‚ç–ˆ‰ñ’T‚µ’¼‚·
	CAnimal* pPrey = GetScene()->FindGameObjectByID<CAnimal>(m_PreyID);
	if (pPrey == nullptr || pPrey->IsDestroy() || pPrey->IsDead()) return nullptr;

	return pPrey;
}

/*****************************************//*
	@brief	| Šl•¨‚ğİ’è‚·‚éˆ—
	@param	| pPreyFŠl•¨‚Ìƒ|ƒCƒ“ƒ^
*//*****************************************/
void CHunter_Job::SetPrey(CAnimal* pPrey)
{
	m_PreyID = pPrey->GetID();
	m_bHasPrey = true;
}

/*****************************************//*
	@brief	| Šl•¨‚Ìİ’è‚ğ‰ğœ‚·‚éˆ—
*//*****************************************/
void CHunter_Job::ClearPrey()
{
	m_PreyID = ObjectID{ "", -1 };
	m_bHasPrey = false;
	m_fChaseTime = 0.0f;
}

/*****************************************//*
	@brief	| Œ»İ‚Ìd–ó‘Ô–¼‚ğæ“¾‚·‚éˆ—
	@return	| d–ó‘Ô–¼‚Ì•¶š—ñ
*//*****************************************/
const char* CHunter_Job::GetStateName() const
{
	switch (m_eCurrentState)
	{
	case WorkState::SearchPrey:		return u8"Šl•¨‚ğ’Tõ’†";
	case WorkState::Hunting:		return u8"ë—Â’†";
	case WorkState::Transporting:	return u8"“÷‚ğ’™‘ ŒÉ‚É‰^”À’†";
	case WorkState::Storing:		return u8"’™‘ ŒÉ‚É—a‚¯’†";
	case WorkState::Resting:		return u8"‹xŒe’†";
	default:						return "Unknown";
	}
}
