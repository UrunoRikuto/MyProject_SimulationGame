/**************************************************//*
	@file	| CollectTarget.h
	@brief	| 収集対象オブジェクトクラスのhファイル
	@note	| CGameObjectを継承
*//**************************************************/
#pragma once
#include "GameObject.h"

// @brief 収集対象オブジェクトクラス
class CCollectTarget : public CGameObject
{
public:
	// @brief コンストラクタ
	CCollectTarget(): m_TargetingID({ "", -1 }){}

	// @brief デストラクタ
	virtual ~CCollectTarget() = default;

	// @brief ターゲット標的に指定してきているゲームオブジェクトのIDを取得
	ObjectID GetTargetingID() const { return m_TargetingID; }

	// @brief ターゲット標的に指定してきているゲームオブジェクトのIDを設定
	void SetTargetingID(const ObjectID& id) { m_TargetingID = id; }

protected:
	// @brief ターゲット標的に指定してきているゲームオブジェクトのID
	ObjectID m_TargetingID;

};

