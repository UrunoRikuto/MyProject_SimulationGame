/**************************************************//*
	@file	| CollectTarget.h
	@brief	| 収集対象オブジェクトクラスのhファイル
	@note	| CGameObjectを継承
*//**************************************************/
#pragma once
#include "GameObject.h"
#include "FieldCell.h"
#include "FieldManager.h"
#include "BillboardRenderer.h"
#include "Item.h"

// @brief 収集対象オブジェクトクラス
class CCollectTarget : public CGameObject
{
public:
	// @brief 収集対象のステータス構造体
	struct CollectStatus
	{
		// 耐久値
		float m_fHp;
		// 最大耐久値
		float m_fMaxHp;

		// ドロップアイテム
		std::vector<CItem*> m_DropItems;
	};

public:
	// @brief コンストラクタ
	CCollectTarget();

	// @brief デストラクタ
	virtual ~CCollectTarget();

	// @brief 描画処理
	void Draw() override;

	// @brief 生成位置設定
	void SetCreatePos(CFieldCell* cell);

	// @brief オブジェクトが破棄された時の処理
	void OnDestroy() override;

	// @brief ターゲット標的に指定してきているゲームオブジェクトのIDを取得
	ObjectID GetTargetingID() const { return m_TargetingID; }

	// @brief ターゲット標的に指定してきているゲームオブジェクトのIDを設定
	void SetTargetingID(const ObjectID& id) { m_TargetingID = id; }

	// @brief 耐久地を減らす
	// @param damage：減らす耐久値
	void DecreaseHp(const float damage);

	// @brief 耐久地が0以下かどうかを取得
	bool IsDead() const { return m_Status.m_fHp <= 0.0f; }

	// @brief アイテムドロップ処理
	// @return ドロップしたアイテムのベクター
	std::vector<CItem*> DropItem() { return m_Status.m_DropItems; }

protected:
	// @brief ターゲット標的に指定してきているゲームオブジェクトのID
	ObjectID m_TargetingID;

	// @brief 連携しているフィールドセルのインデックス
	DirectX::XMINT2 m_LinkedCellIndex;

	// @brief 収集対象のステータス
	CollectStatus m_Status;

	// @brief 耐久値を表示するビルボードコンポーネント
	CBillboardRenderer* m_pHpBillboard;
};

