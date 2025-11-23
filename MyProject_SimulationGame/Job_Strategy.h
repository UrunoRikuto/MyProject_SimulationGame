/**************************************************//*
	@file	| Job_Strategy.h
	@brief	| 職業クラスのhファイル
	@note	| 職業インターフェースの処理を定義
			| ストラテジーパターンで作成
*//**************************************************/
#pragma once
#include <string>

// 前方宣言
class CHuman;

// @brief 職業クラスの基底インターフェース
class IJob_Strategy
{
public:
	// @brief 職業のステータス構造体
	struct JobStatus
	{
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
	virtual ~IJob_Strategy() = default;

	// @brief 職業ごとの仕事処理の純粋仮想関数
	virtual void DoWork() = 0;

	// @brief 職業名を取得する純粋仮想関数
	// @return 職業名の文字列
	virtual std::string GetJobName() const = 0;

	// @brief 所属しているオブジェクトの取得
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

protected:
	// @brief 所属しているオブジェクトのポインタ
	CHuman* m_pOwner = nullptr;

	// @brief 職業のステータス
	JobStatus m_Status;
};

