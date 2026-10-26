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

// @brief 近隣の同種動物として扱う距離
constexpr float Wolf_Flock_Radius = 20.0f;

// @brief 獲物を仕留めた後の満腹の時間（秒）
// @note 満腹の間は狩りをしない。常に狩り続けると、鹿が補充を待たずに全滅してしまうため
constexpr float Wolf_Fed_Time = 480.0f;

/****************************************//*
	@brief	| コンストラクタ
*//****************************************/
CWolf_Animal::CWolf_Animal()
	: CCarnivorousAnimal()
{
	// 群れ情報の更新用に、生成されたオオカミを全体のリストへ登録する
	s_AllWolves.push_back(this);

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
	// 全体のリストから登録を外す
	s_AllWolves.erase(std::remove(s_AllWolves.begin(), s_AllWolves.end(), this), s_AllWolves.end());
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

	// 満腹の時間を進める
	if (m_fFedTimer > 0.0f)
	{
		m_fFedTimer -= fDeltaTime;
	}

	// 群れの情報(近隣の同種動物の位置・速度・標的)を最新の状態にする
	if (CAnimal::s_bRefreshFlockInfo)
	{
		RefreshFlock();
	}

	// この更新で標的に設定した草食動物(後の標的取得で再検索せずに済むよう保持する)
	CHerbivorousAnimal* pSetTarget = nullptr;

	// 時間帯による標的設定/解除
	switch (CGameTimeManager::GetInstance()->GetCurrentDayTime())
	{
	case CGameTimeManager::DAY_TIME::EVENING:
	case CGameTimeManager::DAY_TIME::NIGHT:
		// 夕方～夜は標的設定(満腹の間は狩りをしない)
		if (m_fFedTimer <= 0.0f)
		{
			pSetTarget = SetTarget();
		}
		else if (auto* pFedAI = dynamic_cast<CFlockAttackAI*>(m_pActionAI))
		{
			pFedAI->ClearTarget();
		}
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
			// この更新で標的に設定した動物が標的位置にいる場合は、再検索せずにそのまま使う
			if (pSetTarget != nullptr &&
				pSetTarget->GetPos().x == targetPos.x &&
				pSetTarget->GetPos().y == targetPos.y &&
				pSetTarget->GetPos().z == targetPos.z)
			{
				pTarget = pSetTarget;
			}
			else
			{
				pTarget = GetScene()->GetGameObject<CHerbivorousAnimal>(targetPos);
			}

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

			// 獲物を仕留めたら、しばらく満腹になって狩りをしない
			if (pTarget->IsDead())
			{
				m_fFedTimer = Wolf_Fed_Time;
			}
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

	// フィールドの外に出ないようにする
	KeepInsideField();
}

/****************************************//*
	@brief	| ターゲット設定
	@return	| 標的に設定した草食動物のポインタ(標的を設定しなかった場合はnullptr)
*//****************************************/
CHerbivorousAnimal* CWolf_Animal::SetTarget()
{
	// 攻撃AI取得
	auto* pFlockAI = dynamic_cast<CFlockAttackAI*>(m_pActionAI);
	// 攻撃AI無ければ終了
	if (!pFlockAI) return nullptr;

	// 群れの誰かがターゲットを持っていたら共有
	for (const BoidsNeighbor& Animal : m_SameAnimalNeighbors)
	{
		// ターゲットが設定されていない、または自分が既にターゲットを持っている場合はスキップ
		// (持っている間も共有すると、群れ全員が互いの古いターゲット位置を写し合い続けてしまう)
		if (!Animal.bSetTarget || pFlockAI->HasTarget()) continue;
		// ターゲット位置共有
		pFlockAI->SetTargetPosition(Animal.pTargetPos);
		return nullptr;
	}

	// 自身のターゲット設定
	CHerbivorousAnimal* pTargetAnimal = GetScene()->GetGameObject<CHerbivorousAnimal>(m_tParam.m_f3Pos);
	// ターゲットが存在しなければ終了
	if (!pTargetAnimal) return nullptr;

	// ターゲット位置設定
	pFlockAI->SetTargetPosition(pTargetAnimal->GetPos());

	// 標的に設定した草食動物を返す
	return pTargetAnimal;
}

/****************************************//*
	@brief	| 近隣の同種動物の情報を作り直す
	@param	| In_Wolfs：オオカミリスト
	@param	| fRadius：近隣として扱う距離
*//****************************************/
void CWolf_Animal::RefreshNeighbors(const std::vector<CWolf_Animal*>& In_Wolfs, float fRadius)
{
	// 同種動物リストクリア
	m_SameAnimalNeighbors.clear();

	// 近くの同種動物を登録
	for (CWolf_Animal* wolf : In_Wolfs)
	{
		// 自分自身はスキップ
		if (wolf == this || wolf->IsDestroy() || wolf->IsDead()) continue;

		// 一定距離以上離れていたらスキップ
		float dist = StructMath::Distance(GetPos(), wolf->GetPos());
		if (dist > fRadius) continue;

		AppendNeighbor(wolf);
	}
}

/****************************************//*
	@brief	| 近隣の同種動物の情報を1頭分追加する
	@param	| pWolf：近隣として追加する狼
*//****************************************/
void CWolf_Animal::AppendNeighbor(CWolf_Animal* pWolf)
{
	// 群れの動物の行動AI取得
	auto* pFlockAI = dynamic_cast<CFlockAttackAI*>(pWolf->m_pActionAI);

	// 近隣情報登録
	BoidsNeighbor neighbor;
	// 位置
	neighbor.v3Position = { pWolf->m_tParam.m_f3Pos.x, pWolf->m_tParam.m_f3Pos.y, pWolf->m_tParam.m_f3Pos.z };
	// 速度
	neighbor.v3Velocity = { pWolf->m_f3Velocity.x, pWolf->m_f3Velocity.y, pWolf->m_f3Velocity.z };
	// 標的設定有無
	neighbor.bSetTarget = (pFlockAI != nullptr) ? pFlockAI->HasTarget() : false;
	// 標的位置
	neighbor.pTargetPos = (pFlockAI != nullptr) ? pFlockAI->GetTargetPosition() : DirectX::XMFLOAT3(0,0,0);

	// リストに追加
	m_SameAnimalNeighbors.push_back(neighbor);
}

/****************************************//*
	@brief	| 群れ登録
	@param	| In_Wolfs：オオカミリスト
*//****************************************/
void CWolf_Animal::RegisterToFlock(std::vector<CWolf_Animal*> In_Wolfs)
{
	// 生成時点の近隣情報を登録
	RefreshNeighbors(In_Wolfs, Wolf_Flock_Radius);

	// 自身のホームポジション設定
	if (auto* pFlockAI = dynamic_cast<CFlockAttackAI*>(m_pActionAI))
	{
		// ホームポジション設定
		pFlockAI->SetHomePosition(GetPos());
	}
}

/****************************************//*
	@brief	| 群れの情報を最新の状態に更新
	@note	| 生成されている全てのオオカミから、近隣の位置・速度・標的を作り直す
*//****************************************/
void CWolf_Animal::RefreshFlock()
{
	// 近隣検索用の空間分割は、更新の1ステップに1回だけ作り直す
	if (s_nGridStep != CAnimal::GetUpdateStep())
	{
		s_Grid.Build(s_AllWolves, Wolf_Flock_Radius);
		s_nGridStep = CAnimal::GetUpdateStep();
	}

	m_SameAnimalNeighbors.clear();

	// 周囲のマスにいる狼だけを調べる(全頭との距離は調べない)
	const DirectX::XMFLOAT3 myPos = GetPos();
	const float fRadiusSq = Wolf_Flock_Radius * Wolf_Flock_Radius;
	s_Grid.ForEachNear(myPos, [&](CWolf_Animal* wolf)
	{
		if (wolf == this || wolf->IsDestroy() || wolf->IsDead()) return;

		const DirectX::XMFLOAT3 pos = wolf->GetPos();
		const float dx = myPos.x - pos.x;
		const float dy = myPos.y - pos.y;
		const float dz = myPos.z - pos.z;
		if (dx * dx + dy * dy + dz * dz > fRadiusSq) return;

		AppendNeighbor(wolf);
	});
}
