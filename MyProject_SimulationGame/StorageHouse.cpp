/**************************************************//*
	@file	| StorageHouse.cpp
	@brief	| 貯蔵庫クラスのcppファイル
	@note	| 貯蔵庫の処理を定義
			| CBuildObjectを継承
*//**************************************************/
#include "StorageHouse.h"
#include "ModelRenderer.h"
#include "ImguiSystem.h"
#include "ShaderManager.h"

/*****************************************//*
	@brief　	| コンストラクタ
*//*****************************************/
CStorageHouse::CStorageHouse()
	:CBuildObject()
{
	// モデルレンダラーコンポーネントの追加
	AddComponent<CModelRenderer>();
}

/*****************************************//*
	@brief　	| デストラクタ
*//*****************************************/
CStorageHouse::~CStorageHouse()
{
}

/*****************************************//*
	@brief　	| 初期化処理
*//*****************************************/
void CStorageHouse::Init()
{
	// 基底クラスの初期化処理
	CBuildObject::Init();

	// モデルレンダラーコンポーネントの設定
	CModelRenderer* pModelRenderer = GetComponent<CModelRenderer>();
	pModelRenderer->SetKey("StorageHouse");
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
int CStorageHouse::Inspecter()
{
	// 基底クラスのインスペクター表示処理
	int itemCount = CBuildObject::Inspecter();

	// 収納されているアイテムを種類別にカウント
	std::map<CItem::ITEM_TYPE, int> itemTypes;
	for (CItem* pItem : m_StoredItems)
	{
		CItem::ITEM_TYPE type = pItem->GetItemType();
		itemTypes[type]++;
	}

	// アイテム種類ごとに表示
	ImGui::BeginChildFrame(ImGui::GetID((void*)0), ImVec2(280, 200));

	for (const auto& pair : itemTypes)
	{
		// アイテム名の取得
		std::string itemName = CItem::ITEM_TYPE_TO_STRING(pair.first);
		// アイテム名と数量の表示
		ImGui::Text(std::string(itemName + ": " + std::to_string(pair.second)).c_str());
	}

	ImGui::EndChildFrame();

	// 表示した項目数を返す
	return itemCount;
}

/*****************************************//*
	@brief　	| アイテムの収納
	@param		| pItem：収納するアイテム
*//*****************************************/
void CStorageHouse::StoreItem(CItem* pItem)
{
	// 収納されているアイテムリストに追加
	m_StoredItems.push_back(pItem);
}

/*****************************************//*
	@brief　	| 収納されているアイテムの取り出し
	@param		| eType：取り出すアイテムのタイプ
	@return		| 取り出したアイテムのポインタ、存在しない場合はnullptr
*//*****************************************/
CItem* CStorageHouse::TakeOutItem(CItem::ITEM_TYPE eType)
{
	// 収納されているアイテムリストを探索
	for (auto it = m_StoredItems.begin(); it != m_StoredItems.end(); ++it)
	{
		// 指定されたタイプのアイテムを探索
		if ((*it)->GetItemType() == eType)
		{
			// アイテムを取り出す
			CItem* pItem = *it;
			// 収納されているアイテムリストから削除
			m_StoredItems.erase(it);
			// 取り出したアイテムポインタを返す
			return pItem;
		}
	}

	// 指定されたタイプのアイテムが存在しなかった場合はnullptrを返す
	return nullptr;
}

/*****************************************//*
	@brief　	| 収納されているアイテムの取り出し
	@param		| eCategory：取り出すアイテムのカテゴリー
	@return		| 取り出したアイテムのポインタ、存在しない場合はnullptr
*//*****************************************/
CItem* CStorageHouse::TakeOutItem(CItem::ITEM_CATEGORY eCategory)
{
	// 収納されているアイテムリストを探索
	for (auto it = m_StoredItems.begin(); it != m_StoredItems.end(); ++it)
	{
		// 指定されたカテゴリーのアイテムを探索
		if ((*it)->GetItemCategory() == eCategory)
		{
			// アイテムを取り出す
			CItem* pItem = *it;
			// 収納されているアイテムリストから削除
			m_StoredItems.erase(it);
			// 取り出したアイテムポインタを返す
			return pItem;
		}
	}

	return nullptr;
}

/*****************************************//*
	@brief　	| 食料が存在するかどうかを取得
	@return		| true:存在する false:存在しない
*//*****************************************/
bool CStorageHouse::HasFood() const
{
	// 収納されているアイテムリストを探索
	for (const CItem* pItem : m_StoredItems)
	{
		// 食料カテゴリーのアイテムが存在するかどうかをチェック
		if (pItem->GetItemCategory() == CItem::ITEM_CATEGORY::CookedFood ||
			pItem->GetItemCategory() == CItem::ITEM_CATEGORY::UnCookedFood)
		{
			return true;
		}
	}

	// 食料が存在しなかった場合はfalseを返す
	return false;
}