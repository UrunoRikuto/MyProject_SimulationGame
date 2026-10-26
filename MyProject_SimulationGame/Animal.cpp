/**************************************************//*
	@file	| Animal.cpp
	@brief	| 動物基底クラスのcppファイル
	@note	| 動物の基底クラスを実装
			| CEntityを継承
*//**************************************************/
#include "Animal.h"
#include "ShaderManager.h"
#include "FieldManager.h"
#include "Main.h"
#include "CarnivorousAnimal.h"

/*****************************************//*
	@brief　	| コンストラクタ
*//*****************************************/
CAnimal::CAnimal()
	: CEntity()
	, m_f3Velocity({ 0.0f, 0.0f, 0.0f })
	, m_pActionAI(nullptr)
{
	// モデルレンダラーコンポーネントの追加
	CModelRenderer* pModelRenderer = AddComponent<CModelRenderer>();
	// 頂点シェーダーの設定
	VertexShader* pVS = CShaderManager::GetInstance()->GetVertexShader(VSType::Object);
	pModelRenderer->SetVertexShader(pVS);

	// ピクセルシェーダーの設定
	PixelShader* pPS = CShaderManager::GetInstance()->GetPixelShader(PSType::TexColor);
	pModelRenderer->SetPixelShader(pPS);

}

/*****************************************//*
	@brief　	| デストラクタ
*//*****************************************/
CAnimal::~CAnimal()
{
	// 体力が0以下なら攻撃されて死亡したので
	if (IsDead())
	{
		// 一定範囲内の肉食動物を取得
		std::list<CCarnivorousAnimal*> carnivorousAnimals;
		carnivorousAnimals = GetScene()->GetGameObjects<CCarnivorousAnimal>(m_tParam.m_f3Pos, 20.0f);
		// 取得した肉食動物全ての空腹度を回復させる
		for (CCarnivorousAnimal* pCarnivorousAnimal : carnivorousAnimals)
		{
			pCarnivorousAnimal->RecoverHunger(80.0f);
		}
	}

	// 動物の行動AIの解放
	SAFE_DELETE(m_pActionAI);

	// 登録しているセルの使用フラグを解除
	CFieldManager::GetInstance()->GetFieldGrid()->GetFieldCells()[m_n2BornCellIndex.x][m_n2BornCellIndex.y]->SetUse(false);
}

/****************************************//*
	@brief	| フィールドの外に出ないように位置と速度を補正する
	@note	| 端の少し内側までを行動範囲とし、端に着いた場合は外向きの速度を内向きに跳ね返らせる
*//****************************************/
void CAnimal::KeepInsideField()
{
	// 端から、この距離の内側までを行動範囲にする
	constexpr float Field_Edge_Margin = 4.0f;
	// 端で跳ね返る時の、速度の残り具合
	constexpr float Bounce_Rate = 0.5f;

	const float fLimitX = CFieldGrid::GetHalfExtentX() - Field_Edge_Margin;
	const float fLimitZ = CFieldGrid::GetHalfExtentZ() - Field_Edge_Margin;

	DirectX::XMFLOAT3& pos = m_tParam.m_f3Pos;
	if (pos.x > fLimitX)
	{
		pos.x = fLimitX;
		if (m_f3Velocity.x > 0.0f) m_f3Velocity.x = -m_f3Velocity.x * Bounce_Rate;
	}
	else if (pos.x < -fLimitX)
	{
		pos.x = -fLimitX;
		if (m_f3Velocity.x < 0.0f) m_f3Velocity.x = -m_f3Velocity.x * Bounce_Rate;
	}

	if (pos.z > fLimitZ)
	{
		pos.z = fLimitZ;
		if (m_f3Velocity.z > 0.0f) m_f3Velocity.z = -m_f3Velocity.z * Bounce_Rate;
	}
	else if (pos.z < -fLimitZ)
	{
		pos.z = -fLimitZ;
		if (m_f3Velocity.z < 0.0f) m_f3Velocity.z = -m_f3Velocity.z * Bounce_Rate;
	}
}
