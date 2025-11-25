/**************************************************//*
	@file	| Human.cpp
	@brief	| 人間オブジェクトクラスのcppファイル
	@note	| 人間オブジェクトに関する処理の実装
			| CGameObjectを継承
*//**************************************************/
#include "Human.h"
#include "ModelRenderer.h"
#include "FieldManager.h"
#include <imgui.h>

/****************************************//*
	@brief　	| コンストラクタ
*//****************************************/
CHuman::CHuman()
	: CGameObject()
	, m_pJob(std::make_unique<CNeet_Job>())
{
	// モデルレンダラーコンポーネントの追加
	AddComponent<CModelRenderer>();

	// 耐久値初期化
	m_pStaminaGaugeBillboard = new CBillboardRenderer(this);
	m_pStaminaGaugeBillboard->SetKey("Bar_Gauge");

}

/****************************************//*
	@brief　	| デストラクタ
*//****************************************/
CHuman::~CHuman()
{
}

/****************************************//*
	@brief　	| 初期化処理
*//****************************************/
void CHuman::Init()
{
	// 基底クラスの初期化処理
	CGameObject::Init();

	m_tParam.m_f3Pos = CFieldManager::GetInstance()->GetFieldGrid()->GetFieldCells()[(CFieldGrid::GridSizeX / 2) - 1][(CFieldGrid::GridSizeY / 2) - 1]->GetPos();

	// モデルレンダラーコンポーネントの設定
	CModelRenderer* pModelRenderer = GetComponent<CModelRenderer>();
	pModelRenderer->SetKey("Human");
	pModelRenderer->SetRendererParam(m_tParam);

	// 頂点シェーダーの設定
	VertexShader* pVS = new VertexShader(VSType::Object);
	pModelRenderer->SetVertexShader(pVS);
	// ピクセルシェーダーの設定
	PixelShader* pPS = new PixelShader(PSType::TexColor);
	pModelRenderer->SetPixelShader(pPS);
}

/****************************************//*
	@brief　	| 終了処理
*//****************************************/
void CHuman::Uninit()
{
	// 基底クラスの終了処理
	CGameObject::Uninit();
}

/****************************************//*
	@brief　	| 更新処理
*//****************************************/
void CHuman::Update()
{
	// 基底クラスの更新処理
	CGameObject::Update();

	// 仕事処理
	if(m_pJob)m_pJob->DoWork();

}

/****************************************//*
	@brief　	| 描画処理
*//****************************************/
void CHuman::Draw()
{
	// 基底クラスの描画処理
	CGameObject::Draw();

	// スタミナゲージの描画
	float fStaminaRatio = m_pJob->GetJobStatus().m_fStamina / m_pJob->GetJobStatus().m_fMaxStamina;

	// スタミナが満タンでなければ描画
	if (fStaminaRatio < 1.0f)
	{
		// 前景
		m_pStaminaGaugeBillboard->SetPos({ m_tParam.m_f3Pos.x - (m_tParam.m_f3Size.x * (1.0f - fStaminaRatio)), m_tParam.m_f3Pos.y + 2.0f, m_tParam.m_f3Pos.z });
		m_pStaminaGaugeBillboard->SetSize({ 2.0f * fStaminaRatio, 0.2f, 1.0f });
		m_pStaminaGaugeBillboard->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		m_pStaminaGaugeBillboard->SetUVPos({ 0.0f, 0.0f });
		m_pStaminaGaugeBillboard->SetUVSize({ 1.0f, 1.0f });
		m_pStaminaGaugeBillboard->SetRotation({ 0.0f, 0.0f, 0.0f });
		m_pStaminaGaugeBillboard->SetColor({ 1.0f, 1.0f, 0.0f, 1.0f });
		m_pStaminaGaugeBillboard->Draw();
	}
}

