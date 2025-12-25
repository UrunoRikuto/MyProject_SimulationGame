/**************************************************//*
	@file	| Camera.cpp
	@brief	| カメラクラス実装
*//**************************************************/
#include "Camera.h"
#include "Defines.h"
#include "CameraDebug.h"
#include "CameraGame.h"
#include <new>

// 静的メンバ変数の初期化
CameraKind CCamera::m_eCameraKind = CameraKind::CAM_DEBUG;
CCamera* CCamera::m_pInstance[] = {};

/****************************************//*
	@brief　	| コンストラクタ
*//****************************************/
CCamera::CCamera()
	: m_f3Pos{ 0.0f, 10.0f, -10.0f  }, m_f3Look{ 0.0f,0.0f,0.0f }, m_f3Up{ 0.0f,1.0f,0.0f }
	, m_fFovy(DirectX::XMConvertToRadians(60.0f)), m_fAspect(16.0f / 9.0f)
	, m_fNear(0.3f), m_fFar(1000.0f)
{

}

/****************************************//*
	@brief　	| デストラクタ
*//****************************************/
CCamera::~CCamera()
{

}

/****************************************//*
	@brief　	| ワールド行列を取得する
	@param		| inPos：オブジェクトの座標
	@param		| inSize：オブジェクトの大きさ
	@param		| inRotate：オブジェクトの回転角
	@param		| transpose：転置するかどうかのフラグ
	@return		| ワールド行列(XMFLOAT4X4)
*//****************************************/
const DirectX::XMFLOAT4X4 CCamera::GetWorldMatrix(DirectX::XMFLOAT3 inPos, DirectX::XMFLOAT3 inSize, DirectX::XMFLOAT3 inRotate, bool transpose)
{
	// ワールド行列の計算
	DirectX::XMMATRIX mWorld =
		DirectX::XMMatrixScaling(inSize.x, inSize.y, inSize.z) *
		DirectX::XMMatrixRotationRollPitchYaw(inRotate.x, inRotate.y, inRotate.z) *
		DirectX::XMMatrixTranslation(inPos.x, inPos.y, inPos.z);

	// 転置行列の計算
	if (transpose) mWorld = DirectX::XMMatrixTranspose(mWorld);

	// ワールド行列の格納
	DirectX::XMFLOAT4X4 world;
	DirectX::XMStoreFloat4x4(&world, mWorld);

	return world;
}

/****************************************//*
	@brief　	| ビュー行列を取得する
	@param		| transpose：転置するかどうかのフラグ
	@return		| ビュー行列(XMFLOAT4X4)
*//****************************************/
const DirectX::XMFLOAT4X4 CCamera::GetViewMatrix(bool transpose)
{
	// ビュー行列の計算
	DirectX::XMMATRIX mView = DirectX::XMMatrixLookAtLH(
		DirectX::XMVectorSet(m_f3Pos.x, m_f3Pos.y, m_f3Pos.z, 0.0f),
		DirectX::XMVectorSet(m_f3Look.x, m_f3Look.y, m_f3Look.z, 0.0f),
		DirectX::XMVectorSet(m_f3Up.x, m_f3Up.y, m_f3Up.z, 0.0f));

	// 転置行列の計算
	if (transpose) mView = DirectX::XMMatrixTranspose(mView);

	// ビュー行列の格納
	DirectX::XMFLOAT4X4 view;
	DirectX::XMStoreFloat4x4(&view, mView);

	return view;
}

/****************************************//*
	@brief　	| プロジェクション行列を取得する
	@param		| transpose：転置するかどうかのフラグ
	@return		| プロジェクション行列(XMFLOAT4X4)
*//****************************************/
const DirectX::XMFLOAT4X4 CCamera::GetProjectionMatrix(bool transpose)
{
	// プロジェクション行列の計算
	DirectX::XMMATRIX proj = DirectX::XMMatrixPerspectiveFovLH(
		m_fFovy,
		m_fAspect,
		m_fNear,
		m_fFar
	);

	// 転置行列の計算
	if (transpose)proj = DirectX::XMMatrixTranspose(proj);

	// プロジェクション行列の格納
	DirectX::XMFLOAT4X4 mat;
	DirectX::XMStoreFloat4x4(&mat, proj);

	return mat;
}

/****************************************//*
	@brief　	| ビルボード用ワールド行列を取得する
	@param		| pos：オブジェクトの座標
	@param		| transpose：転置するかどうかのフラグ
	@return		| ビルボード用ワールド行列(XMFLOAT4X4)
*//****************************************/
const DirectX::XMFLOAT4X4 CCamera::GetBillboardWolrdMatrix(DirectX::XMFLOAT3 pos, bool transpose)
{
	// ビルボード用ワールド行列の計算
	// カメラの逆行列を取得
	DirectX::XMMATRIX mCamInv = DirectX::XMMatrixIdentity();
	// ビュー行列を取得
	DirectX::XMFLOAT4X4 view = GetViewMatrix(false);
	
	// ビュー行列の逆行列を計算
	mCamInv = DirectX::XMLoadFloat4x4(&view);
	mCamInv = DirectX::XMMatrixInverse(nullptr,mCamInv);
	DirectX::XMStoreFloat4x4(&view,mCamInv);
	// 平行移動成分を0にする
	view._41 = view._42 = view._43 = 0.0f;
	mCamInv = DirectX::XMLoadFloat4x4(&view);

	// ビルボード用ワールド行列の計算
	DirectX::XMMATRIX mWorld =
		mCamInv *
		DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);

	// 転置行列の計算
	if (transpose) mWorld = DirectX::XMMatrixTranspose(mWorld);

	// ビルボード用ワールド行列の格納
	DirectX::XMFLOAT4X4 world;
	DirectX::XMStoreFloat4x4(&world, mWorld);

	return world;
}

