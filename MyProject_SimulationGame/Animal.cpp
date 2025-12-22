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