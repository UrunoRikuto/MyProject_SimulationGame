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
	@param		| isEnd：true:ImGuiのEnd()を呼ぶ false:呼ばない
	@return		| 表示した項目数
	@note　　　	| ImGuiを使用してオブジェクトのパラメータを表示、編集する
*//*****************************************/
int CStorageHouse::Inspecter(bool isEnd)
{
	// 基底クラスのインスペクター表示処理
	int itemCount = CBuildObject::Inspecter(false);

	std::string ItemCount = std::to_string(static_cast<int>(m_StoredItems.size()));

	if (ImGui::TreeNode(std::string("Stored Items: " + ItemCount).c_str()))
	{

		// 収納されているアイテムを種類別にカウント
		std::map<CItem::ITEM_TYPE,int> itemTypes;
		for (CItem* pItem : m_StoredItems)
		{
			CItem::ITEM_TYPE type = pItem->GetItemType();
			itemTypes[type]++;
		}

		// アイテムの種類ごとに表示
		for (const auto& pair : itemTypes)
		{
			CItem::ITEM_TYPE type = pair.first;
			int count = pair.second;
			ImGui::Text("%s: %d", CItem::ITEM_TYPE_TO_STRING(type).c_str(), count);
			itemCount++;
		}
		ImGui::TreePop();
	}

	// IMGUIウィンドウの終了
	if (isEnd)
	{
		// 子要素の終了
		ImGui::EndChild();
		// 表示項目のカウントを増やす
		itemCount++;
		ImGui::End();
	}

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