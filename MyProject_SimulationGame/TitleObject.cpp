/**************************************************//*
	@file	| TitleObject.cpp
	@brief	| タイトルシーン用ゲームオブジェクトのcppファイル
	@note	| タイトルシーン用のゲームオブジェクトクラス
			| CGameObjectを継承
*//**************************************************/
#include "TitleObject.h"
#include "Sprite3DRenderer.h"

/*****************************************//*
	@brief　	| コンストラクタ
*//*****************************************/
CTitleObject::CTitleObject()
	:CGameObject()
	, m_onFunc(nullptr)
{
	AddComponent<CSprite3DRenderer>()->SetDepth(false);

	m_tParam.m_eCulling = D3D11_CULL_NONE;
}

/*****************************************//*
	@brief　	| デストラクタ
*//*****************************************/
CTitleObject::~CTitleObject()
{
	m_onFunc = nullptr;
}

/*****************************************//*
	@brief　	| 実行
*//*****************************************/
void CTitleObject::Execute()
{
	// 登録されている関数がなければ何もしない
	if (m_onFunc == nullptr)return;

	// 登録されている関数を実行
	m_onFunc();

}