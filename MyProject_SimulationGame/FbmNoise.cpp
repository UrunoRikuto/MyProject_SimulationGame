/**************************************************//*
	@file	| FbmNoise.cpp
	@brief	| FBMノイズ生成クラスのcppファイル
	@note	| 2D FBMノイズを生成
*//**************************************************/
#include "FbmNoise.h"
#include "PerlinNoice.h"
#include "Defines.h"

#include <algorithm>

/****************************************//*
	@brief　	| コンストラクタ
	@param		| seed：シード値
*//****************************************/
FbmNoise::FbmNoise(std::uint32_t seed)
 : m_perlin(new PerlinNoise(seed))
{
}

/****************************************//*
	@brief　	| デストラクタ
*//****************************************/
FbmNoise::~FbmNoise()
{
	// パーリンノイズ生成クラスの解放
	SAFE_DELETE(m_perlin);
}

/****************************************//*
	@brief　	| ノイズ値の取得
	@param		| x：X座標
	@param		| y：Y座標
	@param		| params：FBMノイズのパラメータ
	@return		| ノイズ値（-1.0～1.0）
*//****************************************/
float FbmNoise::noise(float x, float y, const Params& params) const
{
	// パーリンノイズ生成クラスが存在しない場合やオクターブ数が0以下の場合は0.0fを返す
	if (!m_perlin || params.octaves <= 0)
		return 0.0f;

	// 初期振幅と初期周波数を設定
	float amp = params.amplitude;
	float freq = params.frequency;
	// ノイズ値の合計と振幅の合計を初期化
	float sum = 0.0f;
	float ampSum = 0.0f;

	// 各オクターブに対してノイズ値を計算
	for (int i = 0; i < params.octaves; ++i)
	{
		// 各オクターブのノイズ値を取得し、合計に加算
		sum += amp * m_perlin->noise(x * freq, y * freq);
		// 振幅の合計を加算
		ampSum += amp;

		// 次のオクターブの周波数と振幅を更新
		freq *= params.lacunarity;
		amp *= params.gain;
	}

	// 正規化が有効な場合、振幅の合計でノイズ値を正規化
	if (params.normalize && ampSum > 0.0f)sum /= ampSum;

	// ノイズ値を-1.0fから1.0fの範囲にクランプして返す
	return std::clamp(sum, -1.0f, 1.0f);
}
