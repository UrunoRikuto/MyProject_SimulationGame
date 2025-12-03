/**************************************************//*
	@file	| Human.cpp
	@brief	| 人間オブジェクトクラスのcppファイル
	@note	| 人間オブジェクトに関する処理の実装
			| CGameObjectを継承
*//**************************************************/
#include "Human.h"
#include "ModelRenderer.h"
#include "FieldManager.h"
#include "ImguiSystem.h"
#include "ShaderManager.h"
#include "GameTimeManager.h"
#include "StructMath.h"
#include "Oparation.h"
#include "HumanHouse.h"
#include "CivLevelManager.h"
#include "GeneratorManager.h"
#include "Main.h"
#include "Item.h"
#include "StorageHouse.h"

/****************************************//*
	@brief　	| コンストラクタ
*//****************************************/
CHuman::CHuman()
	: CGameObject()
	, m_pJob(std::make_unique<CNeet_Job>())
	, m_pLivingHouse(nullptr)
	, m_isRestingAtHome(false)
	, m_fHunger(Human_Max_Hunger)
	, m_eState(HUMAN_STATE::Working)
	, m_isEating(false)
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
	// スタミナゲージビルボードの解放
	SAFE_DELETE(m_pStaminaGaugeBillboard);

	// 所持アイテムの解放
	for (CItem* pItem : m_ItemList)
	{
		delete pItem;
	}
	m_ItemList.clear();

	// 職業ストラテジーの解放
	m_pJob.reset();
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
	VertexShader* pVS = CShaderManager::GetInstance()->GetVertexShader(VSType::Object);
	pModelRenderer->SetVertexShader(pVS);

	// ピクセルシェーダーの設定
	PixelShader* pPS = CShaderManager::GetInstance()->GetPixelShader(PSType::TexColor);
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

	// 人間の状態による処理分岐
	switch (m_eState)
	{
	case CHuman::HUMAN_STATE::Working:
		// 仕事処理
		if (m_pJob)m_pJob->DoWork();
		break;
	case CHuman::HUMAN_STATE::Resting:
		// 家に帰って休む
		GoHomeAndRest();
		break;
	case CHuman::HUMAN_STATE::Eating:
		// 食べ物を食べに行く
		GoEatFood();
		break;
	}

	// 貯蔵庫の取得
	CStorageHouse* pStorageHouse = GetScene()->GetGameObject<CStorageHouse>();
	// 食料がある場合の処理
	if(HasFood() || pStorageHouse->HasFood())
	{
		// 空腹度が警告値を下回ったら食事状態に移行
		if (m_fHunger < Human_Warning_Hunger)
		{
			m_eState = CHuman::HUMAN_STATE::Eating;
			m_isEating = true;
		}
		// 食事が完了したら仕事状態に移行
		if (IsFullHunger())
		{
			m_isEating = false;
		}
		// 食事が完了していなければ食事状態を維持
		if (!m_isEating)
		{
			// 夜になったら家に帰って休む
			if (CGameTimeManager::GetInstance()->GetCurrentDayTime() == CGameTimeManager::DAY_TIME::NIGHT)
			{
				// 夜になったら家に帰って休む
				m_eState = CHuman::HUMAN_STATE::Resting;
			}
			else
			{
				// 昼間は仕事状態に移行
				m_eState = CHuman::HUMAN_STATE::Working;
			}
		}
	}
	// 食料がない場合の処理
	else
	{
		// 夜になったら家に帰って休む
		if (CGameTimeManager::GetInstance()->GetCurrentDayTime() == CGameTimeManager::DAY_TIME::NIGHT)
		{
			// 夜になったら家に帰って休む
			m_eState = CHuman::HUMAN_STATE::Resting;
		}
		else
		{
			// 昼間は仕事状態に移行
			m_eState = CHuman::HUMAN_STATE::Working;
		}
	}

	// 空腹度の減少処理
	// 休憩中でなければ等倍で空腹度を減少させる
	if (m_eState != CHuman::HUMAN_STATE::Resting)
	{
		m_fHunger -= Human_Natural_Hunger_Decrease;
	}
	// 休憩中であれば半分の速度で空腹度を減少させる
	else
	{
		m_fHunger -= Human_Natural_Hunger_Decrease * 0.5f;
	}
	// 空腹度が0を下回ったら死亡処理
	if (m_fHunger < 0.0f)
	{
		// オブジェクト破棄
		Destroy();
	}
}

/****************************************//*
	@brief　	| 描画処理
*//****************************************/
void CHuman::Draw()
{
	// 家で休憩中でなければ描画
	if (!m_isRestingAtHome)
	{
		// 基底クラスの描画処理
		CGameObject::Draw();
	}

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

	// 家で休憩中フラグをリセット
	m_isRestingAtHome = false;
}

