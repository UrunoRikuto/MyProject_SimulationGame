/**************************************************//*
	@file	| SeasonManager.cpp
	@brief	| 季節管理システムのcppファイル
	@note	| ゲーム内の日数から季節を求め、地面や木などの色を季節に合わせて変えるための情報をシェーダーに渡す
*//**************************************************/
#include "SeasonManager.h"
#include "GameTimeManager.h"
#include <algorithm>
#include <cmath>

namespace
{
	// @brief 季節の数
	constexpr int Season_Count = 4;

	// @brief 季節の終わりのこの割合から、次の季節の色へ移り変わり始める
	constexpr float Season_Transition_Start = 0.8f;

	// @brief 1つの季節での、色の変化の指定
	struct SeasonStyle
	{
		// 変化後の色
		DirectX::XMFLOAT3 color;
		// 変化の強さ(0:変化なし 1:完全に変化後の色)
		float fStrength;
		// 変化させるオブジェクトの割合(0～1)
		float fRatio;
	};

	// @brief 木の葉の色(春 夏 秋 冬の順)
	const SeasonStyle Tree_Styles[Season_Count] =
	{
		{ { 1.00f, 0.62f, 0.80f }, 1.0f, 1.0f },	// 春：ピンク
		{ { 0.00f, 0.00f, 0.00f }, 0.0f, 1.0f },	// 夏：緑(そのまま)
		{ { 1.00f, 0.45f, 0.08f }, 1.0f, 1.0f },	// 秋：オレンジ(全ての木)
		{ { 1.05f, 1.05f, 1.10f }, 1.0f, 1.0f },	// 冬：白
	};

	// @brief 草の色(春 夏 秋 冬の順)
	const SeasonStyle Grass_Styles[Season_Count] =
	{
		{ { 0.00f, 0.00f, 0.00f }, 0.0f, 1.0f },	// 春：緑(そのまま)
		{ { 0.00f, 0.00f, 0.00f }, 0.0f, 1.0f },	// 夏：緑(そのまま)
		{ { 0.90f, 0.80f, 0.30f }, 0.25f, 1.0f },	// 秋：ほんの少し黄色
		{ { 1.05f, 1.05f, 1.10f }, 0.95f, 1.0f },	// 冬：白
	};

	// @brief 地面の色(春 夏 秋 冬の順)
	const SeasonStyle Ground_Styles[Season_Count] =
	{
		{ { 0.00f, 0.00f, 0.00f }, 0.0f, 1.0f },	// 春：緑(そのまま)
		{ { 0.00f, 0.00f, 0.00f }, 0.0f, 1.0f },	// 夏：緑(そのまま)
		{ { 0.00f, 0.00f, 0.00f }, 0.0f, 1.0f },	// 秋：緑(そのまま)
		{ { 1.05f, 1.05f, 1.10f }, 1.0f, 1.0f },	// 冬：白
	};

	// @brief 滑らかな補間(0～1)
	float SmoothStep(float fEdge0, float fEdge1, float fX)
	{
		const float t = std::clamp((fX - fEdge0) / (fEdge1 - fEdge0), 0.0f, 1.0f);
		return t * t * (3.0f - 2.0f * t);
	}
}

/*****************************************//*
	@brief　	| ゲーム内の経過日数(小数部あり)の取得
*//*****************************************/
float CSeasonManager::GetElapsedDays()
{
	if (!s_bActive) return 0.0f;

	CGameTimeManager* pTime = CGameTimeManager::GetInstance();
	return static_cast<float>(pTime->GetGameDays()) + pTime->GetDayProgress();
}

/*****************************************//*
	@brief　	| 現在の季節の取得
*//*****************************************/
CSeasonManager::Season CSeasonManager::GetSeason()
{
	const int nSeasonIndex = static_cast<int>(GetElapsedDays() / Season_Length_Days) % Season_Count;
	return static_cast<Season>(nSeasonIndex);
}

/*****************************************//*
	@brief　	| 現在の季節の名前の取得
*//*****************************************/
const char* CSeasonManager::GetSeasonName()
{
	switch (GetSeason())
	{
	case Season::Spring: return u8"春";
	case Season::Summer: return u8"夏";
	case Season::Autumn: return u8"秋";
	case Season::Winter: return u8"冬";
	}
	return "";
}

/*****************************************//*
	@brief　	| 現在の季節の、何日目か(0から数える)の取得
*//*****************************************/
int CSeasonManager::GetDayInSeason()
{
	return static_cast<int>(GetElapsedDays()) % Season_Length_Days;
}

/*****************************************//*
	@brief　	| モデルのキーから、色の変化の対象を求める
	@param　	| sKey：モデルのキー
*//*****************************************/
CSeasonManager::TargetKind CSeasonManager::GetTargetKind(const std::string& sKey)
{
	if (sKey == "Wood") return TargetKind::Tree;
	if (sKey == "Grass") return TargetKind::Grass;
	if (sKey == "FieldGround") return TargetKind::Ground;
	return TargetKind::None;
}

/*****************************************//*
	@brief　	| 色の変化の対象ごとの、現在のシェーダーに渡す情報の取得
	@param　	| eKind：色の変化の対象
*//*****************************************/
CSeasonManager::ShaderParam CSeasonManager::GetShaderParam(TargetKind eKind)
{
	ShaderParam result = {};

	// 季節の反映が無効な場合や、変化しない対象は、何も変化させない
	if (!s_bActive || eKind == TargetKind::None) return result;

	const SeasonStyle* pStyles = nullptr;
	float fMaskMode = 0.0f;
	switch (eKind)
	{
	case TargetKind::Tree:
		pStyles = Tree_Styles;
		break;
	case TargetKind::Grass:
		pStyles = Grass_Styles;
		break;
	case TargetKind::Ground:
		pStyles = Ground_Styles;
		fMaskMode = 1.0f;
		break;
	default:
		return result;
	}

	// 現在の季節と、その中での進み具合
	const float fDays = GetElapsedDays();
	const int nSeason = static_cast<int>(fDays / Season_Length_Days) % Season_Count;
	const int nNextSeason = (nSeason + 1) % Season_Count;
	const float fProgress = std::fmod(fDays, static_cast<float>(Season_Length_Days)) / static_cast<float>(Season_Length_Days);

	// 季節の終わりに近づくほど、次の季節の色に近づける
	const float w = SmoothStep(Season_Transition_Start, 1.0f, fProgress);
	const SeasonStyle& now = pStyles[nSeason];
	const SeasonStyle& next = pStyles[nNextSeason];

	// 変化の強さと割合は、そのまま補間する
	const float fStrength = now.fStrength + (next.fStrength - now.fStrength) * w;
	const float fRatio = now.fRatio + (next.fRatio - now.fRatio) * w;

	// 色は、変化の強さで重みを付けて混ぜる(変化なしの季節の色に引きずられないようにするため)
	const float wNow = now.fStrength * (1.0f - w);
	const float wNext = next.fStrength * w;
	const float fSum = wNow + wNext;
	DirectX::XMFLOAT3 color = { 0.0f, 0.0f, 0.0f };
	if (fSum > 0.0001f)
	{
		color.x = (now.color.x * wNow + next.color.x * wNext) / fSum;
		color.y = (now.color.y * wNow + next.color.y * wNext) / fSum;
		color.z = (now.color.z * wNow + next.color.z * wNext) / fSum;
	}

	result.color = DirectX::XMFLOAT4(color.x, color.y, color.z, 0.0f);
	result.param = DirectX::XMFLOAT4(fStrength, fRatio, fMaskMode, 0.0f);
	return result;
}
