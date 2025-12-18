#include "Deer_Animal.h"
#include "FlockEscapeAI.h"
#include "Main.h"
#include "CarnivorousAnimal.h"
#include "Oparation.h"
#include <cstdint>

CDeer_Animal::CDeer_Animal()
	: CHerbivorousAnimal()
{
	BoidsParams params;

	// --- 視野半径と分離半径 ---
	// 密集しすぎを避けるため、視野/分離をやや広め
	params.fViewRadius =18.0f;
	params.fSeparationRadius =9.0f;

	// --- 各行動の重み係数 ---
	// 草食:「少しまとまるが、近づきすぎない」
	params.fWeightSeparation =2.2f;
	params.fWeightAlignment =0.55f;
	params.fWeightCohesion =0.18f;

	// ---速度と力の最大値 ---
	params.fMaxSpeed =7.0f;
	params.fMaxForce =3.5f;

	// --- 各行動の最大力 ---
	params.fMaxSeparationForce =4.0f;
	params.fMaxAlignmentForce =3.0f;
	params.fMaxCohesionForce =2.0f;

	m_pActionAI = new(std::nothrow) CFlockEscapeAI(params);
}

CDeer_Animal::~CDeer_Animal()
{
}

float CDeer_Animal::Rand01()
{
	// 簡易LCG（個体ごとに独立）
	m_uRandSeed =1664525u * m_uRandSeed +1013904223u;
	// 上位24bitを0..1へ
	return static_cast<float>((m_uRandSeed >>8) &0x00FFFFFF) / static_cast<float>(0x01000000);
}

void CDeer_Animal::Init()
{
	CHerbivorousAnimal::Init();

	// ModelRendererコンポーネント設定
	CModelRenderer* pModelRenderer = GetComponent<CModelRenderer>();
	pModelRenderer->SetKey("Deer");


	// 個体ごとに異なるシードにする（IDのsameCountと座標を混ぜる）
	ObjectID id = GetID();
	auto p = GetPos();
	uint32_t h =2166136261u;
	auto mix = [&h](uint32_t v)
	{
		h ^= v;
		h *=16777619u;
	};
	mix(static_cast<uint32_t>(id.m_nSameCount));
	mix(static_cast<uint32_t>(static_cast<int>(p.x *1000.0f)));
	mix(static_cast<uint32_t>(static_cast<int>(p.z *1000.0f)));
	m_uRandSeed = (h ==0 ?1u : h);

	// 初期wander方向
	float angle = Rand01() * DirectX::XM_2PI;
	m_WanderDir = { sinf(angle),0.0f, cosf(angle) };

	m_fWanderUpdateTimer =0.0f;
	m_fIdleTimer =0.0f;
	m_fNextWanderUpdate =0.8f + Rand01() *1.6f; //0.8..2.4
	m_fNextIdleChange =1.5f + Rand01() *3.0f; //1.5..4.5
	m_bIdle = (Rand01() <0.25f);
}

