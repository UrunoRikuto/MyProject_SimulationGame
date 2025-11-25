#include "BuildObject.h"
#include "ModelRenderer.h"
#include "ImguiSystem.h"

/****************************************//*
	@brief　	| コンストラクタ
*//****************************************/
CBuildObject::CBuildObject()
	:CGameObject()
	, m_nBuildLevel(1)
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
