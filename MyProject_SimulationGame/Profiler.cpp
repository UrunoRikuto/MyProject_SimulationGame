/**************************************************//*
	@file	| Profiler.cpp
	@brief	| 簡易プロファイラのcppファイル
	@note	| 区間ごと・オブジェクトの型ごとの処理時間を計測し、レポートを出力する
*//**************************************************/
#include "Profiler.h"
#include <Windows.h>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <map>

namespace
{
	// @brief 長かったフレームとして記録する件数
	constexpr size_t Worst_Frame_Count = 3;

	// @brief 長かったフレームの記録に載せる項目数
	constexpr size_t Worst_Frame_Top_Count = 8;

	// @brief 型名から先頭の "class " や "struct " を取り除く
	std::string MakeTypeName(const std::type_info& type)
	{
		std::string name = type.name();
		const char* prefixes[] = { "class ", "struct " };
		for (const char* prefix : prefixes)
		{
			const size_t len = strlen(prefix);
			if (name.compare(0, len, prefix) == 0)
			{
				return name.substr(len);
			}
		}
		return name;
	}
}

/*****************************************//*
	@brief　	| 現在時刻の取得
	@return		| ミリ秒単位の時刻
*//*****************************************/
double CProfiler::NowMs()
{
	static const double fPeriod = []()
	{
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
		return 1000.0 / static_cast<double>(freq.QuadPart);
	}();

	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return static_cast<double>(counter.QuadPart) * fPeriod;
}

/*****************************************//*
	@brief　	| 項目を探す(無ければ作る)
*//*****************************************/
CProfiler::Entry& CProfiler::FindEntry(std::vector<Entry>& entries, std::unordered_map<const void*, size_t>& index, const void* pKey, const char* pName)
{
	auto it = index.find(pKey);
	if (it != index.end())
	{
		return entries[it->second];
	}

	Entry entry;
	entry.m_sName = pName;
	entries.push_back(entry);
	index[pKey] = entries.size() - 1;
	return entries.back();
}

/*****************************************//*
	@brief　	| 項目に処理時間を加算する
*//*****************************************/
void CProfiler::Accumulate(Entry& entry, double fMs)
{
	entry.m_fTotal += fMs;
	entry.m_fFrame += fMs;
	entry.m_fWindow += fMs;
	entry.m_nCalls++;
	if (fMs > entry.m_fMaxCall) entry.m_fMaxCall = fMs;
}

/*****************************************//*
	@brief　	| 区間の処理時間の加算
*//*****************************************/
void CProfiler::Add(const char* pName, double fMs)
{
	Accumulate(FindEntry(m_Sections, m_SectionIndex, pName, pName), fMs);
}

/*****************************************//*
	@brief　	| オブジェクトの型ごとの処理時間の加算
*//*****************************************/
void CProfiler::AddObject(bool bDraw, const std::type_info& type, double fMs)
{
	std::vector<Entry>& entries = bDraw ? m_Draws : m_Updates;
	std::unordered_map<const void*, size_t>& index = bDraw ? m_DrawIndex : m_UpdateIndex;

	auto it = index.find(&type);
	if (it == index.end())
	{
		Entry entry;
		entry.m_sName = MakeTypeName(type);
		entries.push_back(entry);
		it = index.emplace(&type, entries.size() - 1).first;
	}
	Accumulate(entries[it->second], fMs);
}

/*****************************************//*
	@brief　	| GPUが1フレームの描画にかかった時間の通知
*//*****************************************/
void CProfiler::SetGpuFrameTime(double fMs)
{
	m_fGpuTotal += fMs;
	m_nGpuCount++;
	if (fMs > m_fGpuMax) m_fGpuMax = fMs;
}

