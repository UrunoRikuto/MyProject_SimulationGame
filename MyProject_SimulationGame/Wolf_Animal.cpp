/**************************************************//*
	@file	| Wolf_Animal.cpp
	@brief	| オオカミクラスの定義
	@note	| オオカミの処理を定義
*//**************************************************/
#include "Wolf_Animal.h"
#include "FlockAttackAI.h"
#include "Oparation.h"
#include "Main.h"

/****************************************//*
	@brief　	| コンストラクタ
*//****************************************/
CWolf_Animal::CWolf_Animal()
	: CCarnivorousAnimal()
{
	BoidsParams params;
	// パラメータ設定
	params.fViewRadius = 5.0f;
	params.fSeparationRadius = 10.0f;
	params.fWeightSeparation = 1.0f;
	params.fWeightAlignment = 0.1f;
	params.fWeightCohesion = 0.1f;
	params.fMaxSpeed = 3.0f;
	params.fMaxForce = 1.0f;
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

	// 近隣情報を値で保持（動的メモリ排除）
	m_SameAnimalNeighbors.clear();

	// 同種の動物を取得
	auto WolfObjects = GetScene()->GetGameObjects<CWolf_Animal>(m_tParam.m_f3Pos, 1000.0f);
	for (auto& wolf : WolfObjects)
	{
		// 自身は除外
		if (wolf->GetID().m_sName == this->GetID().m_sName
			&& wolf->GetID().m_nSameCount == this->GetID().m_nSameCount) continue;

		// 近隣情報を追加
		BoidsNeighbor neighbor;
		neighbor.v3Position = { wolf->m_tParam.m_f3Pos.x, wolf->m_tParam.m_f3Pos.y, wolf->m_tParam.m_f3Pos.z };
		neighbor.v3Velocity = { wolf->m_f3Velocity.x, wolf->m_f3Velocity.y, wolf->m_f3Velocity.z };
		// リストに追加
		m_SameAnimalNeighbors.push_back(neighbor);
	}

	Vec3 pos = { m_tParam.m_f3Pos.x, m_tParam.m_f3Pos.y, m_tParam.m_f3Pos.z };
	Vec3 vel = { m_f3Velocity.x, m_f3Velocity.y, m_f3Velocity.z };

	// ステアリングベクトル
	Vec3 steer = m_pActionAI->UpdateAI(pos, vel, m_SameAnimalNeighbors);

	// 速度にステアリングを加算
	vel += steer * fDeltaTime;

	// 最大速度で制限
	float speed = Length(vel);
	// 最大速度を超えていたら制限
	if (speed > 3.0f && speed > 0.0001f)
	{
		// 最大速度で正規化
		vel = vel * (3.0f / speed);
	}

	// 速度を保存
	m_f3Velocity = { vel.x, vel.y, vel.z };
	// 回転を速度方向に向ける
	m_tParam.m_f3Rotate.y = atan2f(m_f3Velocity.x, m_f3Velocity.z);

	// 位置に速度を加算
	m_tParam.m_f3Pos += m_f3Velocity * fDeltaTime;
}

