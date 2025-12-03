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

// @brief スタミナ値の最大値
constexpr float Job_Max_Stamina = 100.0f;

// @brief 仕事によるスタミナ減少値
constexpr float Job_Work_Stamina_Decrease = 10.0f;


// @brief 職業クラスの基底インターフェース
class IJob_Strategy
{
public:
	// @brief 職業のステータス構造体
	struct JobStatus
	{
		// @brief コンストラクタ
		JobStatus()
			: m_fWorkPower(1.0f)
			, m_fStamina(100.0f)
			, m_fMaxStamina(m_fStamina)
		{
		}

		// 労働力
		float m_fWorkPower;

		// スタミナ
		float m_fStamina;
		// 最大スタミナ
		float m_fMaxStamina;
	};

public:
	// @brief 仮想デストラクタ
	virtual ~IJob_Strategy();

	// @brief インスペクター表示処理
	// @param isEnd：true:ImGuiのEnd()を呼ぶ false:呼ばない
	// @return 表示した項目数
	virtual int Inspecter(bool isEnd = true) = 0;

	// @brief 職業ごとの仕事処理の純粋仮想関数
	virtual void DoWork() = 0;

	// @brief 職業ごとの切り替え処理の純粋仮想関数
	virtual void OnChangeJob() = 0;

	// @brief 職業名を取得する純粋仮想関数
	// @return 職業名の文字列
	virtual std::string GetJobName() const = 0;

	// @brief 所属しているオブジェクトの取得
	// @param pOwner：所属しているオブジェクトの参照
	// @return 所属しているオブジェクトのポインタ
	void SetOwner(CHuman& pOwner) { m_pOwner = &pOwner; }

	// @brief 所属しているオブジェクトの取得
	// @return 所属しているオブジェクトのポインタ
	CHuman* GetOwner() const { return m_pOwner; }

	// @brief 職業のステータスを取得
	// @return 職業のステータス構造体の参照
	JobStatus& GetJobStatus() { return m_Status; }

	// @brief 職業のステータスを設定
	// @param status：設定する職業のステータス構造体の参照
	void SetJobStatus(const JobStatus& status) { m_Status = status; }

	// @brief スタミナの増加処理
	// @param fAmount：増加量
	void ChangeStamina(float fAmount);

protected:

	// @brief 休憩処理
	// @return true:休憩完了 false:休憩中
	bool RestAction();

protected:
	// @brief 所属しているオブジェクトのポインタ
	CHuman* m_pOwner = nullptr;

	// @brief 職業のステータス
	JobStatus m_Status;

	// @brief 休憩に利用している休憩施設のID
	CRefreshFacility* m_UsingRefreshFacility = nullptr;
};