/****************************************//*
	@brief　	| インスペクター表示処理
	@param		| isEnd：true:ImGuiのEnd()を呼ぶ false:呼ばない
	@return		| 表示した項目数
	@note		| ImGuiを使用してオブジェクトのパラメータを表示、編集する
*//****************************************/
int CHuman::Inspecter(bool isEnd)
{
	// 基底クラスのインスペクター表示処理
	// (位置情報、サイズ情報、回転情報)
	int nItemCount = CGameObject::Inspecter(false);

	/**** 職業表示 ****/

	// 折りたたみヘッダーの表示
	if (ImGui::CollapsingHeader("Job"))
	{
		// 現在の職業を文字列で取得
		std::string currentJob = m_pJob->GetJobName();
		// 表示
		ImGui::Text(("JobName: " + currentJob).c_str());

		// Combo 用（string → const char* の配列に変換）
		std::vector<const char*> items;
		for (auto& s : JobNames) items.push_back(s.c_str());

		// 現在のインデックス
		int currentIndex = std::distance(
			JobNames.begin(),
			std::find(JobNames.begin(), JobNames.end(), currentJob)
		);

		if (ImGui::Combo("JobSelect", &currentIndex, items.data(), items.size()))
		{
			// 選択された string の職業名
			currentJob = JobNames[currentIndex];

			// Strategy 差し替え
			SetHumanJob(CreateJobByName(currentJob, *this));
		}

		ImGui::Text("Job Status:");
		// 職業ステータスの取得
		IJob_Strategy::JobStatus& status = m_pJob->GetJobStatus();
		ImGui::Text((" WorkPower: " + std::to_string(status.m_fWorkPower)).c_str());
		ImGui::Text((" Stamina: " + std::to_string(status.m_fStamina) + " / " + std::to_string(status.m_fMaxStamina)).c_str());
	}

	// 折りたたみヘッダーの表示
	if(ImGui::CollapsingHeader("Items"))
	{
		ImGui::BeginChild("ItemList", ImVec2(0, 150), true);
		// 所持アイテムリストの表示
		// 名前と個数を表示
		std::map<CItem::ITEM_TYPE, int> itemCountMap;
		for (const CItem* pItem : m_ItemList)
		{
			itemCountMap[pItem->GetItemType()]++;
		}
		for (const auto& pair : itemCountMap)
		{
			// アイテム名と個数を表示
			std::string itemName = CItem::ITEM_TYPE_TO_STRING(pair.first);
			ImGui::Text((itemName + ": " + std::to_string(pair.second)).c_str());
		}


		ImGui::EndChild();
		nItemCount++;
	}

	/***** 職業表示 *****/

	// IMGUIウィンドウの終了
	if (isEnd)
	{
		// 子要素の終了
		ImGui::EndChild();
		// 表示項目のカウントを増やす
		nItemCount++;
		ImGui::End();
	}



	// 表示した項目数を返す
	return nItemCount;
}

/****************************************//*
	@brief　	| アイテムを取りだす
	@return		| 取り出したアイテムポインタ、所持していなかった場合はnullptr
*//****************************************/
CItem* CHuman::TakeOutItem()
{
	// 所持アイテムリストが空の場合はnullptrを返す
	if (m_ItemList.empty())return nullptr;

	// 所持アイテムリストの先頭のアイテムを取り出す
	CItem* pItem = m_ItemList.front();

	// 所持アイテムリストから削除
	m_ItemList.pop_front();

	// 取り出したアイテムポインタを返す
	return pItem;
}

/****************************************//*
	@brief　	| アイテムを取り出す
	@param		| itemType：取り出すアイテムタイプ
	@return		| 取り出したアイテムポインタ、所持していなかった場合はnullptr
*//****************************************/
const CItem* CHuman::TakeOutItem(const CItem::ITEM_TYPE itemType)
{
	// 所持アイテムリストを探索
	for (auto it = m_ItemList.begin(); it != m_ItemList.end(); ++it)
	{
		// 指定されたアイテムタイプと一致するアイテムを探索
		if ((*it)->GetItemType() == itemType)
		{
			// アイテムを取り出す
			CItem* pItem = *it;
			// 所持アイテムリストから削除
			m_ItemList.erase(it);
			// 取り出したアイテムポインタを返す
			return pItem;
		}
	}

	// 指定されたアイテムタイプのアイテムを所持していなかった場合はnullptrを返す
	return nullptr;
}

/****************************************//*
	@brief　	| アイテムを所持する
	@param		| pItem：所持するアイテムポインタ
*//****************************************/
void CHuman::HoldItem(CItem* pItem)
{
	// 所持アイテムリストに追加
	m_ItemList.push_back(pItem);
}
