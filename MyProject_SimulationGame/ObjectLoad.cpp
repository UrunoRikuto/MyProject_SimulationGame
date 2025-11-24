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

	// スカイボックスのモデルのロード
	CRendererComponent::Load(RendererKind::Model, MODEL_PATH("Skybox.fbx"), "SkyBox", 1.0f, Model::Flip::None);

	// 地面のモデルのロード
	CRendererComponent::Load(RendererKind::Model, MODEL_PATH("FieldGround.obj"), "FieldGround", 1.0f, Model::Flip::None);

	// 人間のモデルのロード
	CRendererComponent::Load(RendererKind::Model, MODEL_PATH("Human.obj"), "Human", 1.0f, Model::Flip::None);

	// 木のモデルのロード
	CRendererComponent::Load(RendererKind::Model, MODEL_PATH("Wood.obj"), "Wood", 1.0f, Model::Flip::None);

	// 石のモデルのロード
	CRendererComponent::Load(RendererKind::Model, MODEL_PATH("Stone.obj"), "Stone", 1.0f, Model::Flip::None);

	// 貯蔵庫のモデルのロード
	CRendererComponent::Load(RendererKind::Model, MODEL_PATH("StorageHouse.obj"), "StorageHouse", 1.0f, Model::Flip::None);

	// リフレッシュ施設のモデルのロード
	CRendererComponent::Load(RendererKind::Model, MODEL_PATH("RefreshFacility.obj"), "RefreshFacility", 1.0f, Model::Flip::None);

	// HPバーのゲージテクスチャのロード
	CRendererComponent::Load(RendererKind::Texture, TEXTURE_PATH("Bar_Gauge.png"), "Bar_Gauge");
}


/*****************************************//*
	@brief　	| 全てのオブジェクトをアンロード
*//****************************************/
void CObjectLoad::UnLoadAll()
{
    CRendererComponent::UnLoad();
}
