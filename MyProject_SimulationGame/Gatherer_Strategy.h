/**************************************************//*
	@file	| CGatherer_Strategy.h
	@brief	| 採集職業クラスのhファイル
	@note	| 探して採取する職の処理を実装
			| CCollect_Strategyを継承
*//**************************************************/
#pragma once
#include "Collect_Strategy.h"
#include "CollectTarget.h"

// @brief 探して採取する採取職業クラス
class CGatherer_Strategy : public CCollect_Strategy
{
private:
	// @brief 仕事状態
	enum class WorkState
	{
		// 探して移動
		SearchAndMove,
		// 採取中
		Gathering,
		// 素材アイテムを貯蔵庫に運搬中
		Transporting,
		// 貯蔵庫に貯蔵中
		Storing,
		// 休憩中
		Resting
	};

public:
	// @brief 仕事処理
	virtual void DoWork() override;

	// @brief 切り替え処理
	virtual void OnChangeJob() override;

	// @brief インスペクター表示処理
	// @param isEnd：true:ImGuiのEnd()を呼ぶ false:呼ばない
	// @return 表示した項目数
	virtual int Inspecter(bool isEnd = true) override;

private:

	// @brief 探索＆移動処理
	void SearchAndMoveAction();

	// @brief 採取処理
	void GatheringAction();

	// @brief 運搬処理
	void TransportingAction();

	// @brief 貯蔵処理
	void StoringAction();

	// @brief 休憩処理
	void RestingAction();


protected:

	// @brief 標的を探す処理
	// @param vNotTargetIDs：ターゲティングIDが設定されているオブジェクトのIDリスト
	// @return 採取対象オブジェクトのポインタ
	virtual CCollectTarget* SearchTarget(std::vector<ObjectID> vNotTargetIDs) = 0;

private:

	// @brief 現在の仕事状態
	WorkState m_eCurrentState = WorkState::SearchAndMove;
	// @brief 前の仕事状態
	WorkState m_ePrevState = WorkState::SearchAndMove;

	// @brief 標的にしている採取対象オブジェクトのポインタ
	CCollectTarget* m_pTarget = nullptr;

};