/****************************************//*
	@brief　	| オブジェクトが破棄された時の処理
*//****************************************/
void CHuman::OnDestroy()
{
	// 人間の生成依頼を作成
	CGeneratorManager::GetInstance()->AddGenerateRequest({
			CGeneratorManager::GenerateType::Human,
			Human_Regenerate_Wait_Time // 生成までの時間（秒）
		});

	// 住んでいる家から自分を削除
	if (m_pLivingHouse)
	{
		// 住んでいる家から自分を削除
		m_pLivingHouse->RemoveResident(this);
		m_pLivingHouse = nullptr;
	}

	// 基底クラスのオブジェクト破棄時の処理
	CGameObject::OnDestroy();
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

	ImGui::Separator();

	// 空腹度表示
	// テキストカラーの設定
	// 通常時は緑色、空腹度が20以下の場合は赤色に変更
	ImVec4 HumgerTextColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
	if(m_fHunger <= 20.0f)
	{
		HumgerTextColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	}

	ImGui::TextColored(HumgerTextColor,"Humger: %.2f / %.2f", m_fHunger, Human_Max_Hunger);

	ImGui::Separator();

	// 折りたたみヘッダーの表示
	if (ImGui::CollapsingHeader("Job"))
	{
		// 現在の職業を文字列で取得
		std::string currentJob = m_pJob->GetJobName();

		// 職業名リストの取得
		const std::vector<std::string> JobNames = CCivLevelManager::GetInstance()->GetUnlockJobNames();

		// Combo 用（string → const char* の配列に変換）
		std::vector<const char*> items;
		for (auto& s : JobNames) items.push_back(s.c_str());

		// 現在のインデックス
		int currentIndex = static_cast<int>(std::distance(
			JobNames.begin(),
			std::find(JobNames.begin(), JobNames.end(), currentJob)
		));

		if (ImGui::Combo("JobSelect", &currentIndex, items.data(), static_cast<int>(items.size())))
		{
			// 選択された string の職業名
			currentJob = JobNames[currentIndex];

			// Strategy 差し替え
			SetHumanJob(CreateJobByName(currentJob, *this));
		}

		// 職業ストラテジーのインスペクター表示処理
		if (m_pJob)
		{
			nItemCount += m_pJob->Inspecter(false);
		}
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
	m_ItemList.erase(m_ItemList.begin());

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
	@brief　	| アイテムを取り出す
	@param		| itemCategory：取り出すアイテムカテゴリー
	@return		| 取り出したアイテムポインタ、所持していなかった場合はnullptr
*//****************************************/
const CItem* CHuman::TakeOutItem(const CItem::ITEM_CATEGORY itemCategory)
{
	// 所持アイテムリストを探索
	for (auto it = m_ItemList.begin(); it != m_ItemList.end(); ++it)
	{
		// 指定されたアイテムカテゴリーと一致するアイテムを探索
		if ((*it)->GetItemCategory() == itemCategory)
		{
			// アイテムを取り出す
			CItem* pItem = *it;
			// 所持アイテムリストから削除
			m_ItemList.erase(it);
			// 取り出したアイテムポインタを返す
			return pItem;
		}
	}

	// 指定されたアイテムカテゴリーのアイテムを所持していなかった場合はnullptrを返す
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

/****************************************//*
	@brief　	| 職業ストラテジーの設定
	@param		| job：設定する職業ストラテジーポインタ
*//****************************************/
void CHuman::SetHumanJob(std::unique_ptr<IJob_Strategy> job)
{
	// 職業切り替え時の処理
	m_pJob->OnChangeJob();

	// スタミナ値を引き継ぐ
	job->GetJobStatus().m_fStamina = m_pJob->GetJobStatus().m_fStamina;
	
	// 所属オブジェクトを設定
	m_pJob = std::move(job);
}

/****************************************//*
	@brief　	| 家に帰って休む処理
	@return		| true:家に帰って休憩中 false:休憩していない
*//****************************************/
void CHuman::GoHomeAndRest()
{
	// 休憩処理
	if (m_pLivingHouse)
	{
		// 家の位置を取得
		DirectX::XMFLOAT3 f3HousePos = m_pLivingHouse->GetPos();

		// 自分の位置を取得
		DirectX::XMFLOAT3 f3MyPos = m_tParam.m_f3Pos;

		// 距離を計算
		float fDistance = StructMath::Distance(f3HousePos, f3MyPos);

		// 家の近くにいる場合
		if (fDistance < 0.3f)
		{
			// 家で休憩中フラグを立てる
			m_isRestingAtHome = true;
			// 休憩処理
			if (m_pJob)m_pJob->ChangeStamina(1.0f);

			return;
		}

		// 家の方向を計算
		DirectX::XMFLOAT3 f3Direction = f3HousePos - f3MyPos;

		// 正規化
		f3Direction = StructMath::Normalize(f3Direction);

		f3MyPos += f3Direction * Human_Move_Speed;
		// 位置更新
		m_tParam.m_f3Pos = f3MyPos;
	}
}

/****************************************//*
	@brief　	| 食料を食べに行く処理
	@note		| 所持している調理済み食料アイテムを優先的に食べる
				| 所持している未調理食料アイテムを次に食べる
				| 所持している食料アイテムがなければ倉庫から探しに行く
*//****************************************/
void CHuman::GoEatFood()
{
	// 所持している調理済み食料アイテムを探す
	const CItem* pFoodItem = TakeOutItem(CItem::ITEM_CATEGORY::CookedFood);

	// 調理済み食料アイテムがあれば食べる
	if (pFoodItem == nullptr)
	{
		// 所持している未調理食料アイテムを探す
		pFoodItem = TakeOutItem(CItem::ITEM_CATEGORY::UnCookedFood);
	}

	// 食料が所持アイテムの中から見つからなかった場合は倉庫から探しに行く
	if(pFoodItem == nullptr)
	{
		// 一番近い貯蔵庫を探す
		CStorageHouse* pStorageHouse = GetScene()->GetGameObject<CStorageHouse>();

		// 見つからなかった場合は処理終了
		if (pStorageHouse == nullptr)return;

		// 貯蔵庫の位置を取得
		DirectX::XMFLOAT3 f3StoragePos = pStorageHouse->GetPos();
		// 自分の位置を取得
		DirectX::XMFLOAT3 f3MyPos = m_tParam.m_f3Pos;

		// 距離を計算
		float fDistance = StructMath::Distance(f3StoragePos, f3MyPos);

		// 貯蔵庫が一定範囲外にある場合は移動する
		if (fDistance > 1.0f)
		{
			// 貯蔵庫の方向を計算
			DirectX::XMFLOAT3 f3Direction = f3StoragePos - f3MyPos;
			// 正規化
			f3Direction = StructMath::Normalize(f3Direction);
			f3MyPos += f3Direction * Human_Move_Speed;
			// 位置更新
			m_tParam.m_f3Pos = f3MyPos;
			return;
		}

		// 貯蔵庫の近くにいる場合は食料を探す
		pFoodItem = pStorageHouse->TakeOutItem(CItem::ITEM_CATEGORY::CookedFood);
		// 調理済み食料アイテムがあれば食べる
		if (pFoodItem == nullptr)
		{
			// 所持している未調理食料アイテムを探す
			pFoodItem = pStorageHouse->TakeOutItem(CItem::ITEM_CATEGORY::UnCookedFood);
		}

		// 食料アイテムがあれば
		if (pFoodItem)
		{
			// 食料アイテムのカテゴリーによって空腹度回復量を変える
			switch (pFoodItem->GetItemCategory())
			{
				// 未調理食料アイテム
			case CItem::ITEM_CATEGORY::UnCookedFood:
				// 食料アイテムを食べる(0.5倍)
				m_fHunger += CItem::GetHungerRecoveryValue(pFoodItem->GetItemType()) * Human_UnCookedFood_Hunger_Recovery_Multiplier;
				break;
				// 調理済み食料アイテム
			case CItem::ITEM_CATEGORY::CookedFood:
				// 食料アイテムを食べる(等倍)
				m_fHunger += CItem::GetHungerRecoveryValue(pFoodItem->GetItemType());
				break;
			}
			// 空腹度が最大値を超えないように補正
			if (m_fHunger > Human_Max_Hunger)
			{
				m_fHunger = Human_Max_Hunger;
			}
		}
		// 食料が見つからなかった場合は仕事状態に移行
		else
		{
			m_eState = CHuman::HUMAN_STATE::Working;
		}
	}
	
}

/****************************************//*
	@brief　	| 食料を所持しているかどうかの判定
	@return		| true:所持している false:所持していない
*//****************************************/
bool CHuman::HasFood() const
{
	// 所持している食料アイテムを探す
	for (const CItem* pItem : m_ItemList)
	{
		if (pItem->GetItemCategory() == CItem::ITEM_CATEGORY::CookedFood ||
			pItem->GetItemCategory() == CItem::ITEM_CATEGORY::UnCookedFood)
		{
			return true;
		}
	}

	// 所持していなかった場合はfalseを返す
	return false;
}

/****************************************//*
	@brief　	| 空腹度の減少
	@param		| fAmount：減少量
*//****************************************/
void CHuman::DecreaseHunger(float fAmount)
{
	// 空腹度を減少
	m_fHunger -= fAmount;

	// 空腹度が0未満にならないように補正
	if (m_fHunger < 0.0f)
	{
		m_fHunger = 0.0f;
	}
}
