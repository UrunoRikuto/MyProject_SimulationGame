/**************************************************//*
	@file	| CollisionBase.h
	@brief	| 当たり判定基底クラス
*//**************************************************/
#pragma once
#include "Component.h" 

// @brief 当たり判定基底クラス
class CCollisionBase : public CComponent
{
public:
	// コンストラクタの継承
    using CComponent::CComponent;

	// @brief デストラクタ
    virtual ~CCollisionBase();

	// @brief 衝突が起きたかどうかを取得
	// @param other：衝突先
	// @return true:衝突 false:非衝突
    virtual bool IsHit(CCollisionBase* other);

protected:


};
