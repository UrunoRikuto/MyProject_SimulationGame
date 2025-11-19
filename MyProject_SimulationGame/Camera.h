// インクルード部
#pragma once
#include <DirectXMath.h>

/// カメラの種類
enum class CameraKind
{
	CAM_DEBUG,	// デバッグ用カメラ
	CAM_GAME,	// ゲーム用カメラ

	MAX_CAMERA	// カメラの最大数
};

/// <summary>
/// カメラの情報を統合しているシングルトン基底クラス 
/// </summary>
class CCamera
{	
public:
	CCamera();
	virtual ~CCamera();
	virtual void Update() = 0;
	
public:
	// ==============================
	//    アクセサ
	// ==============================
	// 
	// ------------Setter------------
	/// <summary> 現在のカメラの種類をセットする </summary>
	/// <param name="kind"> カメラの種類 </param>
	void SetCameraKind(CameraKind kind);

	// ------------Getter------------
	/// <summary> カメラの座標を取得する </summary>
	/// <returns> カメラの座標(XMFLOAT3) </returns>
	const DirectX::XMFLOAT3 GetPos() { return m_f3Pos; }
	/// <summary> カメラの注視点を取得する </summary>
	/// <returns> カメラの注視点(XMFLOAT3) </returns>
	const DirectX::XMFLOAT3 GetLook() { return m_f3Look; }
	/// <summary> カメラの上方向ベクトルを取得する </summary>
	/// <returns> カメラの上方向ベクトル(XMFLOAT3) </returns>
	const DirectX::XMFLOAT3 GetUp() { return m_f3Up; }

	/// <summary> 3D用のカメラワールド座標を取得する </summary>
	/// <param name="inPos"> モデルの座標 </param>
	/// <param name="inSize"> モデルのサイズ false:転置していない </param>
	/// <param name="inRotate"> モデルの回転 false:転置していない </param>
	/// <param name="transpose"> true:転置済み false:転置していない </param>
	/// <returns> 3D用のカメラビュー座標(XMFLOAT4X4) </returns>
	const DirectX::XMFLOAT4X4 GetWorldMatrix(DirectX::XMFLOAT3 inPos, DirectX::XMFLOAT3 inSize, DirectX::XMFLOAT3 isRotate,bool transpose = true);

	/// <summary> 3D用のカメラビュー座標を取得する </summary>
	/// <param name="transpose"> true:転置済み false:転置していない </param>
	/// <returns> 3D用のカメラビュー座標(XMFLOAT4X4) </returns>
	const DirectX::XMFLOAT4X4 GetViewMatrix(bool transpose = true);

	/// <summary> 3D用のカメラプロジェクション座標を取得する </summary>
	/// <param name="transpose"> true:転置済み false:転置していない </param>
	/// <returns> 3D用のカメラプロジェクション座標(XMFLOAT4X4) </returns>
	const DirectX::XMFLOAT4X4 GetProjectionMatrix(bool transpose = true);

	/// <summary> 座標を反映させた2Dビルボード用のカメラワールド座標を取得する </summary>
	/// <param name="pos"> 3D画面上の座標 </param>
	/// <param name="transpose"> true:転置済み false:転置していない </param>
	/// <returns> 2D用のカメラワールド座標(XMFLOAT4X4) </returns>
	const DirectX::XMFLOAT4X4 GetBillboardWolrdMatrix(DirectX::XMFLOAT3 pos,bool transpose = true);

	/// <summary> 座標、回転を反映させた2D用のカメラワールド座標を取得する </summary>
	/// <param name="pos"> 2D画面上の座標 </param>
	/// <param name="rotate"> スプライトの回転(rad) </param>
	/// <param name="transpose"> true:転置済み false:転置していない </param>
	/// <returns> 2D用のカメラワールド座標(XMFLOAT4X4) </returns>
	const DirectX::XMFLOAT4X4 Get2DWolrdMatrix(DirectX::XMFLOAT2 pos,float rotate,bool transpose = true);

	/// <summary> 2D用のカメラビュー座標を取得する </summary>
	/// <param name="transpose"> true:転置済み false:転置していない </param>
	/// <returns> 2D用のカメラビュー座標(XMFLOAT4X4) </returns>
	const DirectX::XMFLOAT4X4 Get2DViewMatrix(bool transpose = true);

	/// <summary> 2D用のカメラビュー座標を取得する </summary>
	/// <param name="transpose"> true:転置済み false:転置していない </param>
	/// <returns> 2D用のカメラビュー座標(XMFLOAT4X4) </returns>
	const DirectX::XMFLOAT4X4 Get2DProjectionMatrix(bool transpose = true);

	/// <summary> カメラのインスタンスを取得する </summary>
	/// <returns> CCamera型クラスのポインタ(CCamera*) </returns>
	static CCamera* GetInstance();

	/// <summary> カメラのインスタンスを解放する </summary>
	void ReleaseInstance();

	/// <summary> 現在のカメラの種類を取得する </summary>
	/// <returns> 現在のカメラの種類(CameraKind) </returns>
	CameraKind GetCameraKind();

protected:
	DirectX::XMFLOAT3 m_f3Pos;	// カメラの座標
	DirectX::XMFLOAT3 m_f3Look;	// カメラの注視点
	DirectX::XMFLOAT3 m_f3Up;	// 上方向ベクトル
	float m_fFovy;		// 画角
	float m_fAspect;	// アスペクト比
	float m_fNear;		// ニアクリップ
	float m_fFar;		// ファークリップ

private:
	static CCamera* m_pInstance[(int)CameraKind::MAX_CAMERA];
	static CameraKind m_eCameraKind;	// 現在のカメラの種類

};
