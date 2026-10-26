/**************************************************//*
	@file	| TutorialManager.h
	@brief	| チュートリアル管理クラスのhファイル
	@note	| 本編の画面の上に、手順ごとの説明とハイライトを重ねて表示するチュートリアル
			| プレイヤーが実際に操作すると、次の手順へ進む(操作できない場合は手順ごとにスキップできる)
			| 初めて起動した時だけ自動で始まり、右下のボタンでいつでも見直せる
*//**************************************************/
#pragma once
#include <vector>
#include <DirectXMath.h>

struct ImFont;

// @brief チュートリアル管理クラス
class CTutorialManager
{
public:
	// @brief チュートリアルを有効にするかどうかの設定
	// @param bEnabled：false:有効にしない(自動計測の時など)
	static void SetEnabled(bool bEnabled) { s_bEnabled = bEnabled; }

	// @brief 毎フレームの更新処理(ゲームシーン中の、手順の進行判定など)
	static void Update();

	// @brief 描画処理(ImGuiのウィンドウを描画する)
	// @param pFont：表示に使うフォント
	static void Draw(ImFont* pFont);

	// @brief チュートリアルを最初から始める
	static void Start();

	// @brief チュートリアルの途中かどうかの取得
	static bool IsActive() { return s_bActive; }

private:
	// @brief 手順を進める条件
	enum class Condition
	{
		Manual,			// 「次へ」ボタンで進む
		CameraMoved,	// カメラを動かす
		HumanSelected,	// 人間を選ぶ
		JobAssigned,	// 選んだ人間に職業を割り当てる
		StoredItem,		// 貯蔵庫に資源が運ばれる
		BuilderAssigned,// 建築職の人間がいる
		SpeedChanged,	// ゲーム速度を変える
	};

	// @brief ハイライトする画面の範囲
	enum class Area
	{
		None,			// なし
		HumanSelect,	// 右上の人間選択
		HumanWindow,	// 右の人間の情報全体
		CivLevel,		// 左上の文明レベル
		GameTime,		// 左の時間・季節
		GameSpeed,		// 上部中央のゲーム速度
		BuildWindow,	// 左下の建築物(貯蔵庫)
	};

	// @brief 1つの手順
	struct Step
	{
		const char* pTitle;
		const char* pBody;
		Condition eCondition;
		Area eArea;
	};

private:
	// @brief 手順のリスト
	static const std::vector<Step>& GetSteps();

	// @brief 手順を始めた時の処理(基準値の記録など)
	static void EnterStep(int nIndex);

	// @brief 手順の進行条件を満たしているかどうかの判定
	static bool CheckCondition(Condition eCondition);

	// @brief 次の手順へ進む(最後の手順の場合はチュートリアルを終える)
	static void NextStep();

	// @brief チュートリアルを終える
	// @param bMarkDone：true:完了として記録する(次回から自動では始めない)
	static void Finish(bool bMarkDone);

	// @brief 完了済みかどうかの記録の読み込み
	static bool IsDoneRecorded();

	// @brief 完了したことの記録
	static void RecordDone();

	// @brief ゲームシーンかどうか
	static bool IsGameScene();

private:
	static inline bool s_bEnabled = true;
	static inline bool s_bActive = false;
	// ゲームシーンに入ってから、自動で始めるかどうかの確認を済ませたか
	static inline bool s_bAutoChecked = false;
	static inline int s_nStep = 0;
	// 進行条件を満たしてから、次の手順へ進むまでの時間(秒)。負の値は条件を満たしていない
	static inline float s_fCompleteTimer = -1.0f;

	// 手順を始めた時の基準値
	static inline DirectX::XMFLOAT3 s_f3LastCameraPos = { 0.0f, 0.0f, 0.0f };
	static inline DirectX::XMFLOAT3 s_f3LastCameraLook = { 0.0f, 0.0f, 0.0f };
	static inline float s_fCameraMoved = 0.0f;
	static inline int s_nBaseStoredCount = 0;
	static inline int s_nBaseGameSpeed = 1;
};
