/**************************************************//*
	@file	| Wolf_Animal.cpp
	@brief	| オオカミクラスの定義
	@note	| オオカミの処理を定義
*//**************************************************/
#include "Wolf_Animal.h"
#include "FlockAttackAI.h"
#include "Oparation.h"
#include "Main.h"
#include "HerbivorousAnimal.h"
#include "GameTimeManager.h"
#include <algorithm>

/****************************************//*
	@brief	| コンストラクタ
*//****************************************/
CWolf_Animal::CWolf_Animal()
	: CCarnivorousAnimal()
{
	BoidsParams params;

	// --- 視野半径と分離半径 ---
	params.fViewRadius =12.0f;
	params.fSeparationRadius =6.0f;
	// --- 各行動の重み係数 ---
	params.fWeightSeparation =1.5f;
	params.fWeightAlignment =0.5f;
	params.fWeightCohesion =0.5f;
	// ---速度と力の最大値 ---
	params.fMaxSpeed =6.0f;
	params.fMaxForce =3.0f;
	// --- 各行動の最大力 ---
	params.fMaxSeparationForce =3.0f;
	params.fMaxAlignmentForce =3.0f;
	params.fMaxCohesionForce =3.0f;

	m_pActionAI = new(std::nothrow) CFlockAttackAI(params);

	m_fAttack =15.0f;
	m_fAttackInterval =0.8f;
}

/****************************************//*
	@brief	| デストラクタ
*//****************************************/
CWolf_Animal::~CWolf_Animal() = default;

/****************************************//*
	@brief	| 初期化処理
*//****************************************/
void CWolf_Animal::Init()
{
	CCarnivorousAnimal::Init();

	CModelRenderer* pModelRenderer = GetComponent<CModelRenderer>();
	pModelRenderer->SetKey("Wolf");
	pModelRenderer->SetRendererParam(m_tParam);
}

/****************************************//*
	@brief	| 更新処理
*//****************************************/
void CWolf_Animal::Update()
{
	CCarnivorousAnimal::Update();

	// 攻撃クールダウン更新
	if (m_fAttackCooldown >0.0f)
	{
		m_fAttackCooldown -= fDeltaTime;
		if (m_fAttackCooldown <0.0f) m_fAttackCooldown =0.0f;
	}

	// 時間帯によって狩りをする/しない
	switch (CGameTimeManager::GetInstance()->GetCurrentDayTime())
	{
	case CGameTimeManager::DAY_TIME::EVENING:
	case CGameTimeManager::DAY_TIME::NIGHT:
		SetTarget();
		break;
	default:
		if (auto* pAI = dynamic_cast<CFlockAttackAI*>(m_pActionAI))
		{
			pAI->ClearTarget();
		}
		break;
	}

	DirectX::XMFLOAT3 pos = { m_tParam.m_f3Pos.x, m_tParam.m_f3Pos.y, m_tParam.m_f3Pos.z };
	DirectX::XMFLOAT3 vel = { m_f3Velocity.x, m_f3Velocity.y, m_f3Velocity.z };

	DirectX::XMFLOAT3 steer = m_pActionAI->UpdateAI(pos, vel, m_SameAnimalNeighbors);
	vel += steer * fDeltaTime;
	vel = vel *0.98f;

	// ------------------------------------------------------------
	// スタミナ参照で最大移動速度を制御（狼）
	//追跡している標的のスタミナが一定以下なら加速（追い込み）
	// ------------------------------------------------------------
	const float staminaMax = (GetMaxStamina() >0.0f) ? GetMaxStamina() :1.0f;
	float staminaRate = GetStamina() / staminaMax;
	staminaRate = std::clamp(staminaRate,0.0f,1.0f);

	const float baseMaxSpeed =6.0f;
	const float minMaxSpeed =3.0f;

	float targetExhaustBoost =1.0f;
	const float targetStaminaThreshold =25.0f;

	// 標的との距離が離れるほど少し加速（追跡の伸びを作る）
	float distanceBoost =1.0f;
	const float distBoostStart =6.0f;
	const float distBoostEnd =22.0f;
	const float distBoostMax =1.12f;

	CHerbivorousAnimal* pTarget = nullptr;
	bool hasTarget = false;

	if (auto* pFlockAI = dynamic_cast<CFlockAttackAI*>(m_pActionAI))
	{
		hasTarget = pFlockAI->HasTarget();
		if (hasTarget)
		{
			DirectX::XMFLOAT3 targetPos = pFlockAI->GetTargetPosition();
			pTarget = GetScene()->GetGameObject<CHerbivorousAnimal>(targetPos);
			if (pTarget)
			{
				// 標的が疲れているときの追い込み加速
				if (pTarget->GetStamina() <= targetStaminaThreshold)
				{
					targetExhaustBoost =1.0f +0.35f * staminaRate;
				}

				// 距離が離れているほど少しだけ上限速度を上げる
				const float dist = StructMath::Distance(GetPos(), pTarget->GetPos());
				if (dist > distBoostStart)
				{
					float t = (dist - distBoostStart) / (distBoostEnd - distBoostStart);
					t = std::clamp(t,0.0f,1.0f);
					distanceBoost =1.0f + (distBoostMax -1.0f) * t;
				}
			}
		}
	}

	float maxSpeed = (minMaxSpeed + (baseMaxSpeed - minMaxSpeed) * staminaRate) * targetExhaustBoost * distanceBoost;

	float speed = StructMath::Length(vel);
	if (speed > maxSpeed && speed >0.0001f)
	{
		vel = vel * (maxSpeed / speed);
		speed = maxSpeed;
	}

	// ------------------------------------------------------------
	// 一定距離になったら攻撃（一定間隔）
	// ------------------------------------------------------------
	if (hasTarget && pTarget != nullptr)
	{
		const float attackRange =1.2f;
		const float dist = StructMath::Distance(GetPos(), pTarget->GetPos());
		const float attackDamage = (GetAttack() >0.0f) ? GetAttack() :15.0f;
		const float attackInterval = (m_fAttackInterval >0.0f) ? m_fAttackInterval :0.8f;

		if (dist <= attackRange && m_fAttackCooldown <=0.0f)
		{
			pTarget->TakeDamage(attackDamage);
			m_fAttackCooldown = attackInterval;
		}
	}

	// ------------------------------------------------------------
	// スタミナ消費/回復
	// ------------------------------------------------------------
	const float speed01 = (baseMaxSpeed >0.0f) ? (speed / baseMaxSpeed) :0.0f;
	const float moveEps =0.15f;
	if (speed > moveEps)
	{
		const float baseConsume =3.0f;
		const float boostConsume = (targetExhaustBoost >1.01f) ?1.5f :1.0f;
		DecreaseStamina(baseConsume * boostConsume * speed01 * fDeltaTime);
	}
	else
	{
		RecoverStamina(5.0f * fDeltaTime);
	}

	m_f3Velocity = { vel.x, vel.y, vel.z };
	m_tParam.m_f3Rotate.y = atan2f(m_f3Velocity.x, m_f3Velocity.z);
	m_tParam.m_f3Pos += m_f3Velocity * fDeltaTime;
}

