/**************************************************//*
	@file	| Profiler.h
	@brief	| 簡易プロファイラのhファイル
	@note	| 区間ごと・オブジェクトの型ごとの処理時間を計測し、レポートを出力する
			| 無効な間は計測を行わないため、負荷はほとんどかからない
			| 起動引数 --profile=秒数 で、自動で計測してレポートを出力し終了する
			| シングルトンパターンで作成
*//**************************************************/
#pragma once
#include "Singleton.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <typeinfo>

// @brief 簡易プロファイラクラス
class CProfiler : public ISingleton<CProfiler>
{
private:
	// @brief 計測項目
	struct Entry
	{
		// @brief 項目名
		std::string m_sName;
		// @brief 全期間の合計時間(ミリ秒)
		double m_fTotal = 0.0;
		// @brief 現在のフレームの合計時間(ミリ秒)
		double m_fFrame = 0.0;
		// @brief 現在の1秒間の合計時間(ミリ秒)
		double m_fWindow = 0.0;
		// @brief 1回の呼び出しで最も長かった時間(ミリ秒)
		double m_fMaxCall = 0.0;
		// @brief 呼び出し回数
		long long m_nCalls = 0;
	};

	// @brief 特に長かったフレームの記録
	struct WorstFrame
	{
		// @brief フレーム間隔(ミリ秒)
		double m_fInterval = 0.0;
		// @brief 何フレーム目か
		long long m_nFrameNo = 0;
		// @brief そのフレームで時間のかかった項目(名前と時間)
		std::vector<std::pair<std::string, double>> m_Top;
	};

public:
	// @brief 表示用の行
	struct Row
	{
		std::string m_sName;
		// @brief 1フレームあたりの平均時間(ミリ秒)
		double m_fAvgMs = 0.0;
		// @brief 1回の呼び出しで最も長かった時間(ミリ秒)
		double m_fMaxMs = 0.0;
		// @brief 1フレームあたりの平均呼び出し回数
		double m_fCallsPerFrame = 0.0;
	};

private:
	// @brief コンストラクタ
	CProfiler() = default;

	friend class ISingleton<CProfiler>;

public:
	// @brief 計測が有効かどうかの取得
	static bool IsEnabled() { return s_bEnabled; }

	// @brief 計測の有効/無効の設定
	void SetEnabled(bool bEnabled) { s_bEnabled = bEnabled; }

	// @brief 現在時刻の取得
	// @return ミリ秒単位の時刻(高精度タイマー)
	static double NowMs();

	// @brief 区間の処理時間の加算
	// @param pName：項目名(文字列リテラルなど、寿命が続く文字列)
	// @param fMs：処理時間(ミリ秒)
	void Add(const char* pName, double fMs);

	// @brief オブジェクトの型ごとの処理時間の加算
	// @param bDraw：true:描画 false:更新
	// @param type：オブジェクトの型
	// @param fMs：処理時間(ミリ秒)
	void AddObject(bool bDraw, const std::type_info& type, double fMs);

	// @brief GPUが1フレームの描画にかかった時間の通知
	// @param fMs：GPUの処理時間(ミリ秒)
	void SetGpuFrameTime(double fMs);

	// @brief 1フレームの終わりに呼び出す処理
	void EndFrame();

	// @brief 直近1秒間の平均を、時間の長い順で取得
	// @return 表示用の行リスト
	const std::vector<Row>& GetLastSecondRows() const { return m_LastSecondRows; }

	// @brief 直近1秒間の平均フレーム間隔の取得
	// @return フレーム間隔(ミリ秒)
	double GetLastSecondFrameMs() const { return m_fLastSecondFrameMs; }

	// @brief レポート文字列の作成
	std::string BuildReport() const;

	// @brief レポートのファイル出力
	// @param pPath：出力先のファイルパス
	// @return true:成功 false:失敗
	bool WriteReport(const char* pPath) const;

	// @brief 自動計測の開始
	// @param fSeconds：計測する秒数
	// @param pReportPath：レポートの出力先
	void StartAutoRun(float fSeconds, const char* pReportPath);

	// @brief 自動計測の確認
	// @return true:計測時間が終わり、レポートを出力した
	bool UpdateAutoRun();

private:
	// @brief 項目を探す(無ければ作る)
	Entry& FindEntry(std::vector<Entry>& entries, std::unordered_map<const void*, size_t>& index, const void* pKey, const char* pName);

