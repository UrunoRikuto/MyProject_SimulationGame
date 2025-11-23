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
		// ※将来的に複数アイテム対応する可能性がある為、vectorで保持
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
	void DecreaseHp(const float damage);

	// @brief 耐久地が0以下かどうかを取得
	bool IsDead() const { return m_Status.m_fHp <= 0.0f; }

protected:
	// @brief ターゲット標的に指定してきているゲームオブジェクトのID
	ObjectID m_TargetingID;

	// @brief 連携しているフィールドセルのインデックス
	DirectX::XMINT2 m_LinkedCellIndex;

	// @brief 収集対象のステータス
	CollectStatus m_Status;

	// @brief 耐久値を表示するビルボードコンポーネント
	// @note 0:背景 1:前景
	CBillboardRenderer* m_pHpBillboard[2];
};

