/**************************************************//*
	@file	| Wolf_Animal.cpp
	@brief	| 狼クラス実装
	@note	| 狼の振る舞いを定義
			| CCarnivorousAnimalを継承
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

	// 集団攻撃AI生成
	m_pActionAI = new(std::nothrow) CFlockAttackAI(params);

	// 攻撃力・攻撃間隔設定
	m_fAttack =15.0f;
	m_fAttackInterval =0.8f;
}

/****************************************//*
	@brief	| デストラクタ
*//****************************************/
CWolf_Animal::~CWolf_Animal()
{
}

/****************************************//*
	@brief	| 初期化処理
*//****************************************/
void CWolf_Animal::Init()
{
	// 親クラス初期化
	CCarnivorousAnimal::Init();

	// モデルレンダラー設定
	CModelRenderer* pModelRenderer = GetComponent<CModelRenderer>();
	pModelRenderer->SetKey("Wolf");
	pModelRenderer->SetRendererParam(m_tParam);
}

/****************************************//*
	@brief	| 更新処理
*//****************************************/
void CWolf_Animal::Update()
{
	// 親クラス更新
	CCarnivorousAnimal::Update();

	// 攻撃クールダウン更新
	if (m_fAttackCooldown >0.0f)
	{
		m_fAttackCooldown -= fDeltaTime;
		if (m_fAttackCooldown < 0.0f) m_fAttackCooldown = 0.0f;
	}

	// 時間帯による標的設定/解除
	switch (CGameTimeManager::GetInstance()->GetCurrentDayTime())
	{
	case CGameTimeManager::DAY_TIME::EVENING:
	case CGameTimeManager::DAY_TIME::NIGHT:
		// 夕方～夜は標的設定
		SetTarget();
		break;
	default:
		// その他は標的解除
		if (auto* pAI = dynamic_cast<CFlockAttackAI*>(m_pActionAI))
		{
			pAI->ClearTarget();
		}
		break;
	}

	// 位置・速度取得
	DirectX::XMFLOAT3 pos = m_tParam.m_f3Pos;
	DirectX::XMFLOAT3 vel = m_f3Velocity;

	// 行動AI更新
	DirectX::XMFLOAT3 steer = m_pActionAI->UpdateAI(pos, vel, m_SameAnimalNeighbors);
	// 速度更新
	vel += steer * fDeltaTime;
	// 減速（空気抵抗）
	vel = vel * 0.98f;

	// スタミナによる速度制限
	const float staminaMax = (GetMaxStamina() >0.0f) ? GetMaxStamina() :1.0f;
	// 0.0～1.0のスタミナ率
	float staminaRate = GetStamina() / staminaMax;
	// クランプ
	staminaRate = std::clamp(staminaRate,0.0f,1.0f);

	// 最大速度計算
	const float baseMaxSpeed = 6.0f;
	// 最低最大速度
	const float minMaxSpeed = 3.0f;

	// 標的が疲れているときの追い込み加速
	float targetExhaustBoost = 1.0f;
	// 標的のスタミナ閾値
	const float targetStaminaThreshold = 25.0f;

	// 距離による速度ブースト
	float distanceBoost =1.0f;

	// 距離ブーストパラメータ
	// 開始距離
	const float distBoostStart = 6.0f;
	// 終了距離
	const float distBoostEnd = 22.0f;
	// 最大ブースト倍率
	const float distBoostMax = 1.12f;

	// 標的取得
	CHerbivorousAnimal* pTarget = nullptr;
	// 標的がいるかどうか
	bool hasTarget = false;
	if (auto* pFlockAI = dynamic_cast<CFlockAttackAI*>(m_pActionAI))
	{
		// 標的がいるか確認
		hasTarget = pFlockAI->HasTarget();
		// 標的がいる場合
		if (hasTarget)
		{
			// 標的の位置取得
			DirectX::XMFLOAT3 targetPos = pFlockAI->GetTargetPosition();
			// 標的の動物取得
			pTarget = GetScene()->GetGameObject<CHerbivorousAnimal>(targetPos);

			// 標的が存在する場合
			if (pTarget)
			{
				// 標的のスタミナが閾値以下の場合は追い込み加速
				if (pTarget->GetStamina() <= targetStaminaThreshold)
				{
					// 標的のスタミナ率
					targetExhaustBoost =1.0f + 0.35f * staminaRate;
				}

				// 距離による速度ブースト計算
				const float dist = StructMath::Distance(GetPos(), pTarget->GetPos());
				// 距離が開始距離より遠い場合はブースト
				if (dist > distBoostStart)
				{
					float t = (dist - distBoostStart) / (distBoostEnd - distBoostStart);
					t = std::clamp(t,0.0f,1.0f);
					distanceBoost =1.0f + (distBoostMax -1.0f) * t;
				}
			}
		}
	}

	// 最終最大速度計算
	float maxSpeed = (minMaxSpeed + (baseMaxSpeed - minMaxSpeed) * staminaRate) * targetExhaustBoost * distanceBoost;

	// 速度制限
	float speed = StructMath::Length(vel);
	// 最大速度を超えていたら制限
	if (speed > maxSpeed && speed >0.0001f)
	{
		// 正規化して最大速度に調整
		vel = vel * (maxSpeed / speed);
		speed = maxSpeed;
	}

	// 標的がいる場合は攻撃判定
	if (hasTarget && pTarget != nullptr)
	{
		// 攻撃判定
		const float attackRange =1.2f;
		// 距離計算
		const float dist = StructMath::Distance(GetPos(), pTarget->GetPos());
		// 攻撃処理
		const float attackDamage = (GetAttack() >0.0f) ? GetAttack() :15.0f;
		// 攻撃間隔
		const float attackInterval = (m_fAttackInterval >0.0f) ? m_fAttackInterval :0.8f;

		// 攻撃範囲内かつクールダウン完了していたら攻撃
		if (dist <= attackRange && m_fAttackCooldown <=0.0f)
		{
			// ダメージ与える
			pTarget->TakeDamage(attackDamage);
			// 攻撃クールダウンをセット
			m_fAttackCooldown = attackInterval;
		}
	}

	// スタミナ消費/回復処理
	const float speed01 = (baseMaxSpeed >0.0f) ? (speed / baseMaxSpeed) :0.0f;
	const float moveEps =0.15f;

	// 移動中は消費
	if (speed > moveEps)
	{
		// 基本消費量
		const float baseConsume = 3.0f;
		// 疲れた標的への追い込み消費増加
		const float boostConsume = (targetExhaustBoost > 1.01f) ? 1.5f :1.0f;
		// 消費実行
		DecreaseStamina(baseConsume * boostConsume * speed01 * fDeltaTime);
	}
	else
	{
		// 停止中は回復
		RecoverStamina(5.0f * fDeltaTime);
	}

	// 速度保存
	m_f3Velocity = { vel.x, vel.y, vel.z };
	// 向き更新（XZ平面）
	m_tParam.m_f3Rotate.y = atan2f(m_f3Velocity.x, m_f3Velocity.z);
	// 位置更新
	m_tParam.m_f3Pos += m_f3Velocity * fDeltaTime;
}

