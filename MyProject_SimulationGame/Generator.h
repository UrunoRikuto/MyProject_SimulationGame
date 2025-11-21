/**************************************************//*
	@file	| IGenerator.h
	@brief	| ジェネレーターインターフェースのhファイル
	@note	| ジェネレーターの基底クラス
			| IObserverを継承
			| ストラテジーパターンで作成
*//**************************************************/
#pragma once
#include "Observer.h"
#include "FieldManager.h"

// @brief ジェネレーターインターフェース
class IGenerator : public IObserver
{
public:
	// @brief 仮想デストラクタ
	virtual ~IGenerator() = default;

	// @brief 通知処理
	void Notify() override;

protected:

	// @brief 生成処理
	virtual void Generate() = 0;

};

// インラインファイルのインクルード
#include "Generator.inl"

// 派生クラスのヘッダーファイルのインクルード
#include "HumanGenerator.h"
#include "WoodGenerator.h"
#include "StoneGenerator.h"
