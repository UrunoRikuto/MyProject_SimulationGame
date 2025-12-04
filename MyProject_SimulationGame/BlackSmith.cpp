/**************************************************//*
	@file	| BlackSmith.cpp
	@brief	| 鍛冶屋クラスのcppファイル
	@note	| 鍛冶屋オブジェクトの処理を実装
			| CBuildObjectを継承
*//**************************************************/
#include "BlackSmith.h"
#include "ModelRenderer.h"
#include "ShaderManager.h"
#include "ImguiSystem.h"

/*****************************************//*
	@brief　	| コンストラクタ
*//*****************************************/
CBlackSmith::CBlackSmith()
	:CBuildObject()
{
	// モデルレンダラーコンポーネントの追加
	AddComponent<CModelRenderer>();
}

/*****************************************//*
	@brief　	| デストラクタ
*//*****************************************/
CBlackSmith::~CBlackSmith()
{
}

/*****************************************//*
	@brief　	| 初期化処理
*//*****************************************/
void CBlackSmith::Init()
{
	// 基底クラスの初期化処理
	CBuildObject::Init();
	// モデルレンダラーコンポーネントの設定
	CModelRenderer* pModelRenderer = GetComponent<CModelRenderer>();
	pModelRenderer->SetKey("BlackSmith");
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
	@return		| 表示した項目数
	@note　　　	| ImGuiを使用してオブジェクトのパラメータを表示、編集する
*//*****************************************/
int CBlackSmith::Inspecter()
{
	int itemCount = 0;

	// 基底クラスのインスペクター表示処理
	itemCount += CBuildObject::Inspecter();

	// 生産依頼リストの表示
	ImGui::Text("Request Tool List:");
	for (const auto& toolType : m_vRequestToolList)
	{
		ImGui::BulletText("%s", CItem::ITEM_TYPE_TO_STRING(toolType).c_str());
		itemCount++;
	}


	return itemCount;
}

/*****************************************//*
	@brief　	| 指定の道具の生産依頼があるかどうかを取得
	@param		| eToolType：道具のアイテムタイプ
	@return		| true:依頼がある false:依頼がない
*//*****************************************/
bool CBlackSmith::HasRequestTool(CItem::ITEM_TYPE eToolType) const
{
	// 依頼リストを走査
	for (const auto& toolType : m_vRequestToolList)
	{
		// 指定の道具タイプが見つかった場合はtrueを返す
		if (toolType == eToolType)
		{
			return true;
		}
	}

	// 見つからなかった場合はfalseを返す
	return false;
}

/*****************************************//*
	@brief　	| 生産依頼の追加
	@param		| eToolType：依頼する道具の種類
*//*****************************************/
void CBlackSmith::AddRequestTool(CItem::ITEM_TYPE eToolType)
{
	// アイテムカテゴリーが道具であるか確認
	// 道具でなければ処理を抜ける
	if(CItem::GetItemCategoryFromType(eToolType) != CItem::ITEM_CATEGORY::Tool)return;

	// 依頼リストに追加
	m_vRequestToolList.push_back(eToolType);
}
