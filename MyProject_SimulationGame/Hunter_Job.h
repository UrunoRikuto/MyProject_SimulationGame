/**************************************************//*
	@file	| Hunter_Job.h
	@brief	| 狩猟職業クラスのhファイル
	@note	| 動物を狩って肉を得る職業の処理を定義
			| CCollect_Strategyを継承
*//**************************************************/
#pragma once
#include "Collect_Strategy.h"
#include "GameObject.h"
#include <string>

// 前方宣言
class CAnimal;

// @brief 職業名を管理する名前空間
namespace JobName
{
	const std::string Hunter = u8"狩猟職";
}

// @brief 動物を狩って肉を得る職業クラス
class CHunter_Job final : public CCollect_Strategy
{
private:
	// @brief 仕事状態
	enum class WorkState
	{
		// 獲物を探す
		SearchPrey,
		// 獲物を追って攻撃する
		Hunting,
		// 肉を貯蔵庫に運ぶ
		Transporting,
		// 貯蔵庫に肉を預ける
		Storing,
		// 休憩中
		Resting
	};

public:
	// @brief コンストラクタ
	CHunter_Job();

	// @brief 仕事処理
	virtual void DoWork() override;

	// @brief 職業切り替え時の処理
	virtual void OnChangeJob() override;

	// @brief インスペクター表示処理
	// @param isEnd：true:ImGuiのEnd()を呼ぶ false:呼ばない
	// @return 表示した項目数
	virtual int Inspecter(bool isEnd = true) override;

	// @brief 職業ステータスのImGui描画処理
	virtual void DrawJobStatusImGui() override;

	// @brief 職業名を取得するオーバーライド関数
	// @return 職業名の文字列
	std::string GetJobName() const override { return JobName::Hunter; }

	// @brief 収集ツール(槍)を持っているかどうかを取得
	// @return true:持っている false:持っていない
	virtual bool HasCollectTool() override;

	// @brief 狩りの結果の集計(バランス確認用)
	struct HuntStats
	{
		// 獲物を探した回数
		int nSearch = 0;
		// 探索範囲内に草食動物がいなかった回数
		int nNoHerbivore = 0;
		// 草食動物はいたが、周囲が危険で襲えなかった回数
		int nTooDangerous = 0;
		// 狩りを始めた回数
		int nStarted = 0;
		// 獲物を倒した回数
		int nKill = 0;
		// 追跡時間が長すぎて諦めた回数
		int nGiveUpTime = 0;
		// 獲物が遠くへ離れすぎて諦めた回数
		int nGiveUpFar = 0;
		// 獲物が他の動物に倒されるなどして、いなくなった回数
		int nPreyLost = 0;
	};

	// @brief 狩りの結果の集計の取得
	static const HuntStats& GetStats() { return s_Stats; }

	// @brief 自身が必要な収集ツールのアイテムタイプを取得するオーバーライド関数
	// @return 槍のアイテムタイプ
	CItem::ITEM_TYPE GetRequiredCollectToolType() const override { return CItem::ITEM_TYPE::Spear; }

private:
	// @brief 獲物を探す処理
	void SearchPreyAction();

	// @brief 獲物を追って攻撃する処理
	void HuntingAction();

	// @brief 肉を貯蔵庫に運ぶ処理
	void TransportingAction();

	// @brief 貯蔵庫に肉を預ける処理
	void StoringAction();

	// @brief 休憩中の処理
	void RestingAction();

	// @brief 槍を用意する処理
	// @return true:槍の用意を待たずに獲物探索へ進める false:槍を取りに移動中
	bool PrepareTool();

	// @brief 襲う獲物を選ぶ処理
	// @return 獲物のポインタ、襲える獲物がいない場合はnullptr
	CAnimal* SelectPrey();

	// @brief 獲物の周囲の危険度を計算する処理
	// @param pPrey：獲物のポインタ
	// @return 周囲にいる肉食動物の攻撃力の合計
	float CalcDanger(CAnimal* pPrey) const;

	// @brief 集団で戦える戦力を計算する処理
	// @return 狩猟職の人数(文明レベルによる上限あり)と労働力から求めた戦力
	float CalcGroupPower() const;

	// @brief 現在の獲物を取得する処理
	// @return 獲物のポインタ、倒された/いなくなった場合はnullptr
	CAnimal* ResolvePrey();

	// @brief 獲物を設定する処理
	void SetPrey(CAnimal* pPrey);

	// @brief 獲物の設定を解除する処理
	void ClearPrey();

	// @brief 仕事状態を切り替える処理
	void ChangeState(WorkState eNext) { m_ePrevState = m_eCurrentState; m_eCurrentState = eNext; }

	// @brief 現在の仕事状態名を取得する処理
	const char* GetStateName() const;

private:
	// @brief 狩りの結果の集計
	inline static HuntStats s_Stats;

	// @brief 現在の仕事状態
	WorkState m_eCurrentState = WorkState::SearchPrey;
	// @brief 前の仕事状態
	WorkState m_ePrevState = WorkState::SearchPrey;

	// @brief 獲物を持っているかどうか
	bool m_bHasPrey = false;
	// @brief 獲物のオブジェクトID
	// @note 他の動物に倒されて破棄される場合があるため、ポインタではなくIDで保持する
	ObjectID m_PreyID{ "", -1 };

	// @brief 攻撃のクールダウン時間(秒)
	float m_fAttackCooldown = 0.0f;
	// @brief 現在の獲物を追っている時間(秒)
	float m_fChaseTime = 0.0f;
	// @brief 獲物探索の間隔タイマー(秒)
	float m_fSearchTimer = 0.0f;
};
