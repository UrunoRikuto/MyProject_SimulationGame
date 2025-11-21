/**************************************************//*
	@file	| SubJect.inl
	@brief	| サブジェクトインターフェースのインラインファイル
	@note	| オブザーバーパターンの基底クラス
*//**************************************************/
#pragma once
#include "SubJect.h"

/*****************************************//*
	@brief　	| オブザーバーの登録処理
	@param		| observer：登録するオブザーバーの参照
*//*****************************************/
inline void ISubJect::AddObserver(IObserver& observer)
{
	m_Observers.push_back(&observer);
}

/*****************************************//*
	@brief　	| オブザーバーの解除処理
	@param		| observer：解除するオブザーバーの参照
*//*****************************************/
inline void ISubJect::RemoveObserver(IObserver& observer)
{
	m_Observers.remove(&observer);
}
