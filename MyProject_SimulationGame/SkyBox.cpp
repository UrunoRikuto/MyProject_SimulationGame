/**************************************************//*
	@file	| SkyBox.cpp
	@brief	| スカイボックスクラスのcppファイル
	@note	| CGameObjectを継承
*//**************************************************/
#include "SkyBox.h"
#include "ModelRenderer.h"
#include "Camera.h"
#include "GameTimeManager.h"
#include "ShaderManager.h"

/*****************************************//*
	@brief　	| コンストラクタ
*//*****************************************/
CSkyBox::CSkyBox()
	: CGameObject()
{
	// モデルレンダラーコンポーネントの追加
	AddComponent<CModelRenderer>();
}

/*****************************************//*
	@brief　	| デストラクタ
*//*****************************************/
CSkyBox::~CSkyBox()
{
}

/*****************************************//*
	@brief　	| 初期化処理
*//*****************************************/
void CSkyBox::Init()
{
	// 基底クラスの初期化処理
	CGameObject::Init();

	// モデルレンダラーコンポーネントの設定
	CModelRenderer* pModelRenderer = GetComponent<CModelRenderer>();
	pModelRenderer->SetKey("SkyBox");
	pModelRenderer->SetRendererParam(m_tParam);

	// 頂点シェーダーの設定
	VertexShader* pVS = CShaderManager::GetInstance()->GetVertexShader(VSType::Object);
	pModelRenderer->SetVertexShader(pVS);

	// ピクセルシェーダーの設定
	PixelShader* pPS = CShaderManager::GetInstance()->GetPixelShader(PSType::TexColor);
	pModelRenderer->SetPixelShader(pPS);

}

/*****************************************//*
	@brief　	| 更新処理
*//*****************************************/
void CSkyBox::Update()
{
	// 基底クラスの更新処理
	CGameObject::Update();

	// カメラに追従させる
	CCamera* pCamera = CCamera::GetInstance();
	DirectX::XMFLOAT3 cameraPos = pCamera->GetPos();
	m_tParam.m_f3Pos = cameraPos;

	// スカイボックスを徐々に回転させる
	m_tParam.m_f3Rotate.y = TORAD(360.0f) * CGameTimeManager::GetInstance()->GetDayProgress();

	// 時間経過で色を変化させる
	CGameTimeManager::DAY_TIME eDayTime = CGameTimeManager::GetInstance()->GetCurrentDayTime();

	// 時間帯別進行度を取得
	float timeOfDayProgress = CGameTimeManager::GetInstance()->GetTimeOfDayProgress();

	// 次の時間帯を取得
	int nextTimeIndex = (static_cast<int>(eDayTime) + 1) % 4;

	DirectX::XMFLOAT4 currentColor = SkyBoxColors[static_cast<int>(eDayTime)];

	DirectX::XMFLOAT4 nextColor = SkyBoxColors[nextTimeIndex];

	// 色の線形補間
	m_tParam.m_f4Color.x = currentColor.x + (nextColor.x - currentColor.x) * timeOfDayProgress;
	m_tParam.m_f4Color.y = currentColor.y + (nextColor.y - currentColor.y) * timeOfDayProgress;
	m_tParam.m_f4Color.z = currentColor.z + (nextColor.z - currentColor.z) * timeOfDayProgress;
	m_tParam.m_f4Color.w = 1.0f;
}