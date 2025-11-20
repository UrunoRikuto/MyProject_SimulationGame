/**************************************************//*
	@file	| Job_Strategy.h
	@brief	| 職業クラスのhファイル
	@note	| 職業インターフェースの処理を定義
			| ストラテジーパターンで作成
*//**************************************************/
#pragma once
#include <string>

// @brief 職業クラスの基底インターフェース
class IJob_Strategy
{
public:
	// @brief 仮想デストラクタ
	virtual ~IJob_Strategy() = default;

	// @brief 職業ごとの仕事処理の純粋仮想関数
	virtual void DoWork() = 0;

	// @brief 職業名を取得する純粋仮想関数
	// @return 職業名の文字列
	virtual std::string GetJobName() const = 0;
};

