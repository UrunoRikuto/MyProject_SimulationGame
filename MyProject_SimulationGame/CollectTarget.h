/**************************************************//*
	@file	| CollectTarget.h
	@brief	| 収集対象オブジェクトクラスのhファイル
	@note	| CGameObjectを継承
*//**************************************************/
#pragma once
#include "GameObject.h"
#include "FieldCell.h"
#include "FieldManager.h"

// @brief 収集対象オブジェクトクラス
class CCollectTarget : public CGameObject
{
public:
	// @brief コンストラクタ
	CCollectTarget(): m_TargetingID({ "", -1 }){}

	// @brief デストラクタ
	virtual ~CCollectTarget() = default;

	// @brief 生成位置設定
	void SetCreatePos(CFieldCell* cell)
	{
		// 位置設定
		m_tParam.m_f3Pos = cell->GetPos();
		// セルを使用中に設定
		cell->SetUse(true);
		// 連携しているフィールドセルのインデックスを保存
		m_LinkedCellIndex = cell->GetIndex();
	}

	// @brief オブジェクトが破棄された時の処理
	void OnDestroy() override
	{
		// 連携しているフィールドセルの使用状態を解除
		CFieldCell* cell = CFieldManager::GetInstance()->GetFieldGrid()->GetFieldCells()[(int)m_LinkedCellIndex.y][(int)m_LinkedCellIndex.x];
		cell->SetUse(false);
	}

	// @brief ターゲット標的に指定してきているゲームオブジェクトのIDを取得
	ObjectID GetTargetingID() const { return m_TargetingID; }

	// @brief ターゲット標的に指定してきているゲームオブジェクトのIDを設定
	void SetTargetingID(const ObjectID& id) { m_TargetingID = id; }

protected:
	// @brief ターゲット標的に指定してきているゲームオブジェクトのID
	ObjectID m_TargetingID;

	// @brief 連携しているフィールドセルのインデックス
	DirectX::XMINT2 m_LinkedCellIndex = { -1, -1};

};

