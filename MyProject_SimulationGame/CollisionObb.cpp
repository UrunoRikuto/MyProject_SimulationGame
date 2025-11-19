/**************************************************//*
	@file	| CollisionObb.cpp
	@brief	| OBB当たり判定クラス
*//**************************************************/
#include "CollisionObb.h"
#include "GameObject.h"
#include "Geometory.h"

// @brief 衝突が起きたかどうかを取得
// @param other：衝突先
// @return true:衝突 false:非衝突
bool CCollisionObb::IsHit(CCollisionBase* other)
{
	// 投影を行うためにワールド行列を取得
	DirectX::XMFLOAT4X4* pWA = this->GetGameObject()->GetWorld();
	DirectX::XMFLOAT4X4* pWB = other->GetGameObject()->GetWorld();
	DirectX::XMMATRIX wa = DirectX::XMLoadFloat4x4(pWA);
	DirectX::XMMATRIX wb = DirectX::XMLoadFloat4x4(pWB);


	// 中心座標を計算用にXMVECTORに変換
	DirectX::XMFLOAT3 fCenterA, fCenterB;
	fCenterA = dynamic_cast<CCollisionObb*>(this)->GetCenter();
	fCenterB = dynamic_cast<CCollisionObb*>(other)->GetCenter();
	DirectX::XMVECTOR vCenterA, vCenterB;
	vCenterA = DirectX::XMLoadFloat3(&fCenterA);
	vCenterB = DirectX::XMLoadFloat3(&fCenterB);

	DirectX::XMFLOAT3 fPosA, fPosB;
	DirectX::XMStoreFloat3(&fPosA, DirectX::XMVector3TransformCoord(vCenterA, wa));
	DirectX::XMStoreFloat3(&fPosB, DirectX::XMVector3TransformCoord(vCenterB, wb));

	// 中心座標間のベクトルを求める
	DirectX::XMVECTOR vD =
		DirectX::XMVectorSet(fPosA.x - fPosB.x, fPosA.y - fPosB.y, fPosA.z - fPosB.z, 0.0f);

	// モデル座標軸を求める
	DirectX::XMVECTOR vN[6];
	vN[0] = DirectX::XMVectorSet(pWA->_11, pWA->_12, pWA->_13, 0.0f);   // X軸
	vN[1] = DirectX::XMVectorSet(pWA->_21, pWA->_22, pWA->_23, 0.0f);   // Y軸
	vN[2] = DirectX::XMVectorSet(pWA->_31, pWA->_32, pWA->_33, 0.0f);   // Z軸

	vN[3] = DirectX::XMVectorSet(pWB->_11, pWB->_12, pWB->_13, 0.0f);   // X軸
	vN[4] = DirectX::XMVectorSet(pWB->_21, pWB->_22, pWB->_23, 0.0f);   // Y軸
	vN[5] = DirectX::XMVectorSet(pWB->_31, pWB->_32, pWB->_33, 0.0f);   // Z軸

	// OBBの大きさ(半分)の長さを掛けたベクトルを求める
	DirectX::XMVECTOR vL[6];
	DirectX::XMFLOAT3 hsA = dynamic_cast<CCollisionObb*>(this)->GetSize() / 2.0f;
	DirectX::XMFLOAT3 hsB = dynamic_cast<CCollisionObb*>(other)->GetSize() / 2.0f;
	vL[0] = DirectX::XMVectorSet(pWA->_11 * hsA.x, pWA->_12 * hsA.x, pWA->_13 * hsA.x, 0.0f);   // X軸
	vL[1] = DirectX::XMVectorSet(pWA->_21 * hsA.y, pWA->_22 * hsA.y, pWA->_23 * hsA.y, 0.0f);
	vL[2] = DirectX::XMVectorSet(pWA->_31 * hsA.z, pWA->_32 * hsA.z, pWA->_33 * hsA.z, 0.0f);

	vL[3] = DirectX::XMVectorSet(pWB->_11 * hsB.x, pWB->_12 * hsB.x, pWB->_13 * hsB.x, 0.0f);
	vL[4] = DirectX::XMVectorSet(pWB->_21 * hsB.y, pWB->_22 * hsB.y, pWB->_23 * hsB.y, 0.0f);
	vL[5] = DirectX::XMVectorSet(pWB->_31 * hsB.z, pWB->_32 * hsB.z, pWB->_33 * hsB.z, 0.0f);

	// 分離軸候補はモデル座標軸
	float fL;
	float fD;
	float f; 
	DirectX::XMVECTOR vS;	// 分離軸候補(計算用の一時変数)
	for (int i = 0; i < 6; ++i) {
		vS = vN[i];
		// OBBの影(半分)の合計
		fL = 0.0f;
		for (int j = 0; j < 6; ++j) {
			DirectX::XMStoreFloat(&f, DirectX::XMVector3Dot(vS, vL[j]));
			fL += fabsf(f);
		}
		// 影と中心間の距離の比較
		DirectX::XMStoreFloat(&f, DirectX::XMVector3Dot(vS, vD));
		fD = fabsf(f);
		if (fL < fD) {
			return false;		// 当たっていない
		}
	}

	// 分離軸候補は2辺から生成
	for (int i = 0; i < 3; ++i) {
		for (int j = 3; j < 6; ++j) {
			// 分離軸候補を計算
			vS = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(vN[i], vN[j]));
			// OBBの影(半分)の合計
			fL = 0.0f;
			for (int k = 0; k < 6; ++k) {
				DirectX::XMStoreFloat(&f, DirectX::XMVector3Dot(vS, vL[k]));
				fL += fabsf(f);
			}
			// 影と中心間の距離の比較
			DirectX::XMStoreFloat(&f, DirectX::XMVector3Dot(vS, vD));
			fD = fabsf(f);
			if (fL < fD) {
				return false;	// 当たっていない
			}
		}
	}
	return true;				// 当たっている
}

