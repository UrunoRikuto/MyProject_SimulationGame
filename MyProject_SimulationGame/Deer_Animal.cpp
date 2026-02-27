/**************************************************//*
	@file	| Deer_Animal.cpp
	@brief	| 鹿クラス実装
	@note	| 鹿の振る舞いを定義
			| CHerbivorousAnimalを継承
*//**************************************************/
#include "Deer_Animal.h"
#include "FlockEscapeAI.h"
#include "Main.h"
#include "CarnivorousAnimal.h"
#include "Oparation.h"
#include <cstdint>
#include <algorithm>

/*****************************************//*
	@brief　	| コンストラクタ
*//*****************************************/
CDeer_Animal::CDeer_Animal()
	: CHerbivorousAnimal()
{
	// Boidsパラメータ設定
	BoidsParams params;

	// --- 視野半径と分離半径 ---
	params.fViewRadius = 18.0f;
	params.fSeparationRadius = 9.0f;

	// --- 各行動の重み係数 ---
	params.fWeightSeparation = 2.2f;
	params.fWeightAlignment = 0.30f;
	params.fWeightCohesion = 0.18f;

	// ---速度と力の最大値 ---
	params.fMaxSpeed =7.5f;
	params.fMaxForce =4.0f;

	// --- 各行動の最大力 ---
	params.fMaxSeparationForce = 4.0f;
	params.fMaxAlignmentForce = 3.0f;
	params.fMaxCohesionForce = 2.0f;

	// 集団逃避AI生成
	m_pActionAI = new(std::nothrow) CFlockEscapeAI(params);
}

/*****************************************//*
	@brief　	| デストラクタ
*//*****************************************/
CDeer_Animal::~CDeer_Animal()
{
}

/*****************************************//*
	@brief　	| 0..1の乱数取得
	@return		| 乱數(0.0f～1.0f)
	@note		| 線形合同法で乱数生成
*//*****************************************/
float CDeer_Animal::Rand01()
{
	// 線形合同法
	m_uRandSeed = 1664525u * m_uRandSeed + 1013904223u;

	// 上位24ビットを使用して0.0f～1.0fに正規化
	return static_cast<float>((m_uRandSeed >> 8) &0x00FFFFFF) / static_cast<float>(0x01000000);
}

/*****************************************//*
	@brief　	| 初期化処理
*//*****************************************/
void CDeer_Animal::Init()
{
	// 親クラス初期化
	CHerbivorousAnimal::Init();

	// ModelRendererコンポーネント設定
	GetComponent<CModelRenderer>()->SetKey("Deer");;

	// 乱数シード初期化（個体ごとに異なる値にする）
	// オブジェクトIDと初期位置からハッシュ値を生成
	ObjectID id = GetID();
	auto p = GetPos();
	// FNV-1aハッシュ
	uint32_t h = 2166136261u;
	// ハッシュ混ぜ込み関数
	auto mix = [&h](uint32_t v)
	{
		h ^= v;
		h *= 16777619u;
	};
	// 混ぜ込み
	mix(static_cast<uint32_t>(id.m_nSameCount));
	mix(static_cast<uint32_t>(static_cast<int>(p.x * 1000.0f)));
	mix(static_cast<uint32_t>(static_cast<int>(p.z * 1000.0f)));
	// シード値決定（0は避ける）
	m_uRandSeed = (h == 0 ? 1u : h);

	// 徘徊方向初期化
	float angle = Rand01() * DirectX::XM_2PI;
	m_WanderDir = { sinf(angle),0.0f, cosf(angle) };

	// タイマー初期化
	m_fWanderUpdateTimer = 0.0f;
	m_fIdleTimer = 0.0f;

	// 次回更新時間初期化
	m_fNextWanderUpdate = 0.8f + Rand01() * 1.6f;
	// 次回Idle/Wander切り替え時間初期化
	m_fNextIdleChange = 1.5f + Rand01() * 3.0f;
	// 初期状態は25%でIdle
	m_bIdle = (Rand01() < 0.25f);

	// 逃避AIパラメータ調整
	if (auto* pEscapeAI = dynamic_cast<CFlockEscapeAI*>(m_pActionAI))
	{
		// 逃避中はあまり密集しないように調整
		pEscapeAI->SetEscapeAlignmentScale(0.4f);
		pEscapeAI->SetFleeParams(1.8f, 22.0f);
	}

	// 脅威チェックタイマー初期化
	m_fThreatCheckTimer =0.0f;
}

