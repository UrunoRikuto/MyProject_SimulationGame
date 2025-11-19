#include "CameraDebug.h"
#include "Input.h"
#include "Oparation.h"

constexpr float ce_fCameraRotate(0.001f);

CCameraDebug::CCameraDebug()
	: m_fRadXZ(0.0f), m_fRadY(DirectX::XMConvertToRadians(125.0f)), m_fRadius(50.0f)
{

}

CCameraDebug::~CCameraDebug()
{

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
	if (IsKeyPress('W'))f3Velocity += f3Forward;
	if (IsKeyPress('S'))f3Velocity -= f3Forward;
	if (IsKeyPress('D'))f3Velocity += f3Right;
	if (IsKeyPress('A'))f3Velocity -= f3Right;
	if (IsKeyPress('Q'))f3Velocity += fUp;
	if (IsKeyPress('E'))f3Velocity -= fUp;

	// 計算したVelocityを注視点に加算
	m_f3Look += f3Velocity;

	// 右クリックした際にカーソル表示を無効化し、マウスの座標を中央に移す
	if (IsMouseButtonTrigger(MOUSEBUTTON_R))
	{
		POINT center;
		center.x = 0;
		center.y = 0;
		ShowCursor(false);
		SetMousePosition(center);
	}

	// 右クリックを押しながらカメラの回転を行う
	if (IsMouseButtonPress(MOUSEBUTTON_R))
	{
		// マウス座標から中央からの移動量を取得する
		POINT mousePos = *GetMousePosition();
		// 移動量を回転に反映させる
		m_fRadXZ += mousePos.x * ce_fCameraRotate;
		m_fRadY -= mousePos.y * ce_fCameraRotate;

		// カメラの座標を中央に戻す
		POINT center;
		center.x = 0;
		center.y = 0;
		SetMousePosition(center);
	}

	// 右クリックを離した際にカーソル表示をもとに戻す
	if (IsMouseButtonRelease(MOUSEBUTTON_R))
	{
		ShowCursor(true);
	}

	// 注視点、回転、カメラの距離からカメラの座標を計算する
	m_f3Pos.x = cosf(m_fRadY) * sinf(m_fRadXZ) * m_fRadius + m_f3Look.x;
	m_f3Pos.y = sinf(m_fRadY) * m_fRadius + m_f3Look.y;
	m_f3Pos.z = cosf(m_fRadY) * cosf(m_fRadXZ) * m_fRadius + m_f3Look.z;
}
