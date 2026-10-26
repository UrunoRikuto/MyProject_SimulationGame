/**************************************************//*
	@file	| Pioneer_Job.h
	@brief	| 開拓職業クラスのhファイル
	@note	| 建築可能な土地を広げる職業の処理を定義
			| CCrafter_Strategyを継承
*//**************************************************/
#pragma once
#include "Crafter_Strategy.h"
#include "GameObject.h"
#include <string>
#include <DirectXMath.h>

// 前方宣言
class CFieldCell;
class CCollectTarget;

// @brief 職業名を管理する名前空間
namespace JobName
{
	const std::string Pioneer = u8"開拓職";
}

// @brief 建築可能な土地を広げる職業クラス
class CPioneer_Job final : public CCrafter_Strategy
{
private:
	// @brief 仕事状態
	enum class WorkState
	{
		// 開拓する土地を探す
		Searching,
		// 開拓する土地へ移動
		Moving,
		// 開拓中
		Clearing,
		// 休憩中
		Resting
	};

public:
	// @brief コンストラクタ
	CPioneer_Job();

	// @brief デストラクタ
	~CPioneer_Job();

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
	std::string GetJobName() const override { return JobName::Pioneer; }

	// @brief 開拓予定の土地の予約を全て解除する(新しいゲームを始めるときに呼ぶ)
	static void ResetClaims();

private:
	// @brief 開拓する土地を探す処理
	void SearchingAction();

	// @brief 開拓する土地へ移動する処理
	void MovingAction();

	// @brief 開拓中の処理
	void ClearingAction();

	// @brief 休憩中の処理
	void RestingAction();

	// @brief 開拓する土地を決定する処理
	// @return true:決定した false:開拓できる土地が無い
	bool FindClearTarget();

	// @brief 開拓する土地のセルを取得する処理
	// @return セルのポインタ、開拓する土地が決まっていない場合はnullptr
	CFieldCell* GetTargetCell() const;

	// @brief 開拓する土地に残っている採取対象(木・岩・草)を取得する処理
	// @return 採取対象のポインタ、無い/既に無くなっている場合はnullptr
	CCollectTarget* ResolveOccupant() const;

	// @brief 開拓する土地の予約を解除する処理
	// @note デストラクタからも呼ばれるため、シーンのオブジェクトには触れない
	void ReleaseClaim();

	// @brief 指定したセルの位置まで移動する処理
	// @return true:到達 false:到達していない
	bool MoveToCell(CFieldCell* pCell);

	// @brief 現在の建築可能な空きセル数を取得する処理
	int GetFreeBuildCellCount() const;

	// @brief 文明レベルに応じて維持したい建築可能な空きセル数を取得する処理
	int GetTargetFreeCellCount() const;

	// @brief 仕事状態を切り替える処理
	void ChangeState(WorkState eNext) { m_ePrevState = m_eCurrentState; m_eCurrentState = eNext; }

	// @brief 現在の仕事状態名を取得する処理
	const char* GetStateName() const;

private:
	// @brief 現在の仕事状態
	WorkState m_eCurrentState = WorkState::Searching;
	// @brief 前の仕事状態
	WorkState m_ePrevState = WorkState::Searching;

	// @brief 開拓する土地が決まっているかどうか
	bool m_bHasTarget = false;
	// @brief 開拓する土地のセルインデックス
	DirectX::XMINT2 m_n2TargetIndex = DirectX::XMINT2(-1, -1);

	// @brief 開拓する土地に採取対象(木・岩・草)が残っているかどうか
	bool m_bHasOccupant = false;
	// @brief 開拓する土地に残っている採取対象のオブジェクトID
	// @note 採取職に先に採取されて破棄される場合があるため、ポインタではなくIDで保持する
	ObjectID m_OccupantID{ "", -1 };

	// @brief 開拓の進行度
	float m_fProgress = 0.0f;
	// @brief 土地探索の間隔タイマー(秒)
	float m_fSearchTimer = 0.0f;
};
