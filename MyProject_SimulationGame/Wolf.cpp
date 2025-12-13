/**************************************************//*
	@file	| Wolf.cpp
	@brief	| オオカミクラスの定義
	@note	| オオカミの処理を定義
*//**************************************************/
#include "Wolf.h"
#include "FlockAttackAI.h"
#include "Oparation.h"
#include "Main.h"

/****************************************//*
	@brief　	| コンストラクタ
*//****************************************/
CWolf::CWolf()
	: CCarnivorousAnimal()
{
	// 群れ攻撃AIを作成して設定
	BoidsParams params;
	// パラメータ調整
	params.fWeightCohesion = 1.0f;
	params.fWeightAlignment = 1.2f;
	params.fWeightSeparation = 0.8f;
	params.fViewRadius = 10.0f;
	m_pActionAI = new(std::nothrow) CFlockAttackAI(params);
}

/****************************************//*
	@brief　	| デストラクタ
*//****************************************/
CWolf::~CWolf()
{
}

/****************************************//*
	@brief　	| 初期化処理
*//****************************************/
void CWolf::Init()
{
	// モデルレンダラーコンポーネントの設定
	CModelRenderer* pModelRenderer = GetComponent<CModelRenderer>();
	pModelRenderer->SetKey("Wolf");
	pModelRenderer->SetRendererParam(m_tParam);
}

/****************************************//*
	@brief　	| 更新処理
*//****************************************/
void CWolf::Update()
{
	// 基底クラスの更新処理を呼び出し
	CCarnivorousAnimal::Update();

	// -- 行動AIの更新
	// 近隣情報リストをクリア
	m_SameAnimalNeighbors.clear();
	// 周囲の同種動物の取得
	auto WolfObjects = GetScene()->GetGameObjects<CWolf>(m_tParam.m_f3Pos, 1000.0f);
	// 近くのオオカミを近隣情報リストに追加
	for (auto& wolf : WolfObjects)
	{
		// 自分自身は除外
		if (wolf->GetID().m_sName == this->GetID().m_sName
			&& wolf->GetID().m_nSameCount == this->GetID().m_nSameCount) continue;
		// 近隣情報構造体を作成
		BoidsNeighbor neighbor;
		// 位置情報ポインタを設定
		neighbor.v3Position = new Vec3{ wolf->m_tParam.m_f3Pos.x, wolf->m_tParam.m_f3Pos.y, wolf->m_tParam.m_f3Pos.z };
		// 速度情報ポインタを設定
		neighbor.v3Velocity = new Vec3{ wolf->m_f3Velocity.x, wolf->m_f3Velocity.y, wolf->m_f3Velocity.z };
		// 近隣リストに追加
		m_SameAnimalNeighbors.push_back(neighbor);
	}

	Vec3 pos = { m_tParam.m_f3Pos.x, m_tParam.m_f3Pos.y, m_tParam.m_f3Pos.z };
	Vec3 vel = { m_f3Velocity.x, m_f3Velocity.y, m_f3Velocity.z };
	// ステアリングベクトルを取得
	Vec3 steer = m_pActionAI->UpdateAI(pos, vel, m_SameAnimalNeighbors);

	// 速度にステアリングを加算
	vel += steer * fDeltaTime;
	// 変数に速度を保存
	m_f3Velocity = { vel.x, vel.y, vel.z };
	// 向きを速度方向に変更
	m_tParam.m_f3Rotate.y = atan2f(m_f3Velocity.x, m_f3Velocity.z);

	// 位置に速度を加算
	m_tParam.m_f3Pos += m_f3Velocity * fDeltaTime;
}