void CDeer_Animal::Update()
{
	CHerbivorousAnimal::Update();

	// 脅威の検知＆共有
	SetThreat();

	DirectX::XMFLOAT3 pos = { m_tParam.m_f3Pos.x, m_tParam.m_f3Pos.y, m_tParam.m_f3Pos.z };
	DirectX::XMFLOAT3 vel = { m_f3Velocity.x, m_f3Velocity.y, m_f3Velocity.z };

	bool hasThreat = false;
	if (auto* pEscapeAI = dynamic_cast<CFlockEscapeAI*>(m_pActionAI))
	{
		hasThreat = pEscapeAI->HasThreat();
		if (hasThreat)
		{
			//逃避中はまとまりを崩して散る
			pEscapeAI->SetEscapeTuning(2.6f,0.05f);
		}
	}

	// ------------------------------------------------------------
	// Idle/Wanderの状態更新（脅威が無い時のみ）
	// ------------------------------------------------------------
	if (!hasThreat)
	{
		m_fWanderUpdateTimer += fDeltaTime;
		m_fIdleTimer += fDeltaTime;

		// 一定間隔でwander方向を更新（個体差あり）
		if (m_fWanderUpdateTimer >= m_fNextWanderUpdate)
		{
			m_fWanderUpdateTimer =0.0f;
			m_fNextWanderUpdate =0.8f + Rand01() *1.6f;

			// 現在方向から少しだけ曲げる（急旋回しない）
			float delta = (Rand01() *2.0f -1.0f) * (DirectX::XM_PI /5.0f); // ±36deg
			float base = atan2f(m_WanderDir.x, m_WanderDir.z);
			float angle = base + delta;
			m_WanderDir = { sinf(angle),0.0f, cosf(angle) };
		}

		// 一定間隔で立ち止まり/移動を切り替える
		if (m_fIdleTimer >= m_fNextIdleChange)
		{
			m_fIdleTimer =0.0f;
			m_fNextIdleChange =1.5f + Rand01() *3.0f;
			//25%: idle,75%: move
			m_bIdle = (Rand01() <0.25f);
		}
	}
	else
	{
		//逃避中は止まらない
		m_bIdle = false;
	}

	DirectX::XMFLOAT3 steer = m_pActionAI->UpdateAI(pos, vel, m_SameAnimalNeighbors);

	// ---直進し続ける不自然さを抑える調整 ---
	//1)速度減衰（摩擦）
	const float drag =0.92f;
	vel = vel * drag;

	//2)追従に少しゆらぎを入れる（wander + idle）
	float wanderStrength =0.6f;
	if (hasThreat) wanderStrength =0.15f;

	// 個体ごとの徘徊方向（群れAIの力を邪魔しない程度に）
	DirectX::XMFLOAT3 wander = m_WanderDir * wanderStrength;

	// Idle中は加速をほぼ0へ（自然に停止）
	float moveFactor =1.0f;
	if (!hasThreat && m_bIdle)
	{
		moveFactor =0.15f;
		wander = wander *0.0f;
	}

	vel += (steer + wander) * (fDeltaTime * moveFactor);

	// 最大速度制限
	float speed = StructMath::Length(vel);
	const float maxMoveSpeed =4.5f;
	if (speed > maxMoveSpeed)
	{
		vel = vel * (maxMoveSpeed / speed);
		speed = maxMoveSpeed;
	}

	// Idle中はさらに減速して停止しやすくする
	if (!hasThreat && m_bIdle)
	{
		vel = vel *0.65f;
	}

	//速度Yを潰さず、位置のYを地面へスナップする ----
	m_f3Velocity = { vel.x, vel.y, vel.z };

	// 移動
	m_tParam.m_f3Pos += m_f3Velocity * fDeltaTime;

	// 地面にスナップ（このプロジェクトは基本的に地面=0想定）
	// ※ここでYを固定することで、速度Yを潰すより副作用を減らす
	m_tParam.m_f3Pos.y =0.0f;

	// 向き更新はXZ平面のみで行う
	DirectX::XMFLOAT3 vXZ = { m_f3Velocity.x,0.0f, m_f3Velocity.z };
	float speedXZ = StructMath::Length(vXZ);
	if (speedXZ >0.05f)
	{
		m_tParam.m_f3Rotate.y = atan2f(vXZ.x, vXZ.z);
	}
}

void CDeer_Animal::SetThreat()
{
	auto* pEscapeAI = dynamic_cast<CFlockEscapeAI*>(m_pActionAI);
	if (!pEscapeAI) return;

	// 1) 既に群れ内共有済みの脅威があるなら、十分離れたら解除
	if (pEscapeAI->HasThreat())
	{
		float dist = StructMath::Distance(GetPos(), pEscapeAI->GetThreatPosition());
		if (dist > m_ClearRange)
		{
			pEscapeAI->ClearThreat();
		}
		return;
	}

	// 2) 群れの誰かが脅威検知してたら共有（BoidsNeighborの "target" フィールドを流用）
	for (const BoidsNeighbor& nb : m_SameAnimalNeighbors)
	{
		if (!nb.bSetTarget) continue;
		pEscapeAI->SetThreatPosition(nb.pTargetPos);
		return;
	}

	// 3) 自分で肉食動物を近傍検索（最寄りの肉食を見る）
	CCarnivorousAnimal* pPredator = GetScene()->GetGameObject<CCarnivorousAnimal>(GetPos());
	if (!pPredator) return;

	float dist = StructMath::Distance(GetPos(), pPredator->GetPos());
	if (dist <= m_AlertRange)
	{
		pEscapeAI->SetThreatPosition(pPredator->GetPos());
	}
}

void CDeer_Animal::RegisterToFlock(std::vector<CDeer_Animal*> In_Deers)
{
	m_SameAnimalNeighbors.clear();

	for (CDeer_Animal* deer : In_Deers)
	{
		if (deer == this) continue;

		auto* pEscapeAI = dynamic_cast<CFlockEscapeAI*>(deer->m_pActionAI);

		BoidsNeighbor neighbor;
		neighbor.v3Position = { deer->m_tParam.m_f3Pos.x, deer->m_tParam.m_f3Pos.y, deer->m_tParam.m_f3Pos.z };
		neighbor.v3Velocity = { deer->m_f3Velocity.x, deer->m_f3Velocity.y, deer->m_f3Velocity.z };

		// "脅威" を BoidsNeighbor 側の targetで共有する（既存設計に合わせて流用）
		neighbor.bSetTarget = (pEscapeAI != nullptr) ? pEscapeAI->HasThreat() : false;
		neighbor.pTargetPos = (pEscapeAI != nullptr) ? pEscapeAI->GetThreatPosition() : DirectX::XMFLOAT3(0,0,0);

		m_SameAnimalNeighbors.push_back(neighbor);
	}
}