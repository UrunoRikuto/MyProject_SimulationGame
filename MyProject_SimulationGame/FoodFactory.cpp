/**************************************************//*
	@file	| FoodFactory.cpp
	@brief	| 食料加工施設クラス実装
	@note	| 調理済みの食事を作成する建築物
			| CBuildObjectを継承
*//**************************************************/
#include "FoodFactory.h"
#include "ModelRenderer.h"
#include "ShaderManager.h"
#include "Main.h"
#include "StorageHouse.h"
#include "Item_Material.h"
#include "ImguiSystem.h"
#include "BuildManager.h"

/*****************************************//*
	@brief　	| コンストラクタ
*//*****************************************/
CFoodFactory::CFoodFactory()
	:CBuildObject()
	, m_vCookRequestList()
	, m_fBuildXP(0.0f)
{
}

/*****************************************//*
	@brief　	| デストラクタ
*//*****************************************/
CFoodFactory::~CFoodFactory()
{
}

/*****************************************//*
	@brief　	| 初期化処理
*//*****************************************/
void CFoodFactory::Init()
{
	// 基底クラスの初期化処理
	CBuildObject::Init();

	// モデルレンダラーコンポーネントの設定
	CModelRenderer* pModelRenderer = GetComponent<CModelRenderer>();
	pModelRenderer->SetKey("FoodFactory");

	// シェーダーマネージャーの取得
	CShaderManager* pShaderManager = CShaderManager::GetInstance();

	// 頂点シェーダーの設定
	pModelRenderer->SetVertexShader(pShaderManager->GetVertexShader(VSType::Object));

	// ピクセルシェーダーの設定
	pModelRenderer->SetPixelShader(pShaderManager->GetPixelShader(PSType::TexColor));

}

/*****************************************//*
	@brief　	| 更新処理
*//*****************************************/
void CFoodFactory::Update()
{
	// 基底クラスの更新処理
	CBuildObject::Update();

	// 調理依頼がいっぱいの場合は何もしない
	if (m_vCookRequestList.size() >= MAX_COOK_REQUESTS[m_nBuildLevel - 1]) return;

	// 製作可能な料理のアイテムタイプを探索
	CItem::ITEM_TYPE eMealType = FindProducibleMealType();

	// 製作可能な料理が見つかった場合は調理依頼を追加
	if (eMealType != CItem::ITEM_TYPE::MAX)
	{
		// 調理依頼の追加
		AddCookRequest(eMealType);
	}

	// 現在の建築レベルが最大レベル未満で、建築経験値が100以上の場合
	if (GetBuildLevel() < CBuildObject::MAX_BUILD_LEVEL && m_fBuildXP >= 100.0f)
	{
		// 建築依頼を出す
		CBuildManager::GetInstance()->AddBuildRequest(CBuildManager::BuildType::FoodFactory);

		// 建築経験値を100減少
		m_fBuildXP -= 100.0f;
	}
}

/*****************************************//*
	@brief　	| インスペクター表示処理
	@return		| 表示した項目数
	@note　　　	| ImGuiを使用してオブジェクトのパラメータを表示、編集する
*//*****************************************/
int CFoodFactory::Inspecter()
{
	int itemCount = 0;

	// 基底クラスのインスペクター表示処理
	itemCount += CBuildObject::Inspecter();

	// 調理依頼リストの表示
	ImGui::Text(u8"調理依頼:");
	ImGui::SameLine();
	int maxCookRequests = MAX_COOK_REQUESTS[m_nBuildLevel - 1];
	ImGui::Text("(%d / %d)", static_cast<int>(m_vCookRequestList.size()), maxCookRequests);
	ImGui::BeginChild(u8"CookRequestList", ImVec2(0, 150), true);

	// 調理依頼リストを走査して表示
	for(const auto& cookRequest : m_vCookRequestList)
	{
		// 料理のアイテムタイプ名の取得
		std::string mealTypeName = CItem::ITEM_TYPE_TO_STRING(cookRequest.eMealType);

		// 依頼状態の表示
		std::string requestStateName;
		switch (cookRequest.eRequestState)
		{
			case REQUEST_STATE::Unprocessed:
			requestStateName = u8"未受注";
			break;
			case REQUEST_STATE::InProcess:
				requestStateName = u8"受注済み";
				break;
			default:
				requestStateName = u8"Unknown";
				break;
		}
		ImGui::Text(requestStateName.c_str());
		ImGui::SameLine();
		// 調理進行度の表示
		ImGui::Text(u8":- %s: %.1f%%", mealTypeName.c_str(), cookRequest.fCookingProgress);
	}

	ImGui::EndChild();
	itemCount++;

	return itemCount;
}

/*****************************************//*
	@brief　	| 調理依頼を受ける
	@return		| 調理依頼構造体のポインタ、なければnullptr
*//*****************************************/
CFoodFactory::CookRequest* CFoodFactory::TakeRequest()
{
	// 依頼リストを走査
	for (auto& request : m_vCookRequestList)
	{
		// 未処理の依頼が存在する場合、その依頼を処理中に設定してポインタを返す
		if (request.eRequestState == REQUEST_STATE::Unprocessed)
		{
			request.eRequestState = REQUEST_STATE::InProcess;
			return &request;
		}
	}

	// 未処理の依頼が存在しなかった場合はnullptrを返す
	return nullptr;
}

/*****************************************//*
	@brief　	| 指定された料理の生産依頼が存在するか確認
	@param		| eMealType：料理のアイテムタイプ
	@return		| true:存在する false:存在しない
*//*****************************************/
bool CFoodFactory::HasRequest(CItem::ITEM_TYPE eMealType) const
{
	// 依頼リストを走査
	for (const auto& request : m_vCookRequestList)
	{
		// 指定された料理の依頼が存在する場合はtrueを返す
		if (request.eMealType == eMealType)
		{
			return true;
		}
	}

	return false;
}