/****************************************//*
	@brief	| ターゲット設定
*//****************************************/
void CWolf_Animal::SetTarget()
{
	// 攻撃AI取得
	auto* pFlockAI = dynamic_cast<CFlockAttackAI*>(m_pActionAI);
	// 攻撃AI無ければ終了
	if (!pFlockAI) return;

	// 群れの誰かがターゲットを持っていたら共有
	for (const BoidsNeighbor& Animal : m_SameAnimalNeighbors)
	{
		// ターゲットが設定されていなければスキップ
		if (!Animal.bSetTarget) continue;
		// ターゲット位置共有
		pFlockAI->SetTargetPosition(Animal.pTargetPos);
		return;
	}

	// 自身のターゲット設定
	CHerbivorousAnimal* pTargetAnimal = GetScene()->GetGameObject<CHerbivorousAnimal>(m_tParam.m_f3Pos);
	// ターゲットが存在しなければ終了
	if (!pTargetAnimal) return;

	// ターゲット位置設定
	pFlockAI->SetTargetPosition(pTargetAnimal->GetPos());
}

/****************************************//*
	@brief	| 群れ登録
*//****************************************/
void CWolf_Animal::RegisterToFlock(std::vector<CWolf_Animal*> In_Wolfs)
{
	// 同種動物リストクリア
	m_SameAnimalNeighbors.clear();

	// 近くの同種動物を登録
	for (CWolf_Animal* wolf : In_Wolfs)
	{
		// 自分自身はスキップ
		if (wolf == this) continue;

		// 一定距離以上離れていたらスキップ
		float dist = StructMath::Distance(GetPos(), wolf->GetPos());
		if (dist >20.0f) continue;

		// 群れの動物の行動AI取得
		auto* pFlockAI = dynamic_cast<CFlockAttackAI*>(wolf->m_pActionAI);

		// 近隣情報登録
		BoidsNeighbor neighbor;
		// 位置
		neighbor.v3Position = { wolf->m_tParam.m_f3Pos.x, wolf->m_tParam.m_f3Pos.y, wolf->m_tParam.m_f3Pos.z };
		// 速度
		neighbor.v3Velocity = { wolf->m_f3Velocity.x, wolf->m_f3Velocity.y, wolf->m_f3Velocity.z };
		// 標的設定有無
		neighbor.bSetTarget = (pFlockAI != nullptr) ? pFlockAI->HasTarget() : false;
		// 標的位置
		neighbor.pTargetPos = (pFlockAI != nullptr) ? pFlockAI->GetTargetPosition() : DirectX::XMFLOAT3(0,0,0);

		// リストに追加
		m_SameAnimalNeighbors.push_back(neighbor);
	}

	// 自身のホームポジション設定
	if (auto* pFlockAI = dynamic_cast<CFlockAttackAI*>(m_pActionAI))
	{
		// ホームポジション設定
		pFlockAI->SetHomePosition(GetPos());
	}
}