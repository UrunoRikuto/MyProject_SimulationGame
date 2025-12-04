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
	CRendererComponent::Load(RendererKind::Model, MODEL_PATH("TestModel.obj"), "TestModel");

	// スカイボックスのモデルのロード
	CRendererComponent::Load(RendererKind::Model, MODEL_PATH("Skybox.fbx"), "SkyBox");

	// 地面のモデルのロード
	CRendererComponent::Load(RendererKind::Model, MODEL_PATH("FieldGround.obj"), "FieldGround");

	// 人間のモデルのロード
	CRendererComponent::Load(RendererKind::Model, MODEL_PATH("Human.obj"), "Human");

	// 木のモデルのロード
	CRendererComponent::Load(RendererKind::Model, MODEL_PATH("Wood.obj"), "Wood");

	// 石のモデルのロード
	CRendererComponent::Load(RendererKind::Model, MODEL_PATH("Stone.obj"), "Stone");

	// 人間の家のモデルのロード
	CRendererComponent::Load(RendererKind::Model, MODEL_PATH("HumanHouse.obj"), "HumanHouse");

	// 貯蔵庫のモデルのロード
	CRendererComponent::Load(RendererKind::Model, MODEL_PATH("StorageHouse.obj"), "StorageHouse");

	// リフレッシュ施設のモデルのロード
	CRendererComponent::Load(RendererKind::Model, MODEL_PATH("RefreshFacility.obj"), "RefreshFacility");

	// 鍛冶屋のモデルのロード
	CRendererComponent::Load(RendererKind::Model, MODEL_PATH("BlackSmith.obj"), "BlackSmith");

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