// @brief 描画処理
void CCollisionObb::Draw()
{
	// コリジョンが有効でない時は描画を行わない
	if (!m_bActive) return;

	// 頂点情報の設定
	DirectX::XMFLOAT3 vertex[8] = {
	   {-m_tCollisionInfo.m_f3HalfSize.x, +m_tCollisionInfo.m_f3HalfSize.y, -m_tCollisionInfo.m_f3HalfSize.z}, // 0
	   {-m_tCollisionInfo.m_f3HalfSize.x, -m_tCollisionInfo.m_f3HalfSize.y, -m_tCollisionInfo.m_f3HalfSize.z}, // 1
	   {+m_tCollisionInfo.m_f3HalfSize.x, +m_tCollisionInfo.m_f3HalfSize.y, -m_tCollisionInfo.m_f3HalfSize.z}, // 2
	   {+m_tCollisionInfo.m_f3HalfSize.x, -m_tCollisionInfo.m_f3HalfSize.y, -m_tCollisionInfo.m_f3HalfSize.z}, // 3
	   {-m_tCollisionInfo.m_f3HalfSize.x, +m_tCollisionInfo.m_f3HalfSize.y, +m_tCollisionInfo.m_f3HalfSize.z}, // 4
	   {-m_tCollisionInfo.m_f3HalfSize.x, -m_tCollisionInfo.m_f3HalfSize.y, +m_tCollisionInfo.m_f3HalfSize.z}, // 5
	   {+m_tCollisionInfo.m_f3HalfSize.x, +m_tCollisionInfo.m_f3HalfSize.y, +m_tCollisionInfo.m_f3HalfSize.z}, // 6
	   {+m_tCollisionInfo.m_f3HalfSize.x, -m_tCollisionInfo.m_f3HalfSize.y, +m_tCollisionInfo.m_f3HalfSize.z}, // 7
	};

	// コンポーネントに紐付けられているGameObjectから回転情報を取得し、描画に使用する
	DirectX::XMFLOAT3 rotate = this->GetGameObject()->GetRotate();
	DirectX::XMMATRIX rot = DirectX::XMMatrixRotationRollPitchYaw(rotate.x, rotate.y, rotate.z);

	for (int i = 0; i < 8; ++i)
	{
		DirectX::XMVECTOR v = DirectX::XMLoadFloat3(&vertex[i]);    // ローカル空間
		v = DirectX::XMVector3Transform(v, rot);    // 回転適用
		v = DirectX::XMVectorAdd(v, DirectX::XMLoadFloat3(&m_tCollisionInfo.m_f3Center)); // ワールド中心を加算
		DirectX::XMStoreFloat3(&vertex[i], v);      // 結果を保存
	}

	// 前面
	Geometory::AddLine(vertex[0], vertex[1], DirectX::XMFLOAT4(0.0f,1.0f,0.0f,1.0f));
	Geometory::AddLine(vertex[0], vertex[2], DirectX::XMFLOAT4(0.0f,1.0f,0.0f,1.0f));
	Geometory::AddLine(vertex[3], vertex[1], DirectX::XMFLOAT4(0.0f,1.0f,0.0f,1.0f));
	Geometory::AddLine(vertex[3], vertex[2], DirectX::XMFLOAT4(0.0f,1.0f,0.0f,1.0f));

	// 背面
	Geometory::AddLine(vertex[4], vertex[5], DirectX::XMFLOAT4(0.0f,1.0f,0.0f,1.0f));
	Geometory::AddLine(vertex[4], vertex[6], DirectX::XMFLOAT4(0.0f,1.0f,0.0f,1.0f));
	Geometory::AddLine(vertex[7], vertex[5], DirectX::XMFLOAT4(0.0f,1.0f,0.0f,1.0f));
	Geometory::AddLine(vertex[7], vertex[6], DirectX::XMFLOAT4(0.0f,1.0f,0.0f,1.0f));

	// 側面
	Geometory::AddLine(vertex[0], vertex[4], DirectX::XMFLOAT4(0.0f,1.0f,0.0f,1.0f));
	Geometory::AddLine(vertex[1], vertex[5], DirectX::XMFLOAT4(0.0f,1.0f,0.0f,1.0f));
	Geometory::AddLine(vertex[2], vertex[6], DirectX::XMFLOAT4(0.0f,1.0f,0.0f,1.0f));
	Geometory::AddLine(vertex[3], vertex[7], DirectX::XMFLOAT4(0.0f,1.0f,0.0f,1.0f));

	// 線の描画
	Geometory::DrawLines();
}
