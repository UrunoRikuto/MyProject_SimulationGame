/**************************************************//*
	@file	| Grass.cpp
	@brief	| 草オブジェクトクラスのcppファイル
	@note	| CGameObjectを継承
*//**************************************************/
#include "Grass.h"
#include "ShaderManager.h"
#include "Oparation.h"

// 草の初期耐久値
constexpr float GRASS_INITIAL_HP = 20.0f;
//  ドロップアイテムの個数
constexpr int DROP_ITEM_COUNT = 5;


/*****************************************//*
	@brief	| コンストラクタ
*//*****************************************/
CGrass::CGrass()
{
}

/*****************************************//*
	@brief	| デストラクタ
*//*****************************************/
CGrass::~CGrass()
{
}

/*****************************************//*
	@brief	| 初期化処理
*//*****************************************/
void CGrass::Init()
{
	// 親クラスの初期化処理を呼び出す
	CCollectTarget::Init();

	// モデルレンダラーコンポーネントの設定
	CModelRenderer* pModelRenderer = GetComponent<CModelRenderer>();
	pModelRenderer->SetKey("Grass");
	pModelRenderer->SetRendererParam(m_tParam);

	// 頂点シェーダーの設定
	VertexShader* pVS = CShaderManager::GetInstance()->GetVertexShader(VSType::Object);
	pModelRenderer->SetVertexShader(pVS);

	// ピクセルシェーダーの設定
	PixelShader* pPS = CShaderManager::GetInstance()->GetPixelShader(PSType::TexColor);
	pModelRenderer->SetPixelShader(pPS);


	// 収集対象のステータスを設定
	m_Status.m_fMaxHp = GRASS_INITIAL_HP;
	m_Status.m_fHp = m_Status.m_fMaxHp;
	// ドロップアイテム設定
	for(int i = 0; i < DROP_ITEM_COUNT; ++i)
	{
		// 0~100のランダムな数値を取得
		int randomNum = GetRandOfRange(0, 100);

		// 50%の確率で麦の種をドロップ
		if(randomNum < 50)
		{
			m_Status.m_DropItems.push_back(new(std::nothrow) CItem(CItem::ITEM_TYPE::WheatSeed));
		}
		// 50%の確率で繊維をドロップ
		else
		{
			m_Status.m_DropItems.push_back(new(std::nothrow) CItem(CItem::ITEM_TYPE::Fiber));
		}
	}

}