/*****************************************//*
	@brief　	| 更新処理
*//*****************************************/
void CDeer_Animal::Update()
{
	// 親クラス更新
	CHerbivorousAnimal::Update();

	// 脅威チェック用タイマーを進める
	m_fThreatCheckTimer += fDeltaTime;

	// 逃避AIの場合、脅威がないときは一定間隔で脅威の有無をチェックして更新する
	{
		auto* pEscapeAI = dynamic_cast<CFlockEscapeAI*>(m_pActionAI);
		bool currentlyHasThreat = (pEscapeAI != nullptr) ? pEscapeAI->HasThreat() : false;
		if (currentlyHasThreat || m_fThreatCheckTimer >= m_fThreatCheckInterval)
		{
			// タイマーリセット
			m_fThreatCheckTimer =0.0f;
			SetThreat();
		}
	}

	// 位置と速度取得
	DirectX::XMFLOAT3 pos = m_tParam.m_f3Pos;
	DirectX::XMFLOAT3 vel = m_f3Velocity;

	// 逃避中かどうか確認
	bool hasThreat = false;
	if (auto* pEscapeAI = dynamic_cast<CFlockEscapeAI*>(m_pActionAI))
	{
		// 脅威の有無取得
		hasThreat = pEscapeAI->HasThreat();
		if (hasThreat)
		{
			// 逃避中は密集しすぎないように調整
			pEscapeAI->SetEscapeTuning(2.8f, 0.05f);
		}
	}

	// 脅威がいない場合
	if (!hasThreat)
	{
		// 徘徊と待機のタイマー更新
		m_fWanderUpdateTimer += fDeltaTime;
		m_fIdleTimer += fDeltaTime;

		// 一定間隔で徘徊方向を更新
		if (m_fWanderUpdateTimer >= m_fNextWanderUpdate)
		{
			// タイマーリセット
			m_fWanderUpdateTimer = 0.0f;
			// 次回更新時間決定
			m_fNextWanderUpdate = 0.8f + Rand01() * 1.6f;

			// 徘徊方向をランダムに少し変化させる
			float delta = (Rand01() * 2.0f - 1.0f) * (DirectX::XM_PI / 5.0f);
			// 現在の方向の角度を取得
			float base = atan2f(m_WanderDir.x, m_WanderDir.z);
			// 新しい方向を計算
			float angle = base + delta;
			// 徘徊方向更新
			m_WanderDir = {sinf(angle),0.0f, cosf(angle) };
		}

		// 一定間隔で待機と徘徊を切り替え
		if (m_fIdleTimer >= m_fNextIdleChange)
		{
			// タイマーリセット
			m_fIdleTimer = 0.0f;
			// 次回切り替え時間決定
			m_fNextIdleChange = 1.5f + Rand01() * 3.0f;

			// 待機/徘徊切り替え
			//25%: idle,75%: move
			m_bIdle = (Rand01() < 0.25f);
		}
	}
	else
	{
		//逃避中は止まらない
		m_bIdle = false;
	}

	// Boidsのステアリングを取得
	DirectX::XMFLOAT3 steer = m_pActionAI->UpdateAI(pos, vel, m_SameAnimalNeighbors);

	// 減速
	const float drag = 0.975f;
	vel = vel * drag;

	// 徘徊とジッター付加
	float wanderStrength = 1.0f;
	if (hasThreat) wanderStrength = 0.5f;

	// 徘徊成分
	DirectX::XMFLOAT3 wander = m_WanderDir * wanderStrength;

	// ジッター成分
	DirectX::XMFLOAT3 vXZ = { vel.x,0.0f, vel.z };
	// 移動方向の長さ取得
	float vLen = StructMath::Length(vXZ);
	// 十分に移動しているときのみジッターを付加
	DirectX::XMFLOAT3 jitter{ 0,0,0 };

	// 移動速度が遅いと蛇行しすぎるので抑制
	if (vLen >0.05f)
	{
		// 移動方向の単位ベクトル取得
		DirectX::XMFLOAT3 fwd = StructMath::Normalize(vXZ);
		// 右方向ベクトル計算
		DirectX::XMFLOAT3 right = { fwd.z, 0.0f, -fwd.x };

		// ジッター周波数
		float hz = 0.4f + Rand01() * 1.2f;

		// ジッター成分計算
		float phase = Rand01() * DirectX::XM_2PI;
		float s = sinf(fDeltaTime + phase) * 0.25f;
		jitter = right * s;
	}

	// 待機中は移動量を大幅に減らす
	float moveFactor = 1.0f;
	// 待機中は徘徊もジッターも無し
	if (!hasThreat && m_bIdle)
	{
		moveFactor = 0.15f;
		wander = wander * 0.0f;
		jitter = { 0,0,0 };
	}

	// 加速係数（逃避中は少し高め）
	const float accelScale = hasThreat ? 1.25f : 1.1f;
	// 速度更新
	vel += (steer + wander + jitter) * (fDeltaTime * moveFactor * accelScale);

	// スタミナ参照で最大移動速度を制御
	const float staminaMax = (GetMaxStamina() > 0.0f) ? GetMaxStamina() : 1.0f;

	// スタミナ率0.0f～1.0f
	float staminaRate = GetStamina() / staminaMax;

	// クランプ
	staminaRate = std::clamp(staminaRate, 0.0f, 1.0f);

	// 基本最大速度と最低最大速度設定
	const float baseMaxSpeed = hasThreat ? 9.5f : 7.0f;
	// 最低最大速度設定 (スタミナ0時の上限)
	const float minMaxSpeed = hasThreat ? 2.0f : 1.5f;

	// スタミナ率に応じた速度変化曲線適用
	float shaped = staminaRate;
	// 低スタミナ域を圧縮
	if (staminaRate <= 0.2f)
	{
		//0.0..0.2 ->0.0..0.25 に圧縮
		shaped = staminaRate * 0.25f;
	}
	else
	{
		//0.2..1.0 ->0.25..1.0 に拡大
		const float t = (staminaRate - 0.2f) / 0.8f;
		shaped = 0.05f + 0.95f * t;
		shaped = shaped * shaped;
	}

	// スタミナに応じた最大移動速度計算
	const float maxMoveSpeed = minMaxSpeed + (baseMaxSpeed - minMaxSpeed) * shaped;

	// 最大速度制限
	float speed = StructMath::Length(vel);

	// 制限超過時はスケーリング
	if (speed > maxMoveSpeed)
	{
		// スケーリング
		vel = vel * (maxMoveSpeed / speed);
		speed = maxMoveSpeed;
	}

	// 待機中はさらに速度を落とす
	if (!hasThreat && m_bIdle)
	{
		vel = vel * 0.65f;
	}

	// スタミナ消費/回復処理
	const float speed01 = (baseMaxSpeed > 0.0f) ? (speed / baseMaxSpeed) : 0.0f;
	float consume = 0.0f;
	// 移動中は消費
	if (!m_bIdle)
	{
		// 消費を増やす
		consume = (hasThreat ? 18.0f : 9.0f) * speed01 * fDeltaTime;
	}
	// 消費/回復実行
	if (consume > 0.0f)
	{
		// 移動中は消費
		DecreaseStamina(consume);
	}
	else
	{
		// 待機中は回復
		RecoverStamina(6.0f * fDeltaTime);
	}

	// 速度保存
	m_f3Velocity = {vel.x, vel.y, vel.z };

	// 位置更新
	m_tParam.m_f3Pos += m_f3Velocity * fDeltaTime;

	// Y座標は地面に固定
	m_tParam.m_f3Pos.y = 0.0f;

	// 向き更新（XZ平面）
	DirectX::XMFLOAT3 moveXZ = { m_f3Velocity.x,0.0f, m_f3Velocity.z };

	// 移動速度が十分な場合のみ向き更新
	float speedXZ = StructMath::Length(moveXZ);
	if (speedXZ > 0.05f)
	{
		// 向き更新
		m_tParam.m_f3Rotate.y = atan2f(moveXZ.x, moveXZ.z);
	}
}

