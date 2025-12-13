/**************************************************//*
	@file	| Job_Strategy.h
	@brief	| 職業クラスのhファイル
	@note	| 職業インターフェースの処理を定義
			| ストラテジーパターンで作成
			| 参考サイト
			| ┗ https://qiita.com/hankehly/items/1848f2fd8e09812d1aaf (C++によるストラテジーパターンの実装例)
*//**************************************************/
#pragma once
#include "RefreshFacility.h"
#include <string>

// 前方宣言
class CHuman;

// @brief 仕事によるスタミナ減少値(毎フレーム)
constexpr float Job_Work_Stamina_Decrease = 10.0f;

// @brief 職業クラスの基底インターフェース
class IJob_Strategy
{
public:
	// @brief 仮想デストラクタ
	virtual ~IJob_Strategy();

	// @brief インスペクター表示処理
	// @param isEnd：true:ImGuiのEnd()を呼ぶ false:呼ばない
	// @return 表示した項目数
	virtual int Inspecter(bool isEnd = true) = 0;

	// @brief 職業ステータスのImGui描画処理
	// @note 職業ごとのステータス表示処理を実装する場合はオーバーライドする
	virtual void DrawJobStatusImGui();

	// @brief 職業ごとの仕事処理の純粋仮想関数
	virtual void DoWork() = 0;

	// @brief 職業ごとの切り替え処理の純粋仮想関数
	virtual void OnChangeJob() = 0;

	// @brief 職業名を取得する純粋仮想関数
	// @return 職業名の文字列
	virtual std::string GetJobName() const = 0;

	// @brief 所属しているオブジェクトの設定
	// @param pOwner：所属するオブジェクトの参照
	void SetOwner(CHuman& pOwner);

	// @brief 所属しているオブジェクトの取得
	// @return 所属しているオブジェクトのポインタ
	CHuman* GetOwner() const { return m_pOwner; }

	// @brief 労働力の取得
	float GetWorkPower() const { return m_fWorkPower; }
	// @brief 労働力の設定
	void SetWorkPower(float fWorkPower) { m_fWorkPower = fWorkPower; }

protected:

	// @brief 休憩処理
	// @return true:休憩完了 false:休憩中
	bool RestAction();

protected:
	// @brief 所属しているオブジェクトのポインタ
	CHuman* m_pOwner = nullptr;

	// @brief 労働力
	float m_fWorkPower;

	// @brief 休憩に利用している休憩施設のID
	CRefreshFacility* m_UsingRefreshFacility = nullptr;
};

