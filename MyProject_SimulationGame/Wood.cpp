/**************************************************//*
	@file	| Wood.cpp
	@brief	| 木オブジェクトクラスのcppファイル
	@note	| CGameObjectを継承
*//**************************************************/
#include "Wood.h"
#include "SeasonManager.h"
#include "GoalManager.h"
#include "ModelRenderer.h"
#include "BillboardRenderer.h"
#include "Oparation.h"
#include "ShaderManager.h"

// 木の初期耐久値
constexpr float WOOD_INITIAL_HP = 50.0f;
//  ドロップアイテムの個数
constexpr int DROP_ITEM_COUNT = 3;

/*****************************************//*
	@brief　	| コンストラクタ
	@param　	| In_f3Pos：初期位置
*//*****************************************/
CWood::CWood()
	:CCollectTarget()
{
}

/*****************************************//*
	@brief　	| デストラクタ
*//*****************************************/
CWood::~CWood()
{
}

/*****************************************//*
	@brief　	| 初期化処理
*//*****************************************/
void CWood::Init()
{
	// 基底クラスの初期化処理
	CGameObject::Init();

	// モデルレンダラーコンポーネントの設定
	CModelRenderer* pModelRenderer = GetComponent<CModelRenderer>();
	pModelRenderer->SetKey("Wood");
	pModelRenderer->SetRendererParam(m_tParam);

	// 頂点シェーダーの設定
	VertexShader* pVS = CShaderManager::GetInstance()->GetVertexShader(VSType::Object);
	pModelRenderer->SetVertexShader(pVS);

	// ピクセルシェーダーの設定
	PixelShader* pPS = CShaderManager::GetInstance()->GetPixelShader(PSType::TexColor);
	pModelRenderer->SetPixelShader(pPS);

	// 木の耐久値設定
	m_Status.m_fHp = WOOD_INITIAL_HP;
	m_Status.m_fMaxHp = WOOD_INITIAL_HP;
	// ドロップアイテム設定
	for (int i = 0; i < DROP_ITEM_COUNT; ++i)
	{
		// 0~100のランダムな数値を取得
		int randomNum = GetRandOfRange(0, 100);

		if (randomNum < 50)
		{
			// 木材アイテムを追加
			m_Status.m_DropItems.push_back(new CItem(CItem::ITEM_TYPE::Wood));
		}
		else
		{
			// リンゴアイテムを追加
			m_Status.m_DropItems.push_back(new CItem(CItem::ITEM_TYPE::Apple));
		}
	}
}

/*****************************************//*
	@brief　	| ドロップアイテムを、その時の状況に合わせて取捨選択する処理
	@note		| 冬と大飢餓の間は木の実(リンゴ)が取れないため、ドロップアイテムからリンゴを除く
*//*****************************************/
void CWood::FilterDropItems()
{
	// (大飢餓の間は、季節に関係なく木の実が取れにくくなる)
	const bool bWinter = (CSeasonManager::GetSeason() == CSeasonManager::Season::Winter);
	if (!bWinter && !CGoalManager::IsFamine()) return;

	for (auto it = m_Status.m_DropItems.begin(); it != m_Status.m_DropItems.end();)
	{
		// 冬は全て取れない。大飢餓の間は、供給の割合の分だけ取れる
		const bool bLost = bWinter || (static_cast<float>(rand() % 100) >= CGoalManager::GetSupplyRate() * 100.0f);
		if ((*it)->GetItemType() == CItem::ITEM_TYPE::Apple && bLost)
		{
			delete* it;
			it = m_Status.m_DropItems.erase(it);
		}
		else
		{
			++it;
		}
	}
}
