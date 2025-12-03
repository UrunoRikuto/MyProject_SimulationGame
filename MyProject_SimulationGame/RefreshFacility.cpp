/**************************************************//*
	@file	| RefreshFacility.cpp
	@brief	| 休憩施設クラスのcppファイル
	@note	| 休憩施設の処理を実装
			| CBuildObjectを継承
*//**************************************************/
#include "RefreshFacility.h"
#include "ModelRenderer.h"
#include "ImguiSystem.h"
#include "ShaderManager.h"

/*****************************************//*
	@brief　	| コンストラクタ
*//*****************************************/
CRefreshFacility::CRefreshFacility()
	:CBuildObject()
{
	// モデルレンダラーコンポーネントの追加
	AddComponent<CModelRenderer>();
}

/*****************************************//*
	@brief　	| デストラクタ
*//*****************************************/
CRefreshFacility::~CRefreshFacility()
{
}

/*****************************************//*
	@brief　	| 初期化処理
*//*****************************************/
void CRefreshFacility::Init()
{
	// 基底クラスの初期化処理
	CBuildObject::Init();

	// モデルレンダラーコンポーネントの設定
	CModelRenderer* pModelRenderer = GetComponent<CModelRenderer>();
	pModelRenderer->SetKey("RefreshFacility");
	pModelRenderer->SetRendererParam(m_tParam);

	// 頂点シェーダーの設定
	VertexShader* pVS = CShaderManager::GetInstance()->GetVertexShader(VSType::Object);
	pModelRenderer->SetVertexShader(pVS);

	// ピクセルシェーダーの設定
	PixelShader* pPS = CShaderManager::GetInstance()->GetPixelShader(PSType::TexColor);
	pModelRenderer->SetPixelShader(pPS);
}

/*****************************************//*
	@brief　	| インスペクター表示処理
	@param		| isEnd：true:ImGuiのEnd()を呼ぶ false:呼ばない
	@return		| 表示した項目数
	@note　　　	| ImGuiを使用してオブジェクトのパラメータを表示、編集する
*//*****************************************/
int CRefreshFacility::Inspecter(bool isEnd)
{
	int itemCount = 0;

#ifdef _DEBUG
	// 基底クラスのインスペクター表示処理
	itemCount += CBuildObject::Inspecter(false);
#endif

	// 使用中の人間数の表示
	ImGui::Text("Using Humans: %d / %d", static_cast<int>(m_pUsingHumans.size()), MAX_USING_HUMANS[m_nBuildLevel - 1]);

	itemCount++;

	// IMGUIウィンドウの終了
	if (isEnd)
	{
		// 子要素の終了
		ImGui::EndChild();
		ImGui::End();
		itemCount++;
	}
	return itemCount;
}

/*****************************************//*
	@brief　	| 休憩施設を使用可能かどうか
	@return		| true:使用可能 false:使用不可
*//*****************************************/
bool CRefreshFacility::CanUseRefreshFacility() const
{
	// 使用中の人間数が最大使用可能人数未満かどうかを返す
	return static_cast<int>(m_pUsingHumans.size()) < MAX_USING_HUMANS[m_nBuildLevel - 1];
}

/*****************************************//*
	@brief　	| 休憩所の使用
	@param		| pHuman：使用する人間のポインタ
*//*****************************************/
void CRefreshFacility::UseRefreshFacility(CHuman& pHuman)
{
	// 既に使用中の場合は何もしない
	for (CHuman* pUsingHuman : m_pUsingHumans)
	{
		if (pUsingHuman == &pHuman)
		{
			return;
		}
	}

	// 使用中の人間リストに追加
	m_pUsingHumans.push_back(&pHuman);
}

/*****************************************//*
	@brief　	| 休憩所を使用しているかどうか
	@param		| pHuman：確認する人間のポインタ
	@return		| true:使用中 false:未使用
*//*****************************************/
bool CRefreshFacility::IsUsingRefreshFacility(CHuman& pHuman) const
{
	// 使用中の人間リストを確認
	for (CHuman* pUsingHuman : m_pUsingHumans)
	{
		if (pUsingHuman == &pHuman)
		{
			return true;
		}
	}

	return false;
}

/*****************************************//*
	@brief　	| 休憩所の使用解除
	@param		| pHuman：使用を解除する人間のポインタ
*//*****************************************/
void CRefreshFacility::ReleaseRefreshFacility(CHuman& pHuman)
{
	// 使用中の人間リストが空の場合は何もしない
	if (m_pUsingHumans.empty())return;

	// 使用中の人間リストから削除
	m_pUsingHumans.erase(std::remove(m_pUsingHumans.begin(), m_pUsingHumans.end(), &pHuman), m_pUsingHumans.end());
}