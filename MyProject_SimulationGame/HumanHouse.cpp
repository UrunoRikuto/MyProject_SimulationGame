/**************************************************//*
	@file	| HumanHouse.cpp
	@brief	| 人間の家クラスのcppファイル
	@note	| 人間が居住する建築物
			| CBuildObjectを継承
*//**************************************************/
#include "HumanHouse.h"
#include "ImguiSystem.h"
#include "ShaderManager.h"
#include "GeneratorManager.h"

/*****************************************//*
	@brief　	| コンストラクタ
*//****************************************/
CHumanHouse::CHumanHouse()
	:CBuildObject()
{
}

/*****************************************//*
	@brief　	| デストラクタ
*//****************************************/
CHumanHouse::~CHumanHouse()
{
}

/*****************************************//*
	@brief　	| 初期化処理
*//****************************************/
void CHumanHouse::Init()
{
	// 親クラスの初期化処理を呼び出す
	CBuildObject::Init();

	// モデルレンダラーコンポーネントの設定
	CModelRenderer* pModelRenderer = GetComponent<CModelRenderer>();
	pModelRenderer->SetKey("HumanHouse");

	// シェーダーマネージャーの取得
	CShaderManager* pShaderManager = CShaderManager::GetInstance();

	// 頂点シェーダーの設定
	pModelRenderer->SetVertexShader(pShaderManager->GetVertexShader(VSType::Object));

	// ピクセルシェーダーの設定
	pModelRenderer->SetPixelShader(pShaderManager->GetPixelShader(PSType::TexColor));

}

/*****************************************//*
	@brief　	| 更新処理
*//****************************************/
void CHumanHouse::Update()
{
	// 親クラスの更新処理を呼び出す
	CBuildObject::Update();

	// 居住者の空き枠に対する生成リクエストは、CHumanGeneratorが全ての家の空き枠をまとめて確認して追加する
}

/*****************************************//*
	@brief　	| インスペクター表示処理
	@return		| 表示した項目数
	@note　　　	| ImGuiを使用してオブジェクトのパラメータを表示、編集する
*//*****************************************/
int CHumanHouse::Inspecter()
{
	int itemCount = CBuildObject::Inspecter();

	// 居住者数の表示
	ImGui::Text(u8"居住者数: %d / %d", static_cast<int>(m_Residents.size()), GetMaxResidents());

	return itemCount;
}

/*****************************************//*
	@brief　	| 最大居住人数の取得
	@return		| 最大居住人数
*//****************************************/
int CHumanHouse::GetMaxResidents() const
{
	return MAX_RESIDENTS[m_nBuildLevel - 1];
}

/*****************************************//*
	@brief　	| 居住者の追加
	@param		| pHuman：追加する人間ポインタ
*//****************************************/
void CHumanHouse::AddResident(CHuman* pHuman)
{
	m_Residents.push_back(pHuman);
}

/*****************************************//*
	@brief　	| 居住者の削除
	@param		| pHuman：削除する人間ポインタ
*//****************************************/
void CHumanHouse::RemoveResident(CHuman* pHuman)
{
	m_Residents.remove(pHuman);
}

/*****************************************//*
	@brief　	| 施設の説明文を取得
	@return		| 人間の家の役割と、レベルを上げたときの効果の説明
*//*****************************************/
std::string CHumanHouse::GetDescription() const
{
	return std::string(u8"人間が住む家です。\n")
		+ u8"レベルを上げると住める人数が増え、空きができると新しい人間が増えます。\n"
		+ u8"・住める人数: " + FormatLevelValues(MAX_RESIDENTS, 1.0f, u8"人");
}
