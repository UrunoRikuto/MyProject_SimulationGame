/**************************************************//*
	@file	| Human.cpp
	@brief	| 人間オブジェクトクラスのcppファイル
	@note	| 人間オブジェクトに関する処理の実装
			| CGameObjectを継承
*//**************************************************/
#include "Human.h"
#include "ModelRenderer.h"
#include "CollisionObb.h"
#include <imgui.h>

/****************************************//*
	@brief　	| コンストラクタ
*//****************************************/
CHuman::CHuman()
	: CGameObject()
	, m_pJob(std::make_unique<CNeet_Job>())
{
	// モデルレンダラーコンポーネントの追加
	AddComponent<CModelRenderer>();

	// OBBコリジョンコンポーネントの追加
	AddComponent<CCollisionObb>();
}

/****************************************//*
	@brief　	| デストラクタ
*//****************************************/
CHuman::~CHuman()
{
}

/****************************************//*
	@brief　	| 初期化処理
*//****************************************/
void CHuman::Init()
{
	// 基底クラスの初期化処理
	CGameObject::Init();

	// モデルレンダラーコンポーネントの設定
	CModelRenderer* pModelRenderer = GetComponent<CModelRenderer>();
	pModelRenderer->SetKey("Human");
	pModelRenderer->SetRendererParam(m_tParam);

	// 頂点シェーダーの設定
	VertexShader* pVS = new VertexShader();
	pVS->Load(SHADER_PATH("VS_Object.cso"));
	pModelRenderer->SetVertexShader(pVS);
	// ピクセルシェーダーの設定
	PixelShader* pPS = new PixelShader();
	pPS->Load(SHADER_PATH("PS_TexColor.cso"));
	pModelRenderer->SetPixelShader(pPS);


	// OBBコリジョンコンポーネントの設定
	CCollisionObb* pCollisionObb = GetComponent<CCollisionObb>();
	pCollisionObb->SetTag("Human");
	pCollisionObb->SetCenter(m_tParam.m_f3Pos);
	pCollisionObb->SetSize(m_tParam.m_f3Size);
	pCollisionObb->SetSizeScale({ 1.0f, 2.0f, 1.0f });
}

/****************************************//*
	@brief　	| 終了処理
*//****************************************/
void CHuman::Uninit()
{
	// 基底クラスの終了処理
	CGameObject::Uninit();
}

/****************************************//*
	@brief　	| 更新処理
*//****************************************/
void CHuman::Update()
{
	// 基底クラスの更新処理
	CGameObject::Update();

	// 仕事処理
	if(m_pJob)m_pJob->DoWork();

}

/****************************************//*
	@brief　	| 描画処理
*//****************************************/
void CHuman::Draw()
{
	// 基底クラスの描画処理
	CGameObject::Draw();
}

/****************************************//*
	@brief　	| インスペクター表示処理
	@param		| isEnd：true:ImGuiのEnd()を呼ぶ false:呼ばない
	@return		| 表示した項目数
	@note		| ImGuiを使用してオブジェクトのパラメータを表示、編集する
*//****************************************/
int CHuman::Inspecter(bool isEnd)
{
	// 基底クラスのインスペクター表示処理
	// (位置情報、サイズ情報、回転情報)
	int nItemCount = CGameObject::Inspecter(false);

	/**** 職業表示 ****/

	// 折りたたみヘッダーの表示
	if (ImGui::CollapsingHeader("Job"))
	{
		// 現在の職業を文字列で取得
		std::string currentJob = m_pJob->GetJobName();
		// 表示
		ImGui::Text(("JobName: " + currentJob).c_str());

		// Combo 用（string → const char* の配列に変換）
		std::vector<const char*> items;
		for (auto& s : JobNames) items.push_back(s.c_str());

		// 現在のインデックス
		int currentIndex = std::distance(
			JobNames.begin(),
			std::find(JobNames.begin(), JobNames.end(), currentJob)
		);

		if (ImGui::Combo("JobSelect", &currentIndex, items.data(), items.size()))
		{
			// 選択された string の職業名
			currentJob = JobNames[currentIndex];

			// Strategy 差し替え
			SetHumanJob(CreateJobByName(currentJob));
		}
	}

	/***** 職業表示 *****/

	// IMGUIウィンドウの終了
	if (isEnd)
	{
		// 子要素の終了
		ImGui::EndChild();
		// 表示項目のカウントを増やす
		nItemCount++;
		ImGui::End();
	}



	// 表示した項目数を返す
	return nItemCount;
}
