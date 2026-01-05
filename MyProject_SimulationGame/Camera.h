/**************************************************//*
	@file	| Camera.h
	@brief	| カメラクラスのヘッダーファイル
*//**************************************************/
#pragma once
#include <DirectXMath.h>

// カメラの種類
enum class CameraKind
{
	CAM_DEBUG,	// デバッグ用カメラ
	CAM_TITLE,	// タイトル用カメラ
	CAM_GAME,	// ゲーム用カメラ

	MAX_CAMERA	// カメラの最大数
};

// カメラクラス
class CCamera
{	
public:
	// @brief コンストラクタ
	CCamera();

	// @brief デストラクタ
	virtual ~CCamera();

	// @brief 更新処理
	virtual void Update() = 0;

	// @brief 初期化処理
	virtual void Init() = 0;
	
public:
	// @brief カメラの種類を設定する
	// @param kind：設定するカメラの種類
	void SetCameraKind(CameraKind kind);

	// @brief カメラの座標を取得する
	// @return カメラの座標(XMFLOAT3)
	const DirectX::XMFLOAT3 GetPos() { return m_f3Pos; }

	// @brief カメラの注視点を取得する
	// @return カメラの注視点(XMFLOAT3)
	const DirectX::XMFLOAT3 GetLook() { return m_f3Look; }

	// @brief カメラの上方向ベクトルを取得する
	// @return カメラの上方向ベクトル(XMFLOAT3)
	const DirectX::XMFLOAT3 GetUp() { return m_f3Up; }

	// @brief ワールド行列を取得する
	// @param inPos：オブジェクトの座標
	// @param inSize：オブジェクトの大きさ
	// @param isRotate：オブジェクトの回転角
	// @param transpose：転置するかどうかのフラグ（デフォルト：true）
	// @return ワールド行列(XMFLOAT4X4)
	const DirectX::XMFLOAT4X4 GetWorldMatrix(DirectX::XMFLOAT3 inPos, DirectX::XMFLOAT3 inSize, DirectX::XMFLOAT3 isRotate,bool transpose = true);

	// @brief ビュー行列を取得する
	// @param transpose：転置するかどうかのフラグ（デフォルト：true）
	const DirectX::XMFLOAT4X4 GetViewMatrix(bool transpose = true);

	// @brief プロジェクション行列を取得する
	// @param transpose：転置するかどうかのフラグ（デフォルト：true）
	// @return プロジェクション行列(XMFLOAT4X4)
	const DirectX::XMFLOAT4X4 GetProjectionMatrix(bool transpose = true);

	// @brief ビルボード用ワールド行列を取得する
	// @param pos：オブジェクトの座標
	// @param transpose：転置するかどうかのフラグ（デフォルト：true）
	// @return ビルボード用ワールド行列(XMFLOAT4X4)
	const DirectX::XMFLOAT4X4 GetBillboardWolrdMatrix(DirectX::XMFLOAT3 pos,bool transpose = true);

	// @brief 2D用ワールド行列を取得する
	// @param pos：オブジェクトの座標
	// @param rotate：オブジェクトの回転角
	// @param transpose：転置するかどうかのフラグ（デフォルト：true）
	// @return 2D用ワールド行列(XMFLOAT4X4)
	const DirectX::XMFLOAT4X4 Get2DWolrdMatrix(DirectX::XMFLOAT2 pos,float rotate,bool transpose = true);

	// @brief 2D用ビュー行列を取得する
	// @param transpose：転置するかどうかのフラグ（デフォルト：true）
	// @return 2D用ビュー行列(XMFLOAT4X4)
	const DirectX::XMFLOAT4X4 Get2DViewMatrix(bool transpose = true);

	// @brief 2D用プロジェクション行列を取得する
	// @param transpose：転置するかどうかのフラグ（デフォルト：true）
	// @return 2D用プロジェクション行列(XMFLOAT4X4)
	const DirectX::XMFLOAT4X4 Get2DProjectionMatrix(bool transpose = true);

	// @brief カメラのインスタンスを取得する
	// @return カメラのインスタンス(CCamera*)
	static CCamera* GetInstance();

	// @brief カメラのインスタンスを解放する
	void ReleaseInstance();

	// @brief カメラの種類を取得する
	// @return カメラの種類(CameraKind)
	CameraKind GetCameraKind();

protected:
	// @brief カメラの座標
	DirectX::XMFLOAT3 m_f3Pos;
	// @brief カメラの注視点
	DirectX::XMFLOAT3 m_f3Look;
	// @brief カメラの上方向ベクトル
	DirectX::XMFLOAT3 m_f3Up;
	// @brief カメラの視野角
	float m_fFovy;
	// @brief カメラのアスペクト比
	float m_fAspect;
	// @brief カメラのニアクリップ距離
	float m_fNear;
	// @brief カメラのファークリップ距離
	float m_fFar;

private:
	// @brief カメラのインスタンス
	static CCamera* m_pInstance[(int)CameraKind::MAX_CAMERA];
	// @brief カメラの種類
	static CameraKind m_eCameraKind;

};