/*****************************************//*
	@brief　	| 脅威（肉食動物）を群れで共有して逃避開始
*//*****************************************/
void CDeer_Animal::SetThreat()
{
	// 逃避AI取得
	auto* pEscapeAI = dynamic_cast<CFlockEscapeAI*>(m_pActionAI);

	// 逃避AI無ければ終了
	if (!pEscapeAI) return;

	// 脅威を既に持っている場合
	if (pEscapeAI->HasThreat())
	{
		// 脅威が一定距離以上離れたらクリア
		float dist = StructMath::Distance(GetPos(), pEscapeAI->GetThreatPosition());

		// クリア距離以上なら解除
		if (dist > m_ClearRange)
		{
			// 解除
			pEscapeAI->ClearThreat();
		}

		return;
	}

	// 群れの誰かが脅威を持っていたら共有
	for (const BoidsNeighbor& nb : m_SameAnimalNeighbors)
	{
		// 脅威が設定されていなければスキップ
		if (!nb.bSetTarget) continue;
		// 脅威位置共有
		pEscapeAI->SetThreatPosition(nb.pTargetPos);
		return;
	}

	// 自分で脅威を見つける
	CCarnivorousAnimal* pPredator = GetScene()->GetGameObject<CCarnivorousAnimal>(GetPos());
	// 見つからなければ終了
	if (!pPredator) return;

	// 脅威との距離計算
	float dist = StructMath::Distance(GetPos(), pPredator->GetPos());
	// 脅威が警戒範囲内なら逃避開始
	if (dist <= m_AlertRange)
	{
		// 脅威位置設定
		pEscapeAI->SetThreatPosition(pPredator->GetPos());
	}
}

/*****************************************//*
	@brief　	| 群れの登録
	@param　	| In_Deers：同種の動物リスト
*//*****************************************/
void CDeer_Animal::RegisterToFlock(std::vector<CDeer_Animal*> In_Deers)
{
	// 既存データクリア
	m_SameAnimalNeighbors.clear();

	// 群れの同種動物を登録
	for (CDeer_Animal* deer : In_Deers)
	{
		// 自分自身はスキップ
		if (deer == this) continue;

		// 逃避AI取得
		auto* pEscapeAI = dynamic_cast<CFlockEscapeAI*>(deer->m_pActionAI);

		// 近隣情報作成
		BoidsNeighbor neighbor;
		// 位置
		neighbor.v3Position = deer->m_tParam.m_f3Pos;
		// 速度
		neighbor.v3Velocity = deer->m_f3Velocity;

		// 脅威情報共有
		neighbor.bSetTarget = (pEscapeAI != nullptr) ? pEscapeAI->HasThreat() : false;
		neighbor.pTargetPos = (pEscapeAI != nullptr) ? pEscapeAI->GetThreatPosition() : DirectX::XMFLOAT3(0, 0, 0);

		// リストに追加
		m_SameAnimalNeighbors.push_back(neighbor);
	}
}