/*****************************************//*
	@brief　	| 長かったフレームの記録
	@param		| fInterval：フレーム間隔(ミリ秒)
*//*****************************************/
void CProfiler::RecordWorstFrame(double fInterval)
{
	// 記録に載る長さかどうか
	if (m_WorstFrames.size() >= Worst_Frame_Count && fInterval <= m_WorstFrames.back().m_fInterval) return;

	// このフレームで時間のかかった項目を集める
	std::vector<std::pair<std::string, double>> items;
	for (const Entry& e : m_Sections)
	{
		if (e.m_fFrame > 0.0) items.push_back({ e.m_sName, e.m_fFrame });
	}
	for (const Entry& e : m_Updates)
	{
		if (e.m_fFrame > 0.0) items.push_back({ "Update:" + e.m_sName, e.m_fFrame });
	}
	for (const Entry& e : m_Draws)
	{
		if (e.m_fFrame > 0.0) items.push_back({ "Draw:" + e.m_sName, e.m_fFrame });
	}
	std::sort(items.begin(), items.end(), [](const auto& a, const auto& b) { return a.second > b.second; });
	if (items.size() > Worst_Frame_Top_Count) items.resize(Worst_Frame_Top_Count);

	WorstFrame worst;
	worst.m_fInterval = fInterval;
	worst.m_nFrameNo = m_nFrames;
	worst.m_Top = items;

	m_WorstFrames.push_back(worst);
	std::sort(m_WorstFrames.begin(), m_WorstFrames.end(), [](const WorstFrame& a, const WorstFrame& b) { return a.m_fInterval > b.m_fInterval; });
	if (m_WorstFrames.size() > Worst_Frame_Count) m_WorstFrames.resize(Worst_Frame_Count);
}

/*****************************************//*
	@brief　	| 同じ名前の項目をまとめて、表示用の行にする
	@param		| entries：項目のリスト
	@param		| pPrefix：項目名の先頭に付ける文字列
	@param		| bWindow：true:直近1秒間の合計を使う false:全期間の合計を使う
	@param		| fFrames：平均を求めるフレーム数
*//*****************************************/
std::vector<CProfiler::Row> CProfiler::BuildRows(const std::vector<Entry>& entries, const char* pPrefix, bool bWindow, double fFrames) const
{
	struct Sum { double total = 0.0; double maxCall = 0.0; long long calls = 0; };
	std::map<std::string, Sum> merged;
	for (const Entry& e : entries)
	{
		Sum& s = merged[std::string(pPrefix) + e.m_sName];
		s.total += bWindow ? e.m_fWindow : e.m_fTotal;
		s.maxCall = (std::max)(s.maxCall, e.m_fMaxCall);
		s.calls += e.m_nCalls;
	}

	std::vector<Row> rows;
	const double fDiv = fFrames > 0.0 ? fFrames : 1.0;
	for (const auto& kv : merged)
	{
		Row row;
		row.m_sName = kv.first;
		row.m_fAvgMs = kv.second.total / fDiv;
		row.m_fMaxMs = kv.second.maxCall;
		row.m_fCallsPerFrame = static_cast<double>(kv.second.calls) / fDiv;
		rows.push_back(row);
	}
	std::sort(rows.begin(), rows.end(), [](const Row& a, const Row& b) { return a.m_fAvgMs > b.m_fAvgMs; });
	return rows;
}

/*****************************************//*
	@brief　	| 直近1秒間の集計
*//*****************************************/
void CProfiler::UpdateLastSecond(double fNow)
{
	if (m_fWindowStart <= 0.0)
	{
		m_fWindowStart = fNow;
		return;
	}
	if (fNow - m_fWindowStart < 1000.0) return;

	const double fFrames = static_cast<double>(m_nWindowFrames);
	m_fLastSecondFrameMs = fFrames > 0.0 ? (fNow - m_fWindowStart) / fFrames : 0.0;

	// 区間・更新・描画をまとめて、時間の長い順に並べる
	m_LastSecondRows.clear();
	for (const Row& r : BuildRows(m_Sections, "", true, fFrames)) m_LastSecondRows.push_back(r);
	for (const Row& r : BuildRows(m_Updates, "Update:", true, fFrames)) m_LastSecondRows.push_back(r);
	for (const Row& r : BuildRows(m_Draws, "Draw:", true, fFrames)) m_LastSecondRows.push_back(r);
	std::sort(m_LastSecondRows.begin(), m_LastSecondRows.end(), [](const Row& a, const Row& b) { return a.m_fAvgMs > b.m_fAvgMs; });

	// 1秒間の集計をリセット
	for (Entry& e : m_Sections) e.m_fWindow = 0.0;
	for (Entry& e : m_Updates) e.m_fWindow = 0.0;
	for (Entry& e : m_Draws) e.m_fWindow = 0.0;
	m_fWindowStart = fNow;
	m_nWindowFrames = 0;
}

/*****************************************//*
	@brief　	| 1フレームの終わりに呼び出す処理
*//*****************************************/
void CProfiler::EndFrame()
{
	if (!s_bEnabled) return;

	const double fNow = NowMs();
	if (m_fStartTime <= 0.0) m_fStartTime = fNow;

	if (m_fLastFrameEnd > 0.0)
	{
		const double fInterval = fNow - m_fLastFrameEnd;
		m_FrameTimes.push_back(static_cast<float>(fInterval));
		m_nFrames++;
		m_nWindowFrames++;
		RecordWorstFrame(fInterval);

		// 遅いフレームのうち、更新・描画・待ち・メッセージ処理のどれにも計上されない時間が半分以上のもの(OSによる停止など)を数える
		if (fInterval > 20.0)
		{
			double fWork = 0.0;
			for (const Entry& e : m_Sections)
			{
				if (e.m_sName == "Main::Update" || e.m_sName == "Main::Draw" || e.m_sName == "Loop: Wait" || e.m_sName == "Loop: Messages") fWork += e.m_fFrame;
			}
			m_nSlowFrames++;
			if (fWork < fInterval * 0.5) m_nSlowOutsideFrames++;
		}
	}
	m_fLastFrameEnd = fNow;

	UpdateLastSecond(fNow);

	// 現在のフレームの集計をリセット
	for (Entry& e : m_Sections) e.m_fFrame = 0.0;
	for (Entry& e : m_Updates) e.m_fFrame = 0.0;
	for (Entry& e : m_Draws) e.m_fFrame = 0.0;
}