/****************************************//*
	@brief	| ターゲット設定
*//****************************************/
void CWolf_Animal::SetTarget()
{
	auto* pFlockAI = dynamic_cast<CFlockAttackAI*>(m_pActionAI);
	if (!pFlockAI) return;

	// 群れの誰かがターゲットを持っていたら共有
	for (const BoidsNeighbor& Animal : m_SameAnimalNeighbors)
	{
		if (!Animal.bSetTarget) continue;
		pFlockAI->SetTargetPosition(Animal.pTargetPos);
		return;
	}

	CHerbivorousAnimal* pTargetAnimal = GetScene()->GetGameObject<CHerbivorousAnimal>(m_tParam.m_f3Pos);
	if (!pTargetAnimal) return;

	pFlockAI->SetTargetPosition(pTargetAnimal->GetPos());
}

/****************************************//*
	@brief	| 群れ登録
*//****************************************/
void CWolf_Animal::RegisterToFlock(std::vector<CWolf_Animal*> In_Wolfs)
{
	m_SameAnimalNeighbors.clear();

	for (CWolf_Animal* wolf : In_Wolfs)
	{
		if (wolf == this) continue;

		float dist = StructMath::Distance(GetPos(), wolf->GetPos());
		if (dist >20.0f) continue;

		auto* pFlockAI = dynamic_cast<CFlockAttackAI*>(wolf->m_pActionAI);

		BoidsNeighbor neighbor;
		neighbor.v3Position = { wolf->m_tParam.m_f3Pos.x, wolf->m_tParam.m_f3Pos.y, wolf->m_tParam.m_f3Pos.z };
		neighbor.v3Velocity = { wolf->m_f3Velocity.x, wolf->m_f3Velocity.y, wolf->m_f3Velocity.z };
		neighbor.bSetTarget = (pFlockAI != nullptr) ? pFlockAI->HasTarget() : false;
		neighbor.pTargetPos = (pFlockAI != nullptr) ? pFlockAI->GetTargetPosition() : DirectX::XMFLOAT3(0,0,0);

		m_SameAnimalNeighbors.push_back(neighbor);
	}

	if (auto* pFlockAI = dynamic_cast<CFlockAttackAI*>(m_pActionAI))
	{
		pFlockAI->SetHomePosition(GetPos());
	}
}