/****************************************//*
	@brief　	| 2D用ワールド行列を取得する
	@param		| pos：オブジェクトの座標
	@param		| rotate：オブジェクトの回転角
	@param		| transpose：転置するかどうかのフラグ
	@return		| 2D用ワールド行列(XMFLOAT4X4)
*//****************************************/
const DirectX::XMFLOAT4X4 CCamera::Get2DWolrdMatrix(DirectX::XMFLOAT2 pos, float rotate, bool transpose)
{
	// 2D用ワールド行列の計算
	DirectX::XMMATRIX mWorld = 
		DirectX::XMMatrixScaling(1.0f, -1.0f, 1.0f) *
		DirectX::XMMatrixRotationZ(rotate) *
		DirectX::XMMatrixTranslation(SCREEN_WIDTH / 2.0f + pos.x, SCREEN_HEIGHT / 2.0f - pos.y, 0.0f);

	// 転置行列の計算
	if (transpose) mWorld = DirectX::XMMatrixTranspose(mWorld);

	// 2D用ワールド行列の格納
	DirectX::XMFLOAT4X4 world;
	DirectX::XMStoreFloat4x4(&world, mWorld);

	return world;
}

/****************************************//*
	@brief　	| 2D用ビュー行列を取得する
	@param		| transpose：転置するかどうかのフラグ
	@return		| 2D用ビュー行列(XMFLOAT4X4)
*//****************************************/
const DirectX::XMFLOAT4X4 CCamera::Get2DViewMatrix(bool transpose)
{
	// 2D用ビュー行列の計算
	DirectX::XMMATRIX mView = DirectX::XMMatrixLookAtLH(
		DirectX::XMVectorSet(0.0f, 0.0f, -0.02f, 0.0f),
		DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

	// 転置行列の計算
	if (transpose) mView = DirectX::XMMatrixTranspose(mView);

	// 2D用ビュー行列の格納
	DirectX::XMFLOAT4X4 view;
	DirectX::XMStoreFloat4x4(&view, mView);

	return view;
}

/****************************************//*
	@brief　	| 2D用プロジェクション行列を取得する
	@param		| transpose：転置するかどうかのフラグ
	@return		| 2D用プロジェクション行列(XMFLOAT4X4)
*//****************************************/
const DirectX::XMFLOAT4X4 CCamera::Get2DProjectionMatrix(bool transpose)
{
	// 2D用プロジェクション行列の計算
	DirectX::XMMATRIX mProj = 
		DirectX::XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.1f, 10.0f);

	// 転置行列の計算
	if (transpose) mProj = DirectX::XMMatrixTranspose(mProj);

	// 2D用プロジェクション行列の格納
	DirectX::XMFLOAT4X4 proj;
	DirectX::XMStoreFloat4x4(&proj, mProj);

	return proj;
}

/****************************************//*
	@brief　	| カメラのインスタンスを取得する
	@return		| カメラのインスタンス(CCamera*)
*//****************************************/
CCamera* CCamera::GetInstance()
{
	// インスタンスが生成されていなければ生成する
	if (!m_pInstance[(int)m_eCameraKind])
	{
		// カメラの種類によってインスタンスを生成
		switch (m_eCameraKind)
		{
		case CameraKind::CAM_DEBUG:
			m_pInstance[(int)m_eCameraKind] = new(std::nothrow) CCameraDebug();
			break;
		case CameraKind::CAM_GAME:
			m_pInstance[(int)m_eCameraKind] = new(std::nothrow) CCameraGame();
			break;
		}
	}

	// インスタンスを返す
	return m_pInstance[(int)m_eCameraKind];
}

/****************************************//*
	@brief　	| カメラのインスタンスを解放する
*//****************************************/
void CCamera::ReleaseInstance()
{
	// インスタンスの解放
	for (int i = 0; i < (int)CameraKind::MAX_CAMERA; i++)
	{
		delete m_pInstance[i];
		m_pInstance[i] = nullptr;
	}
}

/****************************************//*
	@brief　	| カメラの種類を設定する
	@param		| kind：設定するカメラの種類
*//****************************************/
void CCamera::SetCameraKind(CameraKind kind)
{
	m_eCameraKind = kind;
}

/****************************************//*
	@brief　	| カメラの種類を取得する
	@return		| カメラの種類(CameraKind)
*//****************************************/
CameraKind CCamera::GetCameraKind()
{
	return m_eCameraKind;
}
