/**************************************************//*
	@file	| CollisionBase.cpp
	@brief	| 当たり判定基底クラス
*//**************************************************/
#include "CollisionBase.h"

/*****************************************//*
	@brief　	| デストラクタ
*//*****************************************/
CCollisionBase::~CCollisionBase()
{

}

/*****************************************//*
	@brief　	| 衝突が起きたかどうかを取得
	@param　	| other：衝突先
	@return		| true:衝突 false:非衝突
*//*****************************************/
bool CCollisionBase::IsHit(CCollisionBase* other)
{
    return false;
}