/*****************************************//*
	@brief　	| 生産依頼を未処理状態に設定
	@param		| pRequest：ツール生産依頼構造体のポインタ
*//*****************************************/
void CFoodFactory::ResetRequest(CookRequest* pRequest)
{
	if (pRequest == nullptr) return;

	// 依頼状態を未処理に設定
	pRequest->eRequestState = REQUEST_STATE::Unprocessed;

	// 調理進行度を0に設定
	pRequest->fCookingProgress = 0.0f;
}

/*****************************************//*
	@brief　	| 生産依頼の完了報告
	@param		| pRequest：ツール生産依頼構造体のポインタ
	@return		| true:完了報告成功 false:完了報告失敗
*//*****************************************/
bool CFoodFactory::CompleteRequest(CookRequest* pRequest)
{
	if (pRequest == nullptr) return false;

	// 依頼リストから削除できなかった場合、もしくは生産が完了していない場合は削除しない
	m_vCookRequestList.remove_if([pRequest](const CookRequest& request) {
		return (&request == pRequest) && pRequest->fCookingProgress >= 100.0f;
		});

	// 正常に削除された場合はtrueを返す
	if (!HasRequest(pRequest->eMealType))
	{
		if(GetBuildLevel() < CBuildObject::MAX_BUILD_LEVEL)
		{
			// 建築経験値を加算
			m_fBuildXP += BUILD_XP_AMOUNT;
		}

		return true;
	}

	// 見つからなかった場合はfalseを返す
	return false;
}

/*****************************************//*
	@brief　	| 生産依頼を進める
	@param		| In_Request：ツール生産依頼構造体のポインタ
	@return		| 生産が完了した場合は生成したCItemポインタ、未完了の場合はnullptr
*//*****************************************/
std::vector<CItem*> CFoodFactory::ProgressRequest(CookRequest* pRequest)
{
	std::vector<CItem*> pFinishedItems;

	if (pRequest == nullptr) return pFinishedItems;

	// 生産進行度を進める
	pRequest->fCookingProgress += GetCookProductionProgressAmount();

	// 生産進行度が100以上になった場合、依頼を完了させる
	if (pRequest->fCookingProgress >= 100.0f)
	{
		// 新しいCItemインスタンスを生成して返す
		pFinishedItems = CookMaterials::GetFinishedMealItems(pRequest->eMealType);
	}

	// 生産が完了していなければnullptrを返す
	return pFinishedItems;
}

/*****************************************//*
	@brief　	| 調理依頼の追加
	@param		| eMealType：食事のアイテムタイプ
*//*****************************************/
void CFoodFactory::AddCookRequest(CItem::ITEM_TYPE eMealType)
{
	// 調理依頼リストに追加
	m_vCookRequestList.push_back({ REQUEST_STATE::Unprocessed, eMealType, 0.0f });
}

/*****************************************//*
	@brief　	| 製作可能な料理を探索
	@return		| 見つかった場合は料理のアイテムタイプ、見つからなかった場合はMAXを返す
*//*****************************************/
CItem::ITEM_TYPE CFoodFactory::FindProducibleMealType()
{
	// 見つかった料理のアイテムタイプを格納する変数
	CItem::ITEM_TYPE eType = CItem::ITEM_TYPE::MAX;

	// 貯蔵庫の取得
	CStorageHouse* pStorageHouse = GetScene()->GetGameObject<CStorageHouse>();

	// 製作可能な料理のアイテムタイプリストを取得
	std::list<CItem::ITEM_TYPE> producibleMealTypes = GetProducibleMealTypes();

	// 製作可能な料理のアイテムタイプリストを走査
	for (const auto& mealType : producibleMealTypes)
	{
		// 料理の必要材料リストを取得
		auto reqests = CookMaterials::GetCookMaterials(mealType);

		bool bCanCook = true;

		// 必要材料が全て揃っているか確認
		for (const auto& request : reqests)
		{
			// 貯蔵庫に必要材料が足りない場合は調理不可とする
			if (!pStorageHouse->HasItemType(request.eItemType))
			{
				bCanCook = false;
				break;
			}
		}

		// 必要材料が全て揃っていた場合は料理のアイテムタイプを返す
		if (bCanCook)
		{
			// 見つかった料理のアイテムタイプを格納
			eType = mealType;
			// ループを抜ける
			break;
		}
	}

	// 見つかった料理のアイテムタイプを返す
	return eType;
}

/*****************************************//*
	@brief　	| 製作可能な料理のアイテムタイプリストを取得
	@return		| 製作可能な料理のアイテムタイプリスト
	@note 　　	| 包丁などの道具を解放したら追加する料理などをここで判別
				| リストに入れる順番 = 優先的に作る料理の順番
				| 料理 -> 材料の順番でリストに入れること
*//*****************************************/
std::list<CItem::ITEM_TYPE> CFoodFactory::GetProducibleMealTypes()
{
	// 製作可能な料理のアイテムタイプリスト
	std::list<CItem::ITEM_TYPE> vCanCookItems;

	// パン
	vCanCookItems.push_back(CItem::ITEM_TYPE::Bread);

	// アップルパイ
	vCanCookItems.push_back(CItem::ITEM_TYPE::ApplePie);

	// 砂糖
	vCanCookItems.push_back(CItem::ITEM_TYPE::Sugar);

	// 製作可能な料理のアイテムタイプリストを返す
	return vCanCookItems;
}
