/**************************************************//*
	@file	| CollisionObb.h
	@brief	| OBB当たり判定クラス
*//**************************************************/
#pragma once
#include "CollisionBase.h"
#include <DirectXMath.h>
#include "Oparation.h"

// @brief OBB用パラメータ
struct ObbInfo
{
	// 中心座標
	DirectX::XMFLOAT3 m_f3Center;

	// ハーフサイズ
	DirectX::XMFLOAT3 m_f3HalfSize;
};

// @brief OBB当たり判定クラス
class CCollisionObb : public CCollisionBase
{
public:
	// コンストラクタの継承
	using CCollisionBase::CCollisionBase;

	// @brief 描画処理
	void Draw() override;

	// @brief 衝突が起きたかどうかを取得
	// @param other：衝突先
	// @return true:衝突 false:非衝突
	bool IsHit(CCollisionBase* other) override;

	// @brief コリジョン情報の中心座標
	// @return (DirectX::XMFLOAT3)コリジョンの中心座標
	DirectX::XMFLOAT3 GetCenter() { return m_tCollisionInfo.m_f3Center; }

	// @brief コリジョンの中心座標をセット
	// @param center：コリジョンの中心座標
	void SetCenter(const DirectX::XMFLOAT3& center) { m_tCollisionInfo.m_f3Center = center; }

	// @brief コリジョンのサイズを取得
	// @return (DirectX::XMFLOAT3)コリジョンのサイズ
	DirectX::XMFLOAT3 GetSize() { return m_tCollisionInfo.m_f3HalfSize * 2.0f; }

	// @brief コリジョンのサイズをセット
	// @param inSize：コリジョンのサイズ
	void SetSize(const DirectX::XMFLOAT3& inSize) { m_tCollisionInfo.m_f3HalfSize = inSize / 2.0f; }

	// @brief OBB用パラメータの取得
	// @return OBB用パラメータ
	ObbInfo GetCollisionInfo() { return m_tCollisionInfo; }

	// @brief OBB用パラメータのセット
	// @param inInfo：OBB用パラメータ
	void SetCollisionInfo(ObbInfo inInfo) { m_tCollisionInfo = inInfo; }

private:
	// @brief OBB用パラメータ
	ObbInfo m_tCollisionInfo;

};
