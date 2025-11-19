/**********************************************************************************//*
	@file		| Easing.inl
	@brief		| イージング関数
*//***********************************************************************************/
#include "Easing.h"
#include <cmath>

/*************************//*
@brief      | EaseOutQuartのイージング関数
@param t    | 経過時間
@return     | 計算結果
*//*************************/
inline  float easeOutQuart(float t)
{
    return 1 - pow(1 - t, 4);
}

/*************************//*
@brief      | EaseOutQuartの移動関数
@param[in]  | t：経過時間
@param[in]  | start：開始位置
@param[in]  | end：最終位置
@return     | 計算結果
*//*************************/
inline XMFLOAT2 easeMoveOutQuart(float t, XMFLOAT2 start, XMFLOAT2 end)
{
    float easedT = easeOutQuart(t);
    return
    {
        start.x + (end.x - start.x) * easedT,
        start.y + (end.y - start.y) * easedT
    };
}

/*************************//*
@brief      | EaseOutBackのイージング関数
@param[in]  | t：経過時間
@param[in]  | overshoot：過剰量
@return     | 計算結果
*//*************************/
inline float easeOutBack(float t, float overshoot = 1.70158f)
{
    return 1 + (--t) * t * ((overshoot + 1) * t + overshoot);
}

/*************************//*
@brief      | EaseOutBackの移動関数
@param[in]  | t：経過時間
@param[in]  | start：開始位置
@param[in]  | end：最終位置
@param[in]  | overshoot：過剰量
@return     | 計算結果
*//*************************/
inline XMFLOAT2 easeMoveOutBack(float t, XMFLOAT2 start, XMFLOAT2 end, float overshoot = 1.70158f)
{
    float easedT = easeOutBack(t, overshoot);
    return
    {
        start.x + (end.x - start.x) * easedT,
        start.y + (end.y - start.y) * easedT
    };
}

/*************************//*
@brief      | EaseOutQuintのイージング関数
@param t    | 経過時間
@return     | 計算結果
*//*************************/
inline float easeOutQuint(float t)
{
    return 1 - pow(1 - t, 5);
}

/*************************//*
@brief      | EaseOutQuintの移動関数
@param[in]  | t：経過時間
@param[in]  | start：開始位置
@param[in]  | end：最終位置
@return     | 計算結果
*//*************************/
inline XMFLOAT2 easeMoveOutQuint(float t, XMFLOAT2 start, XMFLOAT2 end)
{
    float easedT = easeOutQuint(t);
    return
    {
        start.x + (end.x - start.x) * easedT,
        start.y + (end.y - start.y) * easedT
    };
}
