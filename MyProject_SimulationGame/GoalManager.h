/**************************************************//*
	@file	| GoalManager.h
	@brief	| 目標管理クラスのhファイル
	@note	| ゲームの目標「1年後に訪れる大飢餓を、一定数以上の人間が生き残って乗り越える」を管理する
			| 大飢餓の少し前に予兆を知らせ、大飢餓の間は食料の供給が大きく減る
			| 大飢餓が終わった時点の生存者数で、クリアかゲームオーバーかを決める
*//**************************************************/
#pragma once
#include <string>

struct ImFont;

// @brief 目標管理クラス
class CGoalManager
{
public:
	// @brief 大飢餓が始まるゲーム内の日数(0日目からの経過日数)
	// @note 1年は40日なので、ちょうど1年後。木の実が実る春(10日ごとに切り替わる季節の1つ目)に訪れる大飢餓になる
	static constexpr int Famine_Start_Day = 40;

	// @brief 大飢餓が続く日数
	static constexpr int Famine_Duration_Days = 10;

	// @brief 大飢餓が始まる何日前から予兆を知らせるか
	static constexpr int Warning_Lead_Days = 10;

	// @brief クリアに必要な、大飢餓が終わった時点の生存者数
	static constexpr int Required_Survivors = 20;

	// @brief 大飢餓の間に、木の実(リンゴ)が取れる確率(%)
	static constexpr int Famine_Supply_Percent = 40;

	// @brief 大飢餓の間の、農作物が育つ速さの割合(%)
	static constexpr int Famine_Crop_Growth_Percent = 50;

	// @brief 荒廃した見た目に変わり始める、大飢餓が始まる何日前か(予兆の間に、少しずつ変わっていく)
	static constexpr float Visual_Ramp_In_Days = 2.0f;

	// @brief 大飢餓が終わってから、元の見た目に戻るまでの日数
	static constexpr float Visual_Ramp_Out_Days = 3.0f;

	// @brief 大飢餓が始まったときに、いなくなる鹿と狼の割合(%)
	// @note 狼も減らさないと、残った鹿が狼に狩り尽くされてしまう
	static constexpr int Animal_Loss_Percent = 50;

public:
	// @brief 目標の進行の段階
	enum class Phase
	{
		Normal,		// 大飢餓はまだ先
		Warning,	// 大飢餓の予兆(大飢餓の少し前)
		Famine,		// 大飢餓の最中
		Finished,	// 大飢餓が終わった(クリアかゲームオーバーが決まっている)
	};

public:
	// @brief 新しいゲームを始めるときの初期化
	static void Start();

	// @brief 更新処理(ゲームの更新1回につき1回呼ぶ)
	static void Update();

	// @brief 描画処理(目標の表示、お知らせ、結果の画面)
	// @param pFont：表示に使うフォント
	static void Draw(ImFont* pFont);

	// @brief 結果の画面などを出さずに、目標の進行だけを行う設定(自動計測の時など)
	// @param bHeadless：true:画面を出さず、ゲームも止めない
	static void SetHeadless(bool bHeadless) { s_bHeadless = bHeadless; }

	// @brief 結果の画面を表示していて、ゲームの進行を止めるべきかどうかの取得
	static bool IsPaused() { return s_bResultOpen; }

	// @brief 木の実が取れる確率の取得(平常時は1.0、大飢餓の間はFamine_Supply_Percentに応じて減る)
	static float GetSupplyRate() { return s_ePhase == Phase::Famine ? static_cast<float>(Famine_Supply_Percent) / 100.0f : 1.0f; }

	// @brief 農作物が育つ速さの割合の取得(平常時は1.0、大飢餓の間はFamine_Crop_Growth_Percentに応じて遅くなる)
	static float GetCropGrowthRate() { return s_ePhase == Phase::Famine ? static_cast<float>(Famine_Crop_Growth_Percent) / 100.0f : 1.0f; }

	// @brief 荒廃した見た目の強さの取得(0:通常の見た目 1:完全に荒廃した見た目)
	// @note 大飢餓が近づくと少しずつ強くなり、終わると少しずつ戻る。空・地面・木・草の色に使う
	static float GetFamineVisualRate() { return s_fFamineVisual; }

	// @brief 大飢餓の最中かどうかの取得
	// @note 木の実が取れにくい、木・鹿・狼が補充されない、などの判定に使う
	static bool IsFamine() { return s_ePhase == Phase::Famine; }

	// @brief 現在の段階の取得
	static Phase GetPhase() { return s_ePhase; }

private:
	// @brief 結果
	enum class Result
	{
		None,		// まだ決まっていない
		Cleared,	// クリア
		Failed,		// ゲームオーバー
		Continued,	// クリア後に、続けて遊んでいる
	};

	// @brief 結果の画面に出す集計
	struct Stats
	{
		int nDays = 0;				// 経過日数
		int nSurvivors = 0;			// 生存者数
		int nPeakPopulation = 0;	// 最も多かったときの人口
		int nCivLevel = 0;			// 文明レベル
		int nFacilities = 0;		// 建てた施設の数
		int nDeaths = 0;			// 亡くなった人間の数
		int nFoodAtFamineStart = 0;	// 大飢餓が始まったときの食料の備蓄数
	};

private:
	// @brief ゲームシーン中かどうか
	static bool IsGameScene();

	// @brief ゲーム内の経過日数(小数部あり)の取得
	static float GetElapsedDays();

	// @brief 生存している人間の数を数える
	static int CountSurvivors();

	// @brief 貯蔵庫の食料の数を数える
	static int CountFoodStock();

	// @brief 建築が完了している施設の数を数える
	static int CountFacilities();

	// @brief 人口と食料の表示用の値を最新にする
	static void RefreshCounts();

	// @brief 大飢餓が始まったときの処理(鹿を減らす)
	static void OnFamineStart();

	// @brief 大飢餓が終わったときの、結果の判定
	static void Judge();

	// @brief お知らせを表示する
	// @param sTitle：見出し
	// @param sBody：本文
	static void ShowNotice(const std::string& sTitle, const std::string& sBody);

	// @brief 目標の表示
	static void DrawHud();

	// @brief お知らせの表示
	static void DrawNotice();

	// @brief 結果の画面の表示
	static void DrawResult();

private:
	inline static Phase s_ePhase = Phase::Normal;
	// @brief 荒廃した見た目の強さ(0～1)
	inline static float s_fFamineVisual = 0.0f;
	inline static Result s_eResult = Result::None;
	inline static bool s_bHeadless = false;
	// @brief 結果の画面を表示中かどうか
	inline static bool s_bResultOpen = false;
	// @brief ゲームの更新の回数(人口などの集計を、毎回ではなく一定間隔で行うため)
	inline static int s_nTick = 0;
	// @brief 表示用の人口と食料の数(一定間隔で更新)
	inline static int s_nPopulation = 0;
	inline static int s_nFoodStock = 0;
	// @brief ゲーム開始時点の、亡くなった人間の累計(今回のゲームだけの数にするため)
	inline static int s_nBaseDeathCount = 0;
	inline static Stats s_Stats;

	// @brief お知らせ
	inline static std::string s_sNoticeTitle;
	inline static std::string s_sNoticeBody;
	// @brief お知らせを表示し始めた時刻(ImGuiの時刻。負の場合は表示なし)
	inline static double s_dNoticeStart = -1.0;
};
