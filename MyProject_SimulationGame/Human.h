/**************************************************//*
	@file	| Human.h
	@brief	| 人間オブジェクトクラスのhファイル
	@note	| 人間オブジェクトに関する処理の実装
			| CGameObjectを継承
*//**************************************************/
#pragma once
#include "GameObject.h"
#include "JobOperator.h"
#include "Item.h"
#include <memory>
#include "BillboardRenderer.h"

// 前方宣言
class CHumanHouse;

// @brief 持てるアイテムの最大数
constexpr int MaxHoldItem = 10;

// @brief 移動速度
constexpr float Human_Move_Speed = 0.05f;

// @brief 空腹最大値
constexpr float Human_Max_Hunger = 100.0f;
// @brief 自然空腹減少値
constexpr float Human_Natural_Hunger_Decrease = 0.1f;//0.01f
// @brief 仕事による空腹減少値
constexpr float Human_Work_Hunger_Decrease = 0.5f;

// @brief 人間オブジェクトクラス
class CHuman final : public CGameObject
{
public:
	// @brief コンストラクタ
	CHuman();

	// @brief デストラクタ
	~CHuman();

	// @brief 初期化処理
	void Init() override;

	// @brief 終了処理
	void Uninit() override;

	// @brief 更新処理
	void Update() override;

	// @brief 描画処理
	void Draw() override;

	// @brief オブジェクトが破棄された時の処理
	void OnDestroy() override;

	// @brief インスペクター表示処理
	// @param isEnd：true:ImGuiのEnd()を呼ぶ false:呼ばない
	// @return 表示した項目数
	int Inspecter(bool isEnd = true) override;

	// @brief アイテムを所持しているかどうか
	// @return true:所持している false:所持していない
	bool HasItem() const { return !m_ItemList.empty(); }

	// @brief アイテム所持数の取得
	// @return 所持しているアイテム数
	int GetItemCount() const { return static_cast<int>(m_ItemList.size()); }

	// @brief アイテムリストの取得
	std::vector<CItem*> GetItemList() { return m_ItemList; }

	// @brief アイテムを取りだす
	// @return 取り出したアイテムポインタ、所持していなかった場合はnullptr
	CItem* TakeOutItem();

	// @brief アイテムを取り出す
	// @param itemType：取り出すアイテムタイプ
	// @return 取り出したアイテムポインタ、所持していなかった場合はnullptr
	const CItem* TakeOutItem(const CItem::ITEM_TYPE itemType);

	// @brief アイテムを所持する
	// @param pItem：所持するアイテムポインタ
	void HoldItem(CItem* pItem);

	// @brief 職業ストラテジーの取得
	IJob_Strategy* GetHumanJob() const { return m_pJob.get(); }

	// @brief 職業ストラテジーの設定
	// @param job：設定する職業ストラテジーポインタ
	void SetHumanJob(std::unique_ptr<IJob_Strategy> job);

	// @brief 住んでいる家の設定
	void SetLivingHouse(CHumanHouse* pHouse) { m_pLivingHouse = pHouse; }

	// @brief 家に帰って休む処理
	void GoHomeAndRest();

	// @brief 家で休憩中かどうかの取得
	// @return true:家で休憩中 false:休憩していない
	bool IsRestingAtHome() const { return m_isRestingAtHome; }

	// @brief 空腹度が満タンかどうかの取得
	// @return true:満タン false:満タンではない
	bool IsMaxHunger() const { return m_fHunger >= Human_Max_Hunger; }

	// @brief 空腹度が0かどうかの取得
	// @return true:0 false:0ではない
	bool IsZeroHunger() const { return m_fHunger <= 0.0f; }

	// @brief 空腹度の減少
	// @param fAmount：減少量
	void DecreaseHunger(float fAmount);

private:
	// @brief 職業ストラテジーポインタ
	std::unique_ptr<IJob_Strategy> m_pJob;

	// @brief 所持アイテムリスト
	std::vector<CItem*> m_ItemList;

	// @brief スタミナゲージビルボード
	// 0: 背景, 1: ゲージ本体
	CBillboardRenderer* m_pStaminaGaugeBillboard;

	// @brief 空腹度
	float m_fHunger;

	// @brief 住んでいる家のポインタ
	CHumanHouse* m_pLivingHouse;
	// @brief 家で休憩中かどうか
	bool m_isRestingAtHome = false;

};

