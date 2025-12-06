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

/*****************************************//*
	@brief　	| コンストラクタ
*//*****************************************/
CFoodFactory::CFoodFactory()
	:CBuildObject()
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
	if (m_vCookRequestList.size() >= MAX_COOK_REQUESTS[GetBuildLevel() - 1]) return;

	// 製作可能な料理のアイテムタイプを探索
	CItem::ITEM_TYPE eMealType = FindProducibleMealType();

	// 製作可能な料理が見つかった場合は調理依頼を追加
	if (eMealType != CItem::ITEM_TYPE::MAX)
	{
		// 調理依頼の追加
		AddCookRequest(eMealType);
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
	ImGui::Text("Cook Request List:");
	ImGui::SameLine();
	int maxCookRequests = MAX_COOK_REQUESTS[m_nBuildLevel - 1];
	ImGui::Text("(%d / %d)", static_cast<int>(m_vCookRequestList.size()), maxCookRequests);
	ImGui::BeginChild("CookRequestList", ImVec2(0, 150), true);

	// 調理依頼リストを走査して表示
	for(const auto& cookRequest : m_vCookRequestList)
	{
		// 料理のアイテムタイプ名の取得
		std::string mealTypeName = CItem::ITEM_TYPE_TO_STRING(cookRequest.eMealType);

		// 調理進行度の表示
		ImGui::Text("- %s: %.1f%%", mealTypeName.c_str(), cookRequest.fCookingProgress);
	}

	ImGui::EndChild();
	itemCount++;

	return itemCount;
}

/*****************************************//*
	@brief　	| 調理依頼の追加
	@param		| eMealType：食事のアイテムタイプ
*//*****************************************/
void CFoodFactory::AddCookRequest(CItem::ITEM_TYPE eMealType)
{
	// 調理依頼リストに追加
	m_vCookRequestList.push_back({ eMealType, 0.0f });
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

	// アップルパイ
	vCanCookItems.push_back(CItem::ITEM_TYPE::ApplePie);

	// 砂糖
	vCanCookItems.push_back(CItem::ITEM_TYPE::Sugar);

	// 製作可能な料理のアイテムタイプリストを返す
	return vCanCookItems;
}

/*****************************************//*
	@brief　	| 調理依頼を進める
	@param		| fAmount：進める量
*//*****************************************/
void CFoodFactory::ProgressCookRequests(float fAmount)
{
	// 調理依頼リストを走査
	for (auto& cookRequest : m_vCookRequestList)
	{
		// 調理進行度を進める
		cookRequest.fCookingProgress += fAmount;

		// 調理進行度が100%以上になった場合は調理完了とする
		if (cookRequest.fCookingProgress >= 100.0f)
		{
			cookRequest.fCookingProgress = 100.0f;
		}
	}
}
