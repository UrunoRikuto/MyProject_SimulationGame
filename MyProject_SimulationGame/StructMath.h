/**********************************************************************************//*
	@file		| StructMath.h
	@brief		| \‘¢‘Ì‚Ì‰‰Z
*//***********************************************************************************/
#pragma once
#include <DirectXMath.h>
#include <math.h>

using namespace DirectX;

// \‘¢‘Ì‚Ì‰‰Z‚Ì–¼‘O‹óŠÔ
namespace StructMath
{
	/*===========================================================
	* XMFLOAT3‚ÆXMFLOAT3‚Ì‰‰Z
	===========================================================*/
	// ‰ÁZ
	XMFLOAT3 Add(XMFLOAT3 t1, XMFLOAT3 t2);
	// Œ¸Z
	XMFLOAT3 Sub(XMFLOAT3 t1, XMFLOAT3 t2);
	// æZ
	XMFLOAT3 Mul(XMFLOAT3 t1, XMFLOAT3 t2);
	// œZ
	XMFLOAT3 Div(XMFLOAT3 t1, XMFLOAT3 t2);
	// ³‹K‰»
	XMFLOAT3 Normalize(XMFLOAT3 t1);
	// ’·‚³
	float Length(XMFLOAT3 t1);
	// â‘Î’l
	XMFLOAT3 Abs(XMFLOAT3 t1);
	// “àÏ
	float Dot(XMFLOAT3 t1, XMFLOAT3 t2);
	// ŠOÏ
	XMFLOAT3 Cross(XMFLOAT3 t1, XMFLOAT3 t2);
	// “™‚µ‚¢
	bool Equal(XMFLOAT3 t1, XMFLOAT3 t2);
	// ‹——£
	float Distance(XMFLOAT3 t1, XMFLOAT3 t2);
	// •ûŒü
	XMFLOAT3 Direction(XMFLOAT3 t1, XMFLOAT3 t2);

	/*===========================================================
	* XMFLOAT2‚ÆXMFLOAT2‚Ì‰‰Z
	===========================================================*/
	// ‰ÁZ
	XMFLOAT2 Add(XMFLOAT2 t1, XMFLOAT2 t2);
	// Œ¸Z
	XMFLOAT2 Sub(XMFLOAT2 t1, XMFLOAT2 t2);
	// æZ
	XMFLOAT2 Mul(XMFLOAT2 t1, XMFLOAT2 t2);
	// œZ
	XMFLOAT2 Div(XMFLOAT2 t1, XMFLOAT2 t2);
	// “™‚µ‚¢
	bool Equal(XMFLOAT2 t1, XMFLOAT2 t2);

	/*===========================================================
	* XMFLOAT3‚Æfloat‚Ì‰‰Z
	===========================================================*/
	// ‰ÁZ
	XMFLOAT3 Add(XMFLOAT3 t1, float f);
	// Œ¸Z
	XMFLOAT3 Sub(XMFLOAT3 t1, float f);
	// æZ
	XMFLOAT3 Mul(XMFLOAT3 t1, float f);
	// œZ
	XMFLOAT3 Div(XMFLOAT3 t1, float f);
	// ˆÈ‰º
	bool LessEqual(XMFLOAT3 t1, float f);
	// ˆÈã
	bool MoreEqual(XMFLOAT3 t1, float f);

	/*===========================================================
	* XMFLOAT2‚Æfloat‚Ì‰‰Z
	===========================================================*/
	// ‰ÁZ
	XMFLOAT2 Add(XMFLOAT2 t1, float f);
	// Œ¸Z
	XMFLOAT2 Sub(XMFLOAT2 t1, float f);
	// æZ
	XMFLOAT2 Mul(XMFLOAT2 t1, float f);
	// œZ
	XMFLOAT2 Div(XMFLOAT2 t1, float f);
	// ˆÈ‰º
	bool LessEqual(XMFLOAT2 t1, float f);
	// ˆÈã
	bool MoreEqual(XMFLOAT2 t1, float f);

	/*===========================================================
	* float‚ÆXMFLOAT‚Ì‰‰Z
	===========================================================*/
	// float‚ğXMFLOAT2‚É•ÏŠ·
	XMFLOAT2 FtoF2(float f);
	// float‚ğXMFLOAT3‚É•ÏŠ·
	XMFLOAT3 FtoF3(float f);
	// float‚ğXMFLOAT4‚É•ÏŠ·
	XMFLOAT4 FtoF4(float f);

	/*===========================================================
	* float‚Ì‰‰Z(d—l‚Ì“ˆê)
	===========================================================*/
	// â‘Î’l
	float Abs(float f);
}

// StructMath‚ÌƒCƒ“ƒ‰ƒCƒ“ƒtƒ@ƒCƒ‹
#include "StructMath.inl"