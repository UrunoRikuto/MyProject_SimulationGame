/**************************************************//*
	@file	| PerlinNoise.h
	@brief	| パーリンノイズ生成クラスのhファイル
	@note	| 2Dパーリンノイズを生成するクラス
*//**************************************************/
#pragma once
#include <vector>

// @brief パーリンノイズ生成クラス
class PerlinNoise {
public:
	// @brief コンストラクタ
	// @param seed：シード値
    PerlinNoise(unsigned int seed = 2025);

	// @brief ノイズ値の取得
	// @param x：X座標
	// @param y：Y座標
	// @return ノイズ値（-1.0～1.0）
    float noise(float x, float y) const;

private:
	// @brief パーミュテーションテーブル
    std::vector<int> p;

	// @brief フェード関数
	// @param t：入力値
    static float fade(float t);

	// @brief 線形補間関数
	// @param t：補間係数
	// @param a：値A
	// @param b：値B
	// @return 補間後の値
    static float lerp(float t, float a, float b);

	// @brief 勾配関数
	// @param hash：ハッシュ値
	// @param x：X座標
	// @param y：Y座標
	// @return 勾配値
	static float grad(int hash, float x, float y);
};
