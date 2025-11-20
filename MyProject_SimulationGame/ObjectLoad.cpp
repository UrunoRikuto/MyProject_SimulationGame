/**************************************************//*
	@file	| ObjectLoad.cpp
	@brief	| オブジェクトロードクラスのcppファイル
	@note	| オブジェクトのロード、アンロードを行う
*//**************************************************/
#include "ObjectLoad.h"
#include "RendererComponent.h"
#include "Defines.h"

/*****************************************//*
	@brief　	| 全てのオブジェクトをロード
*//****************************************/
void CObjectLoad::LoadAll()
{
	// フェード用テクスチャのロード
    CRendererComponent::Load(RendererKind::Texture, TEXTURE_PATH("Fade.png"), "Fade");

	// テスト用モデルのロード
	CRendererComponent::Load(RendererKind::Model, MODEL_PATH("TestModel.obj"), "TestModel", 1.0f, Model::Flip::None);

	// Humanモデルのロード
	CRendererComponent::Load(RendererKind::Model, MODEL_PATH("Human.obj"), "Human", 1.0f, Model::Flip::None);

	// Woodモデルのロード
	CRendererComponent::Load(RendererKind::Model, MODEL_PATH("Wood.obj"), "Wood", 1.0f, Model::Flip::None);
}


/*****************************************//*
	@brief　	| 全てのオブジェクトをアンロード
*//****************************************/
void CObjectLoad::UnLoadAll()
{
    CRendererComponent::UnLoad();
}