/*****************************************//*
	@brief　	| レポート文字列の作成
	@return		| レポート
*//*****************************************/
std::string CProfiler::BuildReport() const
{
	std::string out;
	char buf[512];

	// フレーム間隔の統計
	std::vector<float> sorted = m_FrameTimes;
	std::sort(sorted.begin(), sorted.end());
	const size_t n = sorted.size();
	double fSum = 0.0;
	for (float f : sorted) fSum += f;
	const double fAvg = n > 0 ? fSum / static_cast<double>(n) : 0.0;
	auto percentile = [&](double p) -> double
	{
		if (n == 0) return 0.0;
		size_t i = static_cast<size_t>(p * static_cast<double>(n - 1));
		return sorted[i];
	};
	size_t over20 = 0, over33 = 0, over50 = 0;
	for (float f : sorted)
	{
		if (f > 20.0f) over20++;
		if (f > 33.4f) over33++;
		if (f > 50.0f) over50++;
	}

	snprintf(buf, sizeof(buf), "==== Profile report ====\n");
	out += buf;
	snprintf(buf, sizeof(buf), "frames: %zu   duration: %.1f s   avg FPS: %.1f\n", n, fSum / 1000.0, fAvg > 0.0 ? 1000.0 / fAvg : 0.0);
	out += buf;
	snprintf(buf, sizeof(buf), "frame interval (ms): avg %.2f  p50 %.2f  p95 %.2f  p99 %.2f  max %.2f\n", fAvg, percentile(0.5), percentile(0.95), percentile(0.99), n > 0 ? sorted.back() : 0.0f);
	out += buf;
	snprintf(buf, sizeof(buf), "frames over 20ms: %zu (%.1f%%)   over 33ms: %zu (%.1f%%)   over 50ms: %zu (%.1f%%)\n",
		over20, n > 0 ? 100.0 * static_cast<double>(over20) / static_cast<double>(n) : 0.0,
		over33, n > 0 ? 100.0 * static_cast<double>(over33) / static_cast<double>(n) : 0.0,
		over50, n > 0 ? 100.0 * static_cast<double>(over50) / static_cast<double>(n) : 0.0);
	out += buf;
	snprintf(buf, sizeof(buf), "slow frames (>20ms): %lld   of which over half of the time was unaccounted for by Update/Draw/wait/messages (OS stall etc.): %lld\n", m_nSlowFrames, m_nSlowOutsideFrames);
	out += buf;
	if (m_nGpuCount > 0)
	{
		snprintf(buf, sizeof(buf), "GPU frame time (ms): avg %.2f  max %.2f  (samples %lld)\n", m_fGpuTotal / static_cast<double>(m_nGpuCount), m_fGpuMax, m_nGpuCount);
		out += buf;
	}

	// 項目ごとの平均(1フレームあたり)
	const double fFrames = static_cast<double>(m_nFrames);
	auto appendRows = [&](const char* pTitle, const std::vector<Row>& rows, size_t limit)
	{
		snprintf(buf, sizeof(buf), "\n---- %s (avg ms per frame / max single call ms / calls per frame) ----\n", pTitle);
		out += buf;
		size_t count = 0;
		for (const Row& r : rows)
		{
			if (count++ >= limit) break;
			snprintf(buf, sizeof(buf), "%9.3f  %9.3f  %9.1f   %s\n", r.m_fAvgMs, r.m_fMaxMs, r.m_fCallsPerFrame, r.m_sName.c_str());
			out += buf;
		}
	};
	appendRows("sections", BuildRows(m_Sections, "", false, fFrames), 40);
	appendRows("object Update by class", BuildRows(m_Updates, "", false, fFrames), 25);
	appendRows("object Draw by class", BuildRows(m_Draws, "", false, fFrames), 25);

	// 長かったフレーム
	out += "\n---- worst frames ----\n";
	for (const WorstFrame& w : m_WorstFrames)
	{
		snprintf(buf, sizeof(buf), "frame #%lld  interval %.2f ms\n", w.m_nFrameNo, w.m_fInterval);
		out += buf;
		for (const auto& item : w.m_Top)
		{
			snprintf(buf, sizeof(buf), "    %9.3f ms  %s\n", item.second, item.first.c_str());
			out += buf;
		}
	}
	return out;
}

/*****************************************//*
	@brief　	| レポートのファイル出力
	@param		| pPath：出力先のファイルパス
	@return		| true:成功 false:失敗
*//*****************************************/
bool CProfiler::WriteReport(const char* pPath) const
{
	FILE* pFile = nullptr;
	if (fopen_s(&pFile, pPath, "wb") != 0 || pFile == nullptr) return false;

	const std::string report = BuildReport();
	fwrite(report.data(), 1, report.size(), pFile);
	fclose(pFile);
	return true;
}

/*****************************************//*
	@brief　	| 自動計測の開始
	@param		| fSeconds：計測する秒数
	@param		| pReportPath：レポートの出力先
*//*****************************************/
void CProfiler::StartAutoRun(float fSeconds, const char* pReportPath)
{
	s_bEnabled = true;
	m_bAutoRun = true;
	m_fAutoStart = NowMs();
	m_fAutoSeconds = fSeconds;
	m_sReportPath = pReportPath;
}

/*****************************************//*
	@brief　	| 自動計測の確認
	@return		| true:計測時間が終わり、レポートを出力した
*//*****************************************/
bool CProfiler::UpdateAutoRun()
{
	if (!m_bAutoRun) return false;
	if (NowMs() - m_fAutoStart < static_cast<double>(m_fAutoSeconds) * 1000.0) return false;

	WriteReport(m_sReportPath.c_str());
	m_bAutoRun = false;
	return true;
}
