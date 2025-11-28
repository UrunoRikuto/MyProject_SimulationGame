/**************************************************//*
	@file	| PerlinNoice.cpp
	@brief	| パーリンノイズ生成クラスのcppファイル
	@note	| 2Dパーリンノイズを生成するクラス
			| 参考サイト
			|┗ https://postd.cc/understanding-perlin-noise/ (パーリンノイズの理解)
*//**************************************************/
#include "PerlinNoice.h"
#include <numeric>
#include <random>
#include <algorithm>
#include <cmath>
#include <math.h>


/****************************************//*
	@brief　	| コンストラクタ
	@param		| seed：シード値
*//****************************************/
PerlinNoise::PerlinNoise(unsigned int seed)
{
	// パーミュテーションテーブルの初期化
    p.resize(256);
	// 0～255の値で初期化
    std::iota(p.begin(), p.end(), 0);

	// シャッフル
    std::default_random_engine engine(seed);

	// シャッフル実行
    std::shuffle(p.begin(), p.end(), engine);

    // 256 -> 512 に拡張
    p.insert(p.end(), p.begin(), p.end());
}

/****************************************//*
    @brief　	| ノイズ値の取得
    @param		| x：X座標
    @param		| y：Y座標
    @return		| ノイズ値（-1.0～1.0）
*//****************************************/
float PerlinNoise::noise(float x, float y) const
{
    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;

    x -= floor(x);
    y -= floor(y);

    float u = fade(x);
    float v = fade(y);

    int A = p[X] + Y;
    int AA = p[A];
    int AB = p[A + 1];
    int B = p[X + 1] + Y;
    int BA = p[B];
    int BB = p[B + 1];

    return lerp(v,
        lerp(u, grad(p[AA], x, y),
            grad(p[BA], x - 1, y)),
        lerp(u, grad(p[AB], x, y - 1),
            grad(p[BB], x - 1, y - 1))
    );
}

/****************************************//*
	@brief　	| フェード関数
    @param		| t：入力値
    @return		| フェード後の値
*//****************************************/
float PerlinNoise::fade(float t)
{
    return t * t * t * (t * (t * 6 - 15) + 10);
}

/****************************************//*
    @brief　	| 線形補間関数
    @param		| t：補間係数
    @param		| a：開始値
    @param		| b：終了値
    @return		| 補間後の値
*//****************************************/
float PerlinNoise::lerp(float t, float a, float b)
{
    return a + t * (b - a);
}

/****************************************//*
    @brief　	| 勾配関数
    @param		| hash：ハッシュ値
    @param		| x：X座標
    @param		| y：Y座標
    @return		| 勾配値
*//****************************************/
float PerlinNoise::grad(int hash, float x, float y)
{
    int h = hash & 3;
    float u = h < 2 ? x : y;
    float v = h < 2 ? y : x;
    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}