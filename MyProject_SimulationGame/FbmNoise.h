/**************************************************//*
	@file	| FbmNoise.h
	@brief	| FBM (フラクタルブラウン運動) ノイズ生成クラス
	@note	| 2D FBMノイズを生成
*//**************************************************/
#pragma once

#include <cstdint>

class PerlinNoise;

// @brief FBMノイズ生成クラス
class FbmNoise
{
public:
	struct Params
	{
		int octaves = 5; // オクターブ数
		float lacunarity = 2.0f;// 周波数の倍率
		float gain = 0.5f; // 振幅の倍率
		float amplitude = 1.0f;// 初期振幅
		float frequency = 1.0f;// 初期周波数
		bool normalize = true;// 正規化フラグ
	};

	// @brief コンストラクタ
	explicit FbmNoise(std::uint32_t seed = 0);

	// @brief デストラクタ
	~FbmNoise();

	// コピーコンストラクタ・代入演算子の禁止
	FbmNoise(const FbmNoise&) = delete;
	FbmNoise& operator=(const FbmNoise&) = delete;

	// @brief ノイズ値の取得
	// @param x：X座標
	// @param y：Y座標
	// @param params：FBMノイズのパラメータ
	// @return ノイズ値（-1.0～1.0）
	float noise(float x, float y, const Params& params = Params{}) const;

private:
	// @brief パーリンノイズ生成クラスのインスタンス
	PerlinNoise* m_perlin;
};
