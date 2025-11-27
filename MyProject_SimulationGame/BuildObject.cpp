/**************************************************//*
	@file	| BuildObject.cpp
	@brief	| 建築オブジェクトクラスのcppファイル
	@note	| 建築オブジェクトの処理を定義
			| CGameObjectを継承
*//**************************************************/
#include "BuildObject.h"
#include "ModelRenderer.h"
#include "ImguiSystem.h"

/****************************************//*
	@brief　	| コンストラクタ
*//****************************************/
CBuildObject::CBuildObject()
	:CGameObject()
	, m_nBuildLevel(1)
	, m_fBuildProgress(0.0f)
	, m_n2FieldCellIndex({ -1, -1 })
{
}

/****************************************//* 
	@brief　	| デストラクタ
*//****************************************/
CBuildObject::~CBuildObject()
{
}

/****************************************//* 
	@brief　	| 初期化処理
*//****************************************/
void CBuildObject::Init()
{
	// 親クラスの初期化処理を呼び出す
	CGameObject::Init();
}

/*****************************************//*
	@brief　	| インスペクター表示処理
	@param		| isEnd：true:ImGuiのEnd()を呼ぶ false:呼ばない
	@return		| 表示した項目数
	@note　　　	| ImGuiを使用してオブジェクトのパラメータを表示、編集する
*//*****************************************/
int CBuildObject::Inspecter(bool isEnd)
{
	int itemCount = CGameObject::Inspecter(false);

	// 建築物レベルの表示と編集
	ImGui::Text(std::string("Build Level:"+ std::to_string(m_nBuildLevel)).c_str());

	// IMGUIウィンドウの終了
	if (isEnd)
	{
		// 子要素の終了
		ImGui::EndChild();
		ImGui::End();
		itemCount++;
	}

	return itemCount;
}

/*****************************************//*
	@brief　	| 建築物レベルを進める
*//*****************************************/
void CBuildObject::UpgradeBuildLevel()
{
	if (!IsMaxBuildLevel())
	{
		m_nBuildLevel++;
	}
}
/*****************************************//*
	@brief　	| 建築完成度を進める
	@param		| fAmount：進める量
*//*****************************************/
void CBuildObject::ProgressBuild(float fAmount)
{
	m_fBuildProgress += fAmount;
	if (m_fBuildProgress > 100.0f)
	{
		m_fBuildProgress = 100.0f;
	}
}
