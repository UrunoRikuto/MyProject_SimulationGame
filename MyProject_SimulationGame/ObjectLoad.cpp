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

	// 人間のモデルのロード
	CRendererComponent::Load(RendererKind::Model, MODEL_PATH("Human.obj"), "Human", 1.0f, Model::Flip::None);

	// 木のモデルのロード
	CRendererComponent::Load(RendererKind::Model, MODEL_PATH("Wood.obj"), "Wood", 1.0f, Model::Flip::None);

	// 石のモデルのロード
	CRendererComponent::Load(RendererKind::Model, MODEL_PATH("Stone.obj"), "Stone", 1.0f, Model::Flip::None);


	// HPバーのフレームテクスチャのロード
	CRendererComponent::Load(RendererKind::Texture, TEXTURE_PATH("HpBar_Frame.png"), "HpBar_Frame");

	// HPバーのゲージテクスチャのロード
	CRendererComponent::Load(RendererKind::Texture, TEXTURE_PATH("HpBar_Gauge.png"), "HpBar_Gauge");
}


/*****************************************//*
	@brief　	| 全てのオブジェクトをアンロード
*//****************************************/
void CObjectLoad::UnLoadAll()
{
    CRendererComponent::UnLoad();
}
