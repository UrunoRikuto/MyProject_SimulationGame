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

/****************************************//*
	@brief　	| コンストラクタ
*//****************************************/
CWolf_Animal::CWolf_Animal()
	: CCarnivorousAnimal()
{
	BoidsParams params;

	// --- 視野半径と分離半径 ---
	params.fViewRadius = 12.0f;
	params.fSeparationRadius = 6.0f;
	// --- 各行動の重み係数 ---
	params.fWeightSeparation = 1.5f;	// 分離行動の重み係数
	params.fWeightAlignment = 0.5f;		// 整列行動の重み係数
	params.fWeightCohesion = 0.5f;		// 凝集行動の重み係数
	// --- 速度と力の最大値 ---
	params.fMaxSpeed = 6.0f;			// 最大速度
	params.fMaxForce = 3.0f;			// 最大力
	// --- 各行動の最大力 ---
	params.fMaxSeparationForce = 3.0f;	// 分離行動の最大力
	params.fMaxAlignmentForce = 3.0f;	// 整列行動の最大力
	params.fMaxCohesionForce = 3.0f;	// 凝集行動の最大力
	
	// 行動AI生成
	m_pActionAI = new(std::nothrow) CFlockAttackAI(params);
}

/****************************************//*
	@brief　	| デストラクタ
*//****************************************/
CWolf_Animal::~CWolf_Animal()
{
}

/****************************************//*
	@brief　	| 初期化処理
*//****************************************/
void CWolf_Animal::Init()
{
	// 親クラスの初期化処理
	CCarnivorousAnimal::Init();

	// モデルレンダラーコンポーネントの設定
	CModelRenderer* pModelRenderer = GetComponent<CModelRenderer>();
	pModelRenderer->SetKey("Wolf");
	pModelRenderer->SetRendererParam(m_tParam);
}

/****************************************//*
	@brief　	| 更新処理
*//****************************************/
void CWolf_Animal::Update()
{
	// 親クラスの更新処理
	CCarnivorousAnimal::Update();

	// 時間帯の取得
	switch (CGameTimeManager::GetInstance()->GetCurrentDayTime())
	{
	case CGameTimeManager::DAY_TIME::EVENING:
	case CGameTimeManager::DAY_TIME::NIGHT:
		// 標的は設定するが、ここでreturnすると移動更新が止まって硬直する
		SetTarget();
		break;
	default:
		// 標的解除
		dynamic_cast<CFlockAttackAI*>(m_pActionAI)->ClearTarget();
		break;
	}

	DirectX::XMFLOAT3 pos = { m_tParam.m_f3Pos.x, m_tParam.m_f3Pos.y, m_tParam.m_f3Pos.z };
	DirectX::XMFLOAT3 vel = { m_f3Velocity.x, m_f3Velocity.y, m_f3Velocity.z };

	// ステアリングベクトル
	DirectX::XMFLOAT3 steer = m_pActionAI->UpdateAI(pos, vel, m_SameAnimalNeighbors);

	// 速度にステアリングを加算
	vel += steer * fDeltaTime;

	// 簡易減衰（暴れ防止）
	vel = vel * 0.98f;

	// 最大速度で制限
	float speed = StructMath::Length(vel);
	const float maxSpeed = 6.0f; // BoidsParams.fMaxSpeed と揃える
	// 最大速度を超えていたら制限
	if (speed > maxSpeed) vel = vel * (maxSpeed / speed);

	// 速度を保存
	m_f3Velocity = { vel.x, vel.y, vel.z };
	// 回転を速度方向に向ける
	m_tParam.m_f3Rotate.y = atan2f(m_f3Velocity.x, m_f3Velocity.z);

	// 位置に速度を加算
	m_tParam.m_f3Pos += m_f3Velocity * fDeltaTime;
}

/****************************************//*
	@brief　	| 標的の設定
*//****************************************/
void CWolf_Animal::SetTarget()
{
	// 行動AIの取得
	CFlockAttackAI* pFlockAI = dynamic_cast<CFlockAttackAI*>(m_pActionAI);

	// 群れのメンバーが存在するか確認
	if (m_SameAnimalNeighbors.empty()) return;

	// 群れの中に標的を見つけているものがいるか確認
	for (const BoidsNeighbor Animal : m_SameAnimalNeighbors)
	{
		if (!Animal.bSetTarget) continue;

		// 標的位置を共有（値コピー）
		pFlockAI->SetTargetPosition(Animal.pTargetPos);
		return;
	}

	CHerbivorousAnimal* pTargetAnimal = GetScene()->GetGameObject<CHerbivorousAnimal>(m_tParam.m_f3Pos);
	if (pTargetAnimal == nullptr) return;

	pFlockAI->SetTargetPosition(pTargetAnimal->GetPos());
}

/****************************************//*
	@brief　	| 群れの登録
	@param      | In_Wolfs：オオカミリスト
*//****************************************/
void CWolf_Animal::RegisterToFlock(std::vector<CWolf_Animal*> In_Wolfs)
{
	// 近隣リストをクリア
	m_SameAnimalNeighbors.clear();

	// オオカミリストを走査
	for (CWolf_Animal* wolf : In_Wolfs)
	{
		// 自分自身は登録しない
		if (wolf == this)continue;

		// 集団行動AIの取得
		CFlockAttackAI* pFlockAI = dynamic_cast<CFlockAttackAI*>(wolf->m_pActionAI);

		// 近隣情報構造体の作成
		BoidsNeighbor neighbor;
		neighbor.v3Position = { wolf->m_tParam.m_f3Pos.x, wolf->m_tParam.m_f3Pos.y, wolf->m_tParam.m_f3Pos.z };
		neighbor.v3Velocity = { wolf->m_f3Velocity.x, wolf->m_f3Velocity.y, wolf->m_f3Velocity.z };
		// 標的位置ポインタの取得
		neighbor.bSetTarget = pFlockAI->HasTarget();
		neighbor.pTargetPos = pFlockAI->GetTargetPosition();

		// 近隣リストに追加
		m_SameAnimalNeighbors.push_back(neighbor);
	}

	// RegisterToFlock() 内のどこかで一回だけ
	if (auto* pFlockAI = dynamic_cast<CFlockAttackAI*>(m_pActionAI))
	{
		pFlockAI->SetHomePosition(GetPos());
	}
}