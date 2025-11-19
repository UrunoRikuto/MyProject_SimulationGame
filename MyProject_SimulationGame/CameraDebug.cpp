#include "CameraDebug.h"
#include "Input.h"
#include "Oparation.h"
#include "Main.h"

constexpr float ce_fCameraRotate(0.001f);

CCameraDebug::CCameraDebug()
	: m_fRadXZ(0.0f)
	, m_fRadY(DirectX::XMConvertToRadians(125.0f))
	, m_fRadius(50.0f)
{

}

CCameraDebug::~CCameraDebug()
{

}

void CCameraDebug::Init()
{
	m_fRadXZ = 0.0f;
	m_fRadY = DirectX::XMConvertToRadians(125.0f);
	m_fRadius = 50.0f;
	m_f3Look = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	// カメラの初期座標を設定
	m_f3Pos.x = cosf(m_fRadY) * sinf(m_fRadXZ) * m_fRadius + m_f3Look.x;
	m_f3Pos.y = sinf(m_fRadY) * m_fRadius + m_f3Look.y;
	m_f3Pos.z = cosf(m_fRadY) * cosf(m_fRadXZ) * m_fRadius + m_f3Look.z;
}

void CCameraDebug::Update()
{
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
	if (IsMouseWheelUp())f3Velocity += f3Forward;
	if (IsMouseWheelDown())f3Velocity -= f3Forward;
	if (IsKeyPress('D'))f3Velocity += f3Right;
	if (IsKeyPress('A'))f3Velocity -= f3Right;
	if (IsKeyPress(VK_SPACE))f3Velocity += fUp;
	if (IsKeyPress(VK_SHIFT))f3Velocity -= fUp;
	if (IsKeyPress('W'))m_fRadY += 0.01f;
	if (IsKeyPress('S'))m_fRadY -= 0.01f;
	if (IsKeyPress('E'))m_fRadXZ += 0.01f;
	if (IsKeyPress('Q'))m_fRadXZ -= 0.01f;

	// 計算したVelocityを注視点に加算
	m_f3Look += f3Velocity;

	// マウスホイールの回転量をリセット
	ResetMouseWheelDelta();

	// 注視点、回転、カメラの距離からカメラの座標を計算する
	m_f3Pos.x = cosf(m_fRadY) * sinf(m_fRadXZ) * m_fRadius + m_f3Look.x;
	m_f3Pos.y = sinf(m_fRadY) * m_fRadius + m_f3Look.y;
	m_f3Pos.z = cosf(m_fRadY) * cosf(m_fRadXZ) * m_fRadius + m_f3Look.z;
}
