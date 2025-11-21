/**************************************************//*
	@file	| Observer.h
	@brief	| オブザーバーインターフェースのhファイル
	@note	| オブザーバーパターンの基底クラス
*//**************************************************/
#pragma once

// @brief オブザーバーインターフェース
class IObserver
{
public:
	// @brief 仮想デストラクタ
	virtual ~IObserver() = default;

	// @brief 通知処理
	virtual void Notify() = 0;
};

