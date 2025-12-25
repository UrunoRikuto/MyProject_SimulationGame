/**************************************************//*
	@file	| CameraGame.cpp
	@brief	| ゲームカメラクラス実装
*//**************************************************/
#include "CameraGame.h"	
#include "Input.h"
#include "Oparation.h"
#include "Main.h"
#include "ImguiSystem.h"
#include <algorithm>

// 回転速度
constexpr float ce_fCameraRotate(0.001f);

// ズームの上限・下限
constexpr float MAX_RADIUS_ZOOM = 100.0f;
constexpr float MIN_RADIUS_ZOOM = 5.0f;

/*****************************************//*
	@brief　 | コンストラクタ
*//*****************************************/
CCameraGame::CCameraGame()
	: m_fRadXZ(0.0f)
	, m_fRadY(DirectX::XMConvertToRadians(150.0f))
	, m_fRadius(50.0f)
	, m_fChangeOldRadius(m_fRadius)
	, m_bSetOldRadius(false)
{
	// 注視点の初期化
	m_f3Look = DirectX::XMFLOAT3(0.0f,0.0f,0.0f);
}

/*****************************************//*
	@brief　 | デストラクタ
*//*****************************************/
CCameraGame::~CCameraGame()
{

}

/*****************************************//*
	@brief　 | 初期化処理
*//*****************************************/
void CCameraGame::Init()
{
	// 回転角度の初期化
	m_fRadXZ = 0.0f;
	m_fRadY = DirectX::XMConvertToRadians(125.0f);
	// カメラの距離の初期化
	m_fRadius = 50.0f;
	// Radius保存用の初期化
	m_fChangeOldRadius = m_fRadius;
	// 古いRadius保存フラグの初期化
	m_bSetOldRadius = false;
	// 注視点の初期化
	m_f3Look = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	// カメラの初期座標を設定
	m_f3Pos.x = cosf(m_fRadY) * sinf(m_fRadXZ) * m_fRadius + m_f3Look.x;
	m_f3Pos.y = sinf(m_fRadY) * m_fRadius + m_f3Look.y;
	m_f3Pos.z = cosf(m_fRadY) * cosf(m_fRadXZ) * m_fRadius + m_f3Look.z;
}

/*****************************************//*
	@brief　 | 更新処理
*//*****************************************/
void CCameraGame::Update()
{
	// Imguiで選択しているオブジェクトを取得
	CGameObject* pObject = CImguiSystem::GetInstance()->GetSelectedGameObject();

	// 選択しているオブジェクトがない場合、自由移動カメラとして動作
	if (pObject == nullptr)
	{
		// 古いRadiusの値を復元
		if (m_bSetOldRadius)
		{
			// Radiusの値を復元
			m_fRadius = m_fChangeOldRadius;
			m_bSetOldRadius = false;
		}

		// カメラの座標と注視点を使い、前方向ベクトルを取得
		DirectX::XMFLOAT3 f3Forward = m_f3Look - m_f3Pos;
		DirectX::XMVECTOR vForward = DirectX::XMLoadFloat3(&f3Forward);
		vForward = DirectX::XMVector3Normalize(vForward);
		DirectX::XMStoreFloat3(&f3Forward, vForward);

		// カメラの規定の上方向ベクトルを取得
		DirectX::XMFLOAT3 fUp = m_f3Up;
		DirectX::XMVECTOR vUp = DirectX::XMLoadFloat3(&m_f3Up);
		vUp = DirectX::XMVector3Normalize(vUp);
		DirectX::XMStoreFloat3(&fUp, vUp);

		// カメラの前方向ベクトルと上方向ベクトルの内積を使い、右方向ベクトルを取得
		DirectX::XMVECTOR vRight = DirectX::XMVector3Cross(vUp, vForward);
		DirectX::XMFLOAT3 f3Right;
		DirectX::XMStoreFloat3(&f3Right, vRight);

		// キーボード入力からVelocityを計算
		DirectX::XMFLOAT3 f3Velocity{};
		
		// マウスホイール、キー入力で注視点の移動やズームを計算
		if (IsMouseWheelUp())m_fRadius -= 2.0f;
		if (IsMouseWheelDown())m_fRadius += 2.0f;
		if (IsKeyPress(VK_SPACE))f3Velocity += fUp;
		if (IsKeyPress(VK_SHIFT))f3Velocity -= fUp;

		// 前後左右の移動
		if (IsKeyPress('W'))f3Velocity += f3Forward;
		if (IsKeyPress('S'))f3Velocity -= f3Forward;
		if (IsKeyPress('D'))f3Velocity += f3Right;
		if (IsKeyPress('A'))f3Velocity -= f3Right;

		// カメラの回転
		if(IsKeyPress(VK_UP))m_fRadY += 0.03f;
		if(IsKeyPress(VK_DOWN))m_fRadY -= 0.03f;
		if(IsKeyPress(VK_RIGHT))m_fRadXZ += 0.03f;
		if(IsKeyPress(VK_LEFT))m_fRadXZ -= 0.03f;

		// 計算したVelocityを注視点に加算
		m_f3Look += f3Velocity;

		// ズームの下限・上限（必要なら）
		m_fRadius = std::clamp(m_fRadius, MIN_RADIUS_ZOOM, MAX_RADIUS_ZOOM);

		// マウスホイールの回転量をリセット
		ResetMouseWheelDelta();

		// 注視点、回転、カメラの距離からカメラの座標を計算する
		m_f3Pos.x = cosf(m_fRadY) * sinf(m_fRadXZ) * m_fRadius + m_f3Look.x;
		m_f3Pos.y = sinf(m_fRadY) * m_fRadius + m_f3Look.y;
		m_f3Pos.z = cosf(m_fRadY) * cosf(m_fRadXZ) * m_fRadius + m_f3Look.z;

		// Radiusの値を保存
		m_fChangeOldRadius = m_fRadius;
	}
	// 選択しているオブジェクトがある場合、そのオブジェクトを注視点にする
	else
	{
		// 古いRadiusの値を保存
		m_bSetOldRadius = true;

		// 注視点を選択しているオブジェクトの座標にする
		m_f3Look = pObject->GetPos();

		// マウスホイール、キー入力でズームや回転を計算
		if (IsMouseWheelUp())m_fRadius -= 2.0f;
		if (IsMouseWheelDown())m_fRadius += 2.0f;
		if (IsKeyPress('W'))m_fRadY += 0.02f;
		if (IsKeyPress('S'))m_fRadY -= 0.02f;
		if (IsKeyPress('D'))m_fRadXZ += 0.02f;
		if (IsKeyPress('A'))m_fRadXZ -= 0.02f;

		// ズームの下限・上限（必要なら）
		m_fRadius = std::clamp(m_fRadius, MIN_RADIUS_ZOOM, MAX_RADIUS_ZOOM);

		// マウスホイールの回転量をリセット
		ResetMouseWheelDelta();

		// 注視点、回転、カメラの距離からカメラの座標を計算する
		m_f3Pos.x = cosf(m_fRadY) * sinf(m_fRadXZ) * m_fRadius + m_f3Look.x;
		m_f3Pos.y = sinf(m_fRadY) * m_fRadius + m_f3Look.y;
		m_f3Pos.z = cosf(m_fRadY) * cosf(m_fRadXZ) * m_fRadius + m_f3Look.z;
	}
}
