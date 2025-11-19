#include "Camera.h"
#include "Defines.h"
#include "CameraDebug.h"
#include "CameraGame.h"

// ê√ìIïœêîÇÃèâä˙âª
CameraKind CCamera::m_eCameraKind = CameraKind::CAM_DEBUG;
CCamera* CCamera::m_pInstance[] = {};

CCamera::CCamera()
	: m_f3Pos{ 0.0f, 10.0f, -10.0f  }, m_f3Look{ 0.0f,0.0f,0.0f }, m_f3Up{ 0.0f,1.0f,0.0f }
	, m_fFovy(DirectX::XMConvertToRadians(60.0f)), m_fAspect(16.0f / 9.0f)
	, m_fNear(0.3f), m_fFar(1000.0f)
{

}

CCamera::~CCamera()
{

}

const DirectX::XMFLOAT4X4 CCamera::GetWorldMatrix(DirectX::XMFLOAT3 inPos, DirectX::XMFLOAT3 inSize, DirectX::XMFLOAT3 inRotate, bool transpose)
{
	DirectX::XMMATRIX mWorld =
		DirectX::XMMatrixScaling(inSize.x, inSize.y, inSize.z) *
		DirectX::XMMatrixRotationRollPitchYaw(inRotate.x, inRotate.y, inRotate.z) *
		DirectX::XMMatrixTranslation(inPos.x, inPos.y, inPos.z);
	if (transpose) mWorld = DirectX::XMMatrixTranspose(mWorld);
	DirectX::XMFLOAT4X4 world;
	DirectX::XMStoreFloat4x4(&world, mWorld);
	return world;
}

const DirectX::XMFLOAT4X4 CCamera::GetViewMatrix(bool transpose)
{
	DirectX::XMFLOAT4X4 mat;
	DirectX::XMMATRIX view;

	view = DirectX::XMMatrixLookAtLH(
		DirectX::XMVectorSet(m_f3Pos.x, m_f3Pos.y, m_f3Pos.z, 0.0f),
		DirectX::XMVectorSet(m_f3Look.x, m_f3Look.y, m_f3Look.z, 0.0f),
		DirectX::XMVectorSet(m_f3Up.x, m_f3Up.y, m_f3Up.z, 0.0f));

	if (transpose) view = DirectX::XMMatrixTranspose(view);

	DirectX::XMStoreFloat4x4(&mat, view);

	return mat;
}

const DirectX::XMFLOAT4X4 CCamera::GetProjectionMatrix(bool transpose)
{
	DirectX::XMFLOAT4X4 mat;
	DirectX::XMMATRIX proj;
	proj = DirectX::XMMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fNear, m_fFar);

	if (transpose)proj = DirectX::XMMatrixTranspose(proj);

	DirectX::XMStoreFloat4x4(&mat, proj);

	return mat;
}

const DirectX::XMFLOAT4X4 CCamera::GetBillboardWolrdMatrix(DirectX::XMFLOAT3 pos, bool transpose)
{
	DirectX::XMMATRIX mCamInv = DirectX::XMMatrixIdentity();
	DirectX::XMFLOAT4X4 view = GetViewMatrix(false);
	mCamInv = DirectX::XMLoadFloat4x4(&view);
	mCamInv = DirectX::XMMatrixInverse(nullptr,mCamInv);
	DirectX::XMStoreFloat4x4(&view,mCamInv);
	view._41 = view._42 = view._43 = 0.0f;
	mCamInv = DirectX::XMLoadFloat4x4(&view);
	DirectX::XMMATRIX mWorld =
		mCamInv *
		DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);

	if (transpose) mWorld = DirectX::XMMatrixTranspose(mWorld);

	DirectX::XMFLOAT4X4 world;
	DirectX::XMStoreFloat4x4(&world, mWorld);

	return world;
}

const DirectX::XMFLOAT4X4 CCamera::Get2DWolrdMatrix(DirectX::XMFLOAT2 pos, float rotate, bool transpose)
{
	DirectX::XMMATRIX mWorld = 
		DirectX::XMMatrixScaling(1.0f, -1.0f, 1.0f) *
		DirectX::XMMatrixRotationZ(rotate) *
		DirectX::XMMatrixTranslation(SCREEN_WIDTH / 2.0f + pos.x, SCREEN_HEIGHT / 2.0f - pos.y, 0.0f);

	if (transpose) mWorld = DirectX::XMMatrixTranspose(mWorld);

	DirectX::XMFLOAT4X4 world;
	DirectX::XMStoreFloat4x4(&world, mWorld);

	return world;
}

const DirectX::XMFLOAT4X4 CCamera::Get2DViewMatrix(bool transpose)
{
	DirectX::XMMATRIX mView = DirectX::XMMatrixLookAtLH(
		DirectX::XMVectorSet(0.0f, 0.0f, -0.02f, 0.0f),
		DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

	if (transpose) mView = DirectX::XMMatrixTranspose(mView);

	DirectX::XMFLOAT4X4 view;
	DirectX::XMStoreFloat4x4(&view, mView);

	return view;
}

const DirectX::XMFLOAT4X4 CCamera::Get2DProjectionMatrix(bool transpose)
{
	DirectX::XMMATRIX mProj = 
		DirectX::XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.1f, 10.0f);

	if (transpose) mProj = DirectX::XMMatrixTranspose(mProj);

	DirectX::XMFLOAT4X4 proj;
	DirectX::XMStoreFloat4x4(&proj, mProj);

	return proj;
}

CCamera* CCamera::GetInstance()
{
	if (!m_pInstance[(int)m_eCameraKind])
	{
		switch (m_eCameraKind)
		{
		case CameraKind::CAM_DEBUG:
			m_pInstance[(int)m_eCameraKind] = new CCameraDebug();
			break;
		case CameraKind::CAM_GAME:
			m_pInstance[(int)m_eCameraKind] = new CCameraGame();
			break;
		}
	}
	return m_pInstance[(int)m_eCameraKind];
}

void CCamera::ReleaseInstance()
{
	for (int i = 0; i < (int)CameraKind::MAX_CAMERA; i++)
	{
		delete m_pInstance[i];
		m_pInstance[i] = nullptr;
	}
}

void CCamera::SetCameraKind(CameraKind kind)
{
	m_eCameraKind = kind;
}

CameraKind CCamera::GetCameraKind()
{
	return m_eCameraKind;
}
