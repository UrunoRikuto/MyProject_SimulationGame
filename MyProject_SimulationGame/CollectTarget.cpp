/**************************************************//*
	@file	| CollectTarget.cpp
	@brief	| 収集対象オブジェクトクラスのcppファイル
	@note	| CGameObjectを継承
*//**************************************************/
#include "CollectTarget.h"

/*****************************************//*
	@brief　	| コンストラクタ
*//*****************************************/
CCollectTarget::CCollectTarget()
	: m_TargetingID({ "", -1 })
	, m_LinkedCellIndex({ -1, -1 })
	, m_Status()
	, m_pHpBillboard{ nullptr }
{
	// 耐久値初期化
	m_pHpBillboard = new CBillboardRenderer(this);
	m_pHpBillboard->SetKey("Bar_Gauge");
}

/*****************************************//*
	@brief　	| デストラクタ
*//*****************************************/
CCollectTarget::~CCollectTarget()
{
}
/*****************************************//*
	@brief　	| 描画処理
*//*****************************************/
void CCollectTarget::Draw()
{
	// 基底クラスの描画処理
	CGameObject::Draw();

	// 耐久値表示
	float hpRatio = m_Status.m_fHp / m_Status.m_fMaxHp;

	// 耐久値が最大値未満の場合のみ表示
	if (hpRatio < 1.0f)
	{
		// 耐久値ビルボードの描画
		// 前景
		m_pHpBillboard->SetPos({ m_tParam.m_f3Pos.x - (m_tParam.m_f3Size.x * (1.0f - hpRatio)), m_tParam.m_f3Pos.y + 2.0f, m_tParam.m_f3Pos.z });
		m_pHpBillboard->SetSize({ 2.0f * hpRatio, 0.2f, 1.0f });
		m_pHpBillboard->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		m_pHpBillboard->SetUVPos({ 0.0f, 0.0f });
		m_pHpBillboard->SetUVSize({ 1.0f, 1.0f });
		m_pHpBillboard->SetRotation({ 0.0f, 0.0f, 0.0f });
		m_pHpBillboard->SetColor({ 0.0f, 1.0f, 0.0f, 1.0f });
		m_pHpBillboard->SetCullingMode(D3D11_CULL_FRONT);
		m_pHpBillboard->Draw();
	}
}

/*****************************************//*
	@brief　	| 生成位置設定
	@param　	| cell：連携するフィールドセルのポインタ
*//*****************************************/
void CCollectTarget::SetCreatePos(CFieldCell* cell)
{
	// 位置設定
	m_tParam.m_f3Pos = cell->GetPos();

	// セルを使用中に設定
	cell->SetUse(true);
	// 連携しているフィールドセルのインデックスを保存
	m_LinkedCellIndex = cell->GetIndex();
}

/*****************************************//*
	@brief　	| オブジェクトが破棄された時の処理
*//*****************************************/
void CCollectTarget::OnDestroy()
{
	// 連携しているフィールドセルの使用状態を解除
	CFieldCell* cell = CFieldManager::GetInstance()->GetFieldGrid()->GetFieldCells()[(int)m_LinkedCellIndex.y][(int)m_LinkedCellIndex.x];
	cell->SetUse(false);

	// 基底クラスのオブジェクト破棄時の処理
	CGameObject::OnDestroy();
}
/*****************************************//*
	@brief　	| 耐久地を減らす
	@param　	| damage：減らす耐久値
*//*****************************************/
void CCollectTarget::DecreaseHp(const float damage)
{
	m_Status.m_fHp -= damage;
}
