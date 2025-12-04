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
	ImGui::SameLine();
	int maxRequestTool = MAX_REQUEST_TOOL[m_nBuildLevel - 1];
	ImGui::Text("(%d / %d)", static_cast<int>(m_vRequestToolList.size()), maxRequestTool);


	for (const auto& toolType : m_vRequestToolList)
	{
		ImGui::Separator();
		// 依頼道具タイプの取得
		std::string toolTypeStr = CItem::ITEM_TYPE_TO_STRING(toolType.eToolType);

		// 依頼状態の取得
		std::string requestStateStr = (toolType.eRequestState == TOOL_REQUEST_STATE::Unprocessed) ? "Unprocessed" : "InProcess";

		// 生産進行度の取得
		float productionProgress = toolType.fProductionProgress;

		// 依頼内容の表示
		ImGui::Text("Tool Type: %s", toolTypeStr.c_str());
		ImGui::Text("Request State: %s", requestStateStr.c_str());
		ImGui::Text("Production Progress: %.2f", productionProgress);
		ImGui::Separator();

		itemCount++;
	}


	return itemCount;
}

/*****************************************//*
	@brief　	| 生産依頼を追加可能かどうかを取得
	@return		| true:追加可能 false:追加不可
*//*****************************************/
bool CBlackSmith::CanAddRequestTool() const
{
	// 現在の依頼数が最大依頼数未満であれば追加可能
	return m_vRequestToolList.size() < MAX_REQUEST_TOOL[m_nBuildLevel - 1];
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
		if (toolType.eToolType == eToolType)
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

	// ツール生産依頼構造体の作成
	ToolRequest newRequest;
	newRequest.eRequestState = TOOL_REQUEST_STATE::Unprocessed;	// 依頼状態を未処理に設定
	newRequest.eToolType = eToolType;							// ツールのアイテムタイプを設定
	newRequest.fProductionProgress = 0.0f;						// 生産進行度を初期化

	// 依頼リストに追加
	m_vRequestToolList.push_back(newRequest);
}

/*****************************************//*
	@brief　	| 生産依頼を受ける
	@return		| ツール生産依頼構造体のポインタ、無ければnullptr
*//*****************************************/
CBlackSmith::ToolRequest* CBlackSmith::TakeRequestTool()
{
	// 依頼リストを走査
	for (auto& request : m_vRequestToolList)
	{
		// 未処理の依頼を探す
		if (request.eRequestState == TOOL_REQUEST_STATE::Unprocessed)
		{
			// 依頼状態を処理中に更新
			request.eRequestState = TOOL_REQUEST_STATE::InProcess;

			// 依頼構造体のポインタを返す
			return &request;
		}
	}

	return nullptr;
}

/*****************************************//*
	@brief　	| 生産依頼を未処理状態に設定
	@param		| pRequest：対象のツール生産依頼構造体のポインタ
*//*****************************************/
void CBlackSmith::ResetRequestTool(ToolRequest* pRequest)
{
	// 依頼状態を未処理に更新
	pRequest->eRequestState = TOOL_REQUEST_STATE::Unprocessed;
}

/*****************************************//*
	@brief　	| 生産依頼を進める
	@param		| In_Request：進行させるツール生産依頼構造体のポインタ
	@return		| 生産が完了した場合は生成したCItemポインタ、未完了の場合はnullptr
*//*****************************************/
CItem* CBlackSmith::ProgressRequestTool(ToolRequest* In_Request)
{
	// 生産進行度を進める
	In_Request->fProductionProgress += GetToolProductionProgressAmount();

	// 生産進行度が100以上になった場合、依頼を完了させる
	if (In_Request->fProductionProgress >= 100.0f)
	{
		// 新しいCItemインスタンスを生成して返す
		CItem* pNewItem = new CItem(In_Request->eToolType);

		// 依頼リストから該当の依頼を削除
		m_vRequestToolList.remove_if([In_Request](const ToolRequest& request)
		{
			return &request == In_Request;
		});

		return pNewItem;
	}
	// 生産が完了していなければnullptrを返す
	else
	{
		return nullptr;
	}

	return nullptr;
}
