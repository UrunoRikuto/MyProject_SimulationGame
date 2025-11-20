/**************************************************//*
	@file	| Job_Strategy.h
	@brief	| 職業クラスのhファイル
	@note	| 職業インターフェースの処理を定義
			| ストラテジーパターンで作成
*//**************************************************/
#pragma once
#include <string>

// 前方宣言
class CGameObject;

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


	// @brief 所属しているオブジェクトの取得
	// @return 所属しているオブジェクトのポインタ
	void SetOwner(CGameObject& pOwner) { m_pOwner = &pOwner; }

protected:
	// @brief 所属しているオブジェクトのポインタ
	CGameObject* m_pOwner = nullptr;
};

