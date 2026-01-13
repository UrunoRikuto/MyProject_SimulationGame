/**************************************************//*
	@file	| SpriteObject.cpp
	@brief	| スプライトオブジェクトクラスのcppファイル
	@note	| スプライトオブジェクトクラス
			| CGameObjectを継承
*//**************************************************/
#include "SpriteObject.h"
#include "Sprite3DRenderer.h"

/*****************************************//*
	@brief　	| コンストラクタ
*//*****************************************/
CSpriteObject::CSpriteObject()
	:CGameObject()
{
}

/*****************************************//*
	@brief　	| デストラクタ
*//*****************************************/
CSpriteObject::~CSpriteObject()
{
}

/*****************************************//*
	@brief　	| 初期化処理
*//*****************************************/
void CSpriteObject::Init()
{
	CGameObject::Init();

	AddComponent<CSprite3DRenderer>();
}
