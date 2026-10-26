/**************************************************//*
	@file	| SeasonManager.h
	@brief	| 季節管理システムのhファイル
	@note	| ゲーム内の日数から季節を求め、地面や木などの色を季節に合わせて変えるための情報をシェーダーに渡す
			| 季節は、決まった日数ごとに 春 → 夏 → 秋 → 冬 の順で切り替わる
			| 今は見た目だけに影響する(ゲームの内容には影響しない)
*//**************************************************/
#pragma once
#include <DirectXMath.h>
#include <string>

// @brief 季節管理クラス
class CSeasonManager
{
public:
	// @brief 季節の列挙型
	enum class Season
	{
		Spring,		// 春
		Summer,		// 夏
		Autumn,		// 秋
		Winter,		// 冬
	};

	// @brief 季節による色の変化の対象
	enum class TargetKind
	{
		None,		// 変化しない
		Tree,		// 木(葉の部分だけ)
		Grass,		// 草(葉の部分だけ)
		Ground,		// 地面(全体)
	};

	// @brief シェーダー(PS_TexColor.hlsl)の定数バッファに渡す情報
	// @note HLSLのcbuffer Seasonと同じ並びにすること
	struct ShaderParam
	{
		// 変化後の色(rgb)
		DirectX::XMFLOAT4 color;
		// x：変化の強さ(0:変化なし 1:完全に変化後の色) y：変化させるオブジェクトの割合(0～1) z：0:葉の緑の部分だけ 1:全体
		DirectX::XMFLOAT4 param;
	};

	// @brief 1つの季節の日数
	static constexpr int Season_Length_Days = 10;

public:
	// @brief 季節の反映を有効にするかどうかの設定
	// @param bActive：true:有効(ゲームシーン中) false:無効(タイトルなど。色は変化しない)
	static void SetActive(bool bActive) { s_bActive = bActive; }

	// @brief 現在の季節の取得
	static Season GetSeason();

	// @brief 現在の季節の名前の取得
	// @return 春・夏・秋・冬のいずれかの文字列
	static const char* GetSeasonName();

	// @brief 現在の季節の、何日目か(0から数える)の取得
	static int GetDayInSeason();

	// @brief モデルのキーから、色の変化の対象を求める
	// @param sKey：モデルのキー
	static TargetKind GetTargetKind(const std::string& sKey);

	// @brief 色の変化の対象ごとの、現在のシェーダーに渡す情報の取得
	// @param eKind：色の変化の対象
	// @note 季節の終わりの2日間は、次の季節の色へ滑らかに移り変わる
	static ShaderParam GetShaderParam(TargetKind eKind);

private:
	// @brief ゲーム内の経過日数(小数部あり)の取得
	static float GetElapsedDays();

private:
	// @brief 季節の反映が有効かどうか
	inline static bool s_bActive = false;
};
