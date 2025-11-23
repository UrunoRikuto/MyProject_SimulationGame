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

	// @brief インスペクター表示処理
	// @param isEnd：true:ImGuiのEnd()を呼ぶ false:呼ばない
	// @return 表示した項目数
	int Inspecter(bool isEnd = true) override;

	// @brief アイテムを取り出す
	// @param itemType：取り出すアイテムタイプ
	// @return 取り出したアイテムポインタ、所持していなかった場合はnullptr
	const CItem* TakeOutItem(const CItem::ITEM_TYPE itemType);

	// @brief アイテムを所持する
	// @param pItem：所持するアイテムポインタ
	void HoldItem(CItem* pItem);

private:
	// @brief 職業ストラテジーの設定
	// @param job：設定する職業ストラテジーポインタ
	void SetHumanJob(std::unique_ptr<IJob_Strategy> job)
	{
		m_pJob = std::move(job);
	}

private:
	// @brief 職業ストラテジーポインタ
	std::unique_ptr<IJob_Strategy> m_pJob;

	// @brief 所持アイテムリスト
	std::list<CItem*> m_ItemList;
};

