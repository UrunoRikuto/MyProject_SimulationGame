/**************************************************//*
	@file	| Human.h
	@brief	| 人間オブジェクトクラスのhファイル
	@note	| 人間オブジェクトに関する処理の実装
			| CGameObjectを継承
*//**************************************************/
#pragma once
#include "Entity.h"
#include "JobOperator.h"
#include "Item.h"
#include <memory>
#include "BillboardRenderer.h"
#include "Skill.h"

// 前方宣言
class CHumanHouse;

// @brief 体力の最大値
constexpr float Max_Health = 100.0f;

// @brief 持てるアイテムの最大数
constexpr int MaxHoldItem = 50;

// @brief 移動速度
constexpr float Human_Move_Speed = 0.05f;

// @brief 未調理食料による空腹回復倍率
constexpr float Human_UnCookedFood_Hunger_Recovery_Multiplier = 0.5f;

// @brief 仕事による空腹減少値(マイフレーム)
constexpr float Work_Hunger_Decrease = 0.5f;

// @brief 食事に向かい始める空腹度
// @note 貯蔵庫から遠くで働いている間に、食事に戻る途中で餓死しないよう、警告値(Warning_Hunger)より高めに設定する
constexpr float Human_Seek_Food_Hunger = 40.0f;

// @brief 人間オブジェクトクラス
class CHuman final : public CEntity
{
public:
	// @brief 人間の状態列挙型
	enum class HUMAN_STATE
	{
		// 仕事中
		Working,

		// 休憩中(夜間で家で休む)
		Resting,

		// 食事中
		Eating,

		// 食料の確保中(空腹で、食料がどこにもない場合に自分で木の実を探しに行く)
		Foraging,
	};

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

	// @brief アイテムを取り出す
	// @param itemCategory：取り出すアイテムカテゴリー
	// @return 取り出したアイテムポインタ、所持していなかった場合はnullptr
	const CItem* TakeOutItem(const CItem::ITEM_CATEGORY itemCategory);

	// @brief アイテムを所持する
	// @param pItem：所持するアイテムポインタ
	void HoldItem(CItem* pItem);

	// @brief 職業ストラテジーの取得
	IJob_Strategy* GetHumanJob() const { return m_pJob.get(); }

	// @brief 人間の状態の取得
	HUMAN_STATE GetHumanState() const { return m_eState; }

	// @brief これまでに死亡した人間の数の取得
	static int GetDeathCount() { return s_nDeathCount; }

	// @brief 職業ストラテジーの設定
	// @param job：設定する職業ストラテジーポインタ
	void SetHumanJob(std::unique_ptr<IJob_Strategy> job);

	// @brief 住んでいる家の設定
	void SetLivingHouse(CHumanHouse* pHouse) { m_pLivingHouse = pHouse; }

	// @brief 家で休憩中かどうかの取得
	// @return true:家で休憩中 false:休憩していない
	bool IsRestingAtHome() const { return m_isRestingAtHome; }

	// @brief 食料があるかどうかの取得
	bool HasFood() const;

	// @brief ツールアイテムの取得
	CItem* GetToolItem() const { return m_pToolItem; }

	// @brief ツールアイテムの設定
	void SetToolItem(CItem* pToolItem) { m_pToolItem = pToolItem; }

	// @brief ツールアイテムを取り出す
	CItem* TakeOutToolItem();

	// @brief スキルポインタの取得
	CSkill* GetSkill() const { return m_pSkill.get(); }

private:

	// @brief 家に帰って休む処理
	void GoHomeAndRest();

	// @brief 食べ物を食べに行く処理
	void GoEatFood();

	// @brief 食料を自分で確保する処理
	// @note 空腹で、所持品にも貯蔵庫にも食料がない場合に、一番近い木から木の実(リンゴ)を探して採る
	void ForageFood();

	// @brief 食料の確保のために狙っている木の予約を解除する処理
	void ReleaseForageTarget();

private:
	// @brief 人間の状態
	HUMAN_STATE m_eState;

	// @brief これまでに死亡した人間の数
	inline static int s_nDeathCount = 0;

	// @brief 食料の確保のために狙っている木のID
	ObjectID m_ForageTargetID{ "", -1 };

	// @brief 食料の確保のために狙っている木があるかどうか
	bool m_bHasForageTarget = false;

	// @brief 職業ストラテジーポインタ
	std::unique_ptr<IJob_Strategy> m_pJob;

	// @brief スキルポインタ
	std::unique_ptr<CSkill> m_pSkill;

	// @brief ツールアイテム
	CItem* m_pToolItem;
	// @brief ツールを貯蔵庫に戻すかどうかのフラグ
	bool m_isReturnToolToStorage = false;

	// @brief 所持アイテムリスト
	std::vector<CItem*> m_ItemList;

	// @brief スタミナゲージビルボード
	// 0: 背景, 1: ゲージ本体
	CBillboardRenderer* m_pStaminaGaugeBillboard;

	// @brief 住んでいる家のポインタ
	CHumanHouse* m_pLivingHouse;
	// @brief 家で休憩中かどうか
	bool m_isRestingAtHome = false;

};

