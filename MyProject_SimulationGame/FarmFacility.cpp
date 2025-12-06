/**************************************************//*
	@file	| FarmFacility.cpp
	@brief	| 農場施設クラス実装
	@note	| 農場施設クラスの実装
			| CBuildObjectを継承
*//**************************************************/
#include "FarmFacility.h"
#include "ShaderManager.h"
#include "ImguiSystem.h"
#include "BuildManager.h"
#include "Item_Material.h"
#include "CivLevelManager.h"

/*****************************************//*
	@brief　	| コンストラクタ
*//*****************************************/
CFarmFacility::CFarmFacility()
	:CBuildObject()
	, m_pCropList()
	, m_pCompletedItemList()
	, m_fBuildXP(0.0f)
{
}

/*****************************************//*
	@brief　	| デストラクタ
*//*****************************************/
CFarmFacility::~CFarmFacility()
{
}

/*****************************************//*
	@brief　	| 初期化処理
*//*****************************************/
void CFarmFacility::Init()
{
	// 基底クラスの初期化処理
	CBuildObject::Init();

	// モデルレンダラーコンポーネントの設定
	CModelRenderer* pModelRenderer = GetComponent<CModelRenderer>();
	pModelRenderer->SetKey("FarmFacility");

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
void CFarmFacility::Update()
{
	// 基底クラスの更新処理
	CBuildObject::Update();

	// 農作物の成長を進める
	ProgressCrops();

	// 完成した農作物を格納
	StoreCompletedCrops();
}

/*****************************************//*
	@brief　	| インスペクター表示処理
*//*****************************************/
int CFarmFacility::Inspecter()
{
	int itemCount = 0;

	// 基底クラスのインスペクター表示処理
	itemCount += CBuildObject::Inspecter();

	// 農作物リストの表示
	ImGui::Text("Crops:");
	for (const auto& crop : m_pCropList)
	{
		ImGui::BulletText("Type: %s, Progress: %.2f%%",
			CItem::ITEM_TYPE_TO_STRING(crop.eCropType).c_str(),
			crop.fGrowthProgress);
		itemCount++;
	}

	// 完成した農作物リストの表示
	ImGui::Text("Completed Crops:");
	for (const auto& item : m_pCompletedItemList)
	{
		ImGui::BulletText("Type: %s",
			CItem::ITEM_TYPE_TO_STRING(item->GetItemType()).c_str());
		itemCount++;
	}

	// 表示した項目数を返す
	return itemCount;
}

/*****************************************//*
	@brief　	| 農作物を追加可能かどうかを取得
	@return		| true:追加可能 false:追加不可
*//*****************************************/
bool CFarmFacility::CanAddCrop() const
{
	// 現在の農作物数が最大農作物数未満であれば追加可能
	return m_pCropList.size() < MAX_CROP_FARM[m_nBuildLevel - 1];
}

/*****************************************//*
	@brief　	| 農作物を追加
	@param 		| eCropType：農作物のアイテムタイプ
*//*****************************************/
void CFarmFacility::AddCrop(CItem::ITEM_TYPE eCropType)
{
	// アイテムタイプが種でなければ処理終了
	if (CItem::GetItemCategoryFromType(eCropType) != CItem::ITEM_CATEGORY::Seed) return;

	// 農作物構造体を生成し、農作物リストに追加
	Crop newCrop;
	newCrop.eCropType = eCropType;
	newCrop.fGrowthProgress = 0.0f;
	m_pCropList.push_back(newCrop);
}

/*****************************************//*
	@brief　	| 完成した農作物があるかどうかを取得
	@return		| true:完成した農作物がある false:完成した農作物がない
*//*****************************************/
bool CFarmFacility::HasCompletedCrops() const
{
	// 完成した農作物リストが空でなければ完成した農作物がある
	return !m_pCompletedItemList.empty();
}

/*****************************************//*
	@brief　	| 完成した農作物を取得
	@return		| 完成した農作物のCItemポインタ、無ければnullptr
*//*****************************************/
CItem* CFarmFacility::TakeCompletedCrop()
{
	// 完成した農作物リストが空であればnullptrを返す
	if (m_pCompletedItemList.empty()) return nullptr;

	// リストの先頭から農作物を取得
	CItem* pCompletedCrop = m_pCompletedItemList.front();

	// リストから先頭の農作物を削除
	m_pCompletedItemList.pop_front();

	// 取得した農作物を返す
	return pCompletedCrop;
}

/*****************************************//*
	@brief　	| 完成した農作物を格納
*//*****************************************/
void CFarmFacility::StoreCompletedCrops()
{
	// 農作物リストが空であれば処理終了
	if (m_pCropList.empty()) return;

	// 農作物リストを走査
	for (auto it = m_pCropList.begin(); it != m_pCropList.end(); )
	{
		// 成長進行度が100%以上であれば完成
		if (it->fGrowthProgress >= 100.0f)
		{
			// 完成した農作物をCItemとして生成し、完成リストに追加
			CItem* pCompletedItem = new(std::nothrow) CItem(CropMaterials::GetCropFromSeed(it->eCropType));

			if (pCompletedItem != nullptr)
			{
				m_pCompletedItemList.push_back(pCompletedItem);

				// 建築経験値を加算
				m_fBuildXP += BUILD_XP_AMOUNT; 

				// 文明レベルに経験値を加算
				CCivLevelManager::GetInstance()->AddExp(CCivLevelManager::ExpType::Farming);

				if (m_fBuildXP >= 100.0f)
				{
					// 建築経験値が100以上であれば建築依頼を追加
					CBuildManager::GetInstance()->AddBuildRequest(CBuildManager::BuildType::FarmFacility);

					m_fBuildXP -= 100.0f; // 建築経験値をリセット
				}
			}
			// 農作物リストから削除
			it = m_pCropList.erase(it);
		}
		else
		{
			++it;
		}
	}
}

/*****************************************//*
	@brief　	| 農作物の成長を進める
*//*****************************************/
void CFarmFacility::ProgressCrops()
{
	// 農作物リストが空であれば処理終了
	if (m_pCropList.empty()) return;

	// 成長進行度の取得
	float fGrowthAmount = GetCropGrowthProgressAmount();

	// 農作物リストを走査し、成長進行度を進める
	for (auto& crop : m_pCropList)
	{
		crop.fGrowthProgress += fGrowthAmount;
	}
}
