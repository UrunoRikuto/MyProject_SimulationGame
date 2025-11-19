/**********************************************************************************//*
	@file		| Easing.h
	@brief		| イージング関数
*//***********************************************************************************/
#pragma once
#include "StructMath.h"
#include <cmath>

/*＝＝＝＝＝＝＝＝＝＝＝＝＝＝*/
/*＝　　　EaseOutQuart　　　＝*/
/*＝＝＝＝＝＝＝＝＝＝＝＝＝＝*/

// @brief EaseOutQuartのイージング関数
// @param[In] t 経過時間
// @return 計算結果
float easeOutQuart(float t);

// @brief EaseOutQuartの移動関数
// @param[In] t 経過時間
// @param[In] start 開始位置
// @param[In] end 最終位置
// @return 計算結果
XMFLOAT2 easeMoveOutQuart(float t, XMFLOAT2 start, XMFLOAT2 end);


/*＝＝＝＝＝＝＝＝＝＝＝＝＝＝*/
/*＝　　　EaseOutBack　　　 ＝*/
/*＝＝＝＝＝＝＝＝＝＝＝＝＝＝*/

// @brief EaseOutBackのイージング関数
// @param[In] t 経過時間
// @param[In] overshoot 過剰量
// @return 計算結果
float easeOutBack(float t, float overshoot = 1.70158f);

// @brief EaseOutBackの移動関数
// @param[In] t 経過時間
// @param[In] start 開始位置
// @param[In] end 最終位置
// @param[In] overshoot 過剰量
// @return 計算結果
XMFLOAT2 easeMoveOutBack(float t, XMFLOAT2 start, XMFLOAT2 end, float overshoot = 1.70158f);


/*＝＝＝＝＝＝＝＝＝＝＝＝＝＝*/
/*＝　　　EaseOutQuint　　　＝*/
/*＝＝＝＝＝＝＝＝＝＝＝＝＝＝*/

// @brief EaseOutQuintのイージング関数
// @param[In] t 経過時間
// @return 計算結果
float easeOutQuint(float t);

// @brief EaseOutQuintの移動関数
// @param[In] t 経過時間
// @param[In] start 開始位置
// @param[In] end 最終位置
// @return 計算結果
XMFLOAT2 easeMoveOutQuint(float t, XMFLOAT2 start, XMFLOAT2 end);

// インラインファイルのインクルード
#include "Easing.inl"