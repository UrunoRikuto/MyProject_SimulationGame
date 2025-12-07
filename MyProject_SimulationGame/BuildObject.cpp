/**************************************************//*
	@file	| BuildObject.cpp
	@brief	| 建築オブジェクトクラスのcppファイル
	@note	| 建築オブジェクトの処理を定義
			| CGameObjectを継承
*//**************************************************/
#include "BuildObject.h"
#include "ModelRenderer.h"
#include "ImguiSystem.h"

/****************************************//*
	@brief　	| コンストラクタ
*//****************************************/
CBuildObject::CBuildObject()
	:CGameObject()
	, m_nBuildLevel(1)
	, m_fBuildProgress(0.0f)
	, m_n2FieldCellIndex({ -1, -1 })
{
	// 耐久値初期化
	m_pBuildProgressBillboard = new CBillboardRenderer(this);
	m_pBuildProgressBillboard->SetKey("Bar_Gauge");

	// モデルコンポーネントの追加
	AddComponent<CModelRenderer>();
}

/****************************************//* 
	@brief　	| デストラクタ
*//****************************************/
CBuildObject::~CBuildObject()
{
}

/****************************************//* 
	@brief　	| 初期化処理
*//****************************************/
void CBuildObject::Init()
{
	// 親クラスの初期化処理を呼び出す
	CGameObject::Init();
}

/*****************************************//*
	@brief　	| 描画処理
*//*****************************************/
void CBuildObject::Draw()
{
	// 親クラスの描画処理を呼び出す
	CGameObject::Draw();

	// 建築進行度の割合を計算
	float BuildProgressRatio = m_fBuildProgress / BUILD_PROGRESS_MAX;
	// 建築進行度が100%未満の場合は、建築進行度を表示する
	if (BuildProgressRatio < 1.0f)
	{
		// 建築進行度おビルボードの描画
		m_pBuildProgressBillboard->SetPos({ m_tParam.m_f3Pos.x - (m_tParam.m_f3Size.x * (1.0f - BuildProgressRatio)), m_tParam.m_f3Pos.y + 2.0f, m_tParam.m_f3Pos.z });
		m_pBuildProgressBillboard->SetSize({ 2.0f * BuildProgressRatio, 0.2f, 1.0f });
		m_pBuildProgressBillboard->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		m_pBuildProgressBillboard->SetUVPos({ 0.0f, 0.0f });
		m_pBuildProgressBillboard->SetUVSize({ 1.0f, 1.0f });
		m_pBuildProgressBillboard->SetRotation({ 0.0f, 0.0f, 0.0f });
		m_pBuildProgressBillboard->SetColor({ 0.0f, 1.0f, 0.0f, 1.0f });
		m_pBuildProgressBillboard->SetCullingMode(D3D11_CULL_FRONT);
		m_pBuildProgressBillboard->Draw();
	}
}

/*****************************************//*
	@brief　	| インスペクター表示処理
	@param		| isEnd：true:ImGuiのEnd()を呼ぶ false:呼ばない
	@return		| 表示した項目数
	@note　　　	| ImGuiを使用してオブジェクトのパラメータを表示、編集する
*//*****************************************/
int CBuildObject::Inspecter()
{
	int itemCount = 0;

	// 建築物レベルの表示と編集
	ImGui::Text(std::string("Build Level:"+ std::to_string(m_nBuildLevel)).c_str());

	return itemCount;
}

/*****************************************//*
	@brief　	| 建築物レベルを進める
*//*****************************************/
void CBuildObject::UpgradeBuildLevel()
{
	if (!IsMaxBuildLevel())
	{
		m_nBuildLevel++;
	}
}
/*****************************************//*
	@brief　	| 建築完成度を進める
	@param		| fAmount：一秒間で進める量
*//*****************************************/
void CBuildObject::ProgressBuild(float fAmount)
{
	m_fBuildProgress += fAmount;

	if (m_fBuildProgress > BUILD_PROGRESS_MAX)
	{
		m_fBuildProgress = BUILD_PROGRESS_MAX;
	}
}
