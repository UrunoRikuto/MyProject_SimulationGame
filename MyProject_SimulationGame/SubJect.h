/**************************************************//*
	@file	| SubJect.h
	@brief	| サブジェクトインターフェースのhファイル
	@note	| オブザーバーパターンの基底クラス
*//**************************************************/
#pragma once
#include "Observer.h"
#include <list>

// @brief サブジェクトインターフェース
class ISubJect
{
public:
	// @brief 仮想デストラクタ
	virtual ~ISubJect() = default;

	// @brief オブザーバーの登録処理
	void AddObserver(IObserver& observer) { m_Observers.push_back(&observer); }

	// @brief オブザーバーの解除処理
	void RemoveObserver(IObserver& observer) { m_Observers.remove(&observer); }

	// @brief オブザーバーへの通知処理
	virtual void NotifyObservers() = 0;

protected:

	// @brief 登録されているオブザーバーリスト
	std::list<IObserver*> m_Observers;
};