	// @brief 項目に処理時間を加算する
	static void Accumulate(Entry& entry, double fMs);

	// @brief 長かったフレームの記録
	void RecordWorstFrame(double fInterval);

	// @brief 直近1秒間の集計
	void UpdateLastSecond(double fNow);

	// @brief 同じ名前の項目をまとめて、表示用の行にする
	std::vector<Row> BuildRows(const std::vector<Entry>& entries, const char* pPrefix, bool bWindow, double fFrames) const;

private:
	// @brief 計測が有効かどうか
	inline static bool s_bEnabled = false;

	// @brief 20msを超えた遅いフレームの数
	long long m_nSlowFrames = 0;

	// @brief 遅いフレームのうち、更新・描画の処理時間が間隔の半分未満だった(待ち・メッセージ処理・OSの割り込みなど、処理の外で時間がかかった)フレームの数
	long long m_nSlowOutsideFrames = 0;

	// @brief 区間の項目
	std::vector<Entry> m_Sections;
	std::unordered_map<const void*, size_t> m_SectionIndex;

	// @brief オブジェクトの更新の項目(型ごと)
	std::vector<Entry> m_Updates;
	std::unordered_map<const void*, size_t> m_UpdateIndex;

	// @brief オブジェクトの描画の項目(型ごと)
	std::vector<Entry> m_Draws;
	std::unordered_map<const void*, size_t> m_DrawIndex;

	// @brief 全フレームの間隔(ミリ秒)
	std::vector<float> m_FrameTimes;

	// @brief 長かったフレームの記録
	std::vector<WorstFrame> m_WorstFrames;

	// @brief GPUの1フレームの処理時間の合計・回数・最大
	double m_fGpuTotal = 0.0;
	double m_fGpuMax = 0.0;
	long long m_nGpuCount = 0;

	// @brief 前のフレームの終了時刻(ミリ秒)
	double m_fLastFrameEnd = 0.0;

	// @brief 計測したフレーム数
	long long m_nFrames = 0;

	// @brief 計測を開始した時刻(ミリ秒)
	double m_fStartTime = 0.0;

	// @brief 直近1秒間の集計用
	double m_fWindowStart = 0.0;
	long long m_nWindowFrames = 0;
	double m_fLastSecondFrameMs = 0.0;
	std::vector<Row> m_LastSecondRows;

	// @brief 自動計測
	bool m_bAutoRun = false;
	double m_fAutoStart = 0.0;
	float m_fAutoSeconds = 0.0f;
	std::string m_sReportPath;
};

// @brief 区間の処理時間を、スコープを抜けた時に自動で加算するクラス
class CProfileScope
{
public:
	// @brief コンストラクタ
	// @param pName：項目名(文字列リテラル)
	explicit CProfileScope(const char* pName)
		: m_pName(pName)
		, m_fStart(0.0)
		, m_bActive(CProfiler::IsEnabled())
	{
		if (m_bActive) m_fStart = CProfiler::NowMs();
	}

	// @brief デストラクタ
	~CProfileScope()
	{
		if (m_bActive) CProfiler::GetInstance()->Add(m_pName, CProfiler::NowMs() - m_fStart);
	}

	CProfileScope(const CProfileScope&) = delete;
	CProfileScope& operator=(const CProfileScope&) = delete;

private:
	const char* m_pName;
	double m_fStart;
	bool m_bActive;
};

// @brief スコープの処理時間を計測するマクロ
#define PROFILE_CONCAT_INNER(a, b) a##b
#define PROFILE_CONCAT(a, b) PROFILE_CONCAT_INNER(a, b)
#define PROFILE_SCOPE(name) CProfileScope PROFILE_CONCAT(profileScope_, __LINE__)(name)

// @brief オブジェクトの更新/描画を、計測が有効な場合だけ時間を測って呼び出す
// @param bDraw：true:描画 false:更新
// @param pObj：対象のオブジェクト(型ごとに集計する)
// @param func：実行する処理
template<typename T, typename F>
inline void ProfileObject(bool bDraw, T* pObj, F&& func)
{
	if (!CProfiler::IsEnabled())
	{
		func();
		return;
	}

	// 処理の中でオブジェクトが解放されることはないが、念のため先に型を取得しておく
	const std::type_info& type = typeid(*pObj);
	const double fStart = CProfiler::NowMs();
	func();
	CProfiler::GetInstance()->AddObject(bDraw, type, CProfiler::NowMs() - fStart);
}
