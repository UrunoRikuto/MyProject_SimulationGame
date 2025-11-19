/**********************************************************************************//*
	@file		| StructMath.inl
	@brief		| ç\ë¢ëÃÇÃââéZ
*//***********************************************************************************/
#include "StructMath.h"

/*************************//*
@brief  | XMFLOAT3Ç∆XMFLOAT3ÇÃâ¡éZ
*//*************************/
inline XMFLOAT3 StructMath::Add(XMFLOAT3 t1, XMFLOAT3 t2)
{
	return XMFLOAT3(t1.x + t2.x, t1.y + t2.y, t1.z + t2.z);
}

/*************************//*
@brief  | XMFLOAT3Ç∆XMFLOAT3ÇÃå∏éZ
*//*************************/
inline XMFLOAT3 StructMath::Sub(XMFLOAT3 t1, XMFLOAT3 t2)
{
	return XMFLOAT3(t1.x - t2.x, t1.y - t2.y, t1.z - t2.z);
}

/*************************//*
@brief  | XMFLOAT3Ç∆XMFLOAT3ÇÃèÊéZ
*//*************************/
inline XMFLOAT3 StructMath::Mul(XMFLOAT3 t1, XMFLOAT3 t2)
{
	return XMFLOAT3(t1.x * t2.x, t1.y * t2.y, t1.z * t2.z);
}

/*************************//*
@brief  | XMFLOAT3Ç∆XMFLOAT3ÇÃèúéZ
*//*************************/
inline XMFLOAT3 StructMath::Div(XMFLOAT3 t1, XMFLOAT3 t2)
{
	return XMFLOAT3(t1.x / t2.x, t1.y / t2.y, t1.z / t2.z);
}

/*************************//*
@brief  | XMFLOAT3Ç∆XMFLOAT3ÇÃê≥ãKâª
*//*************************/
inline XMFLOAT3 StructMath::Normalize(XMFLOAT3 t1)
{
	float length = sqrtf(t1.x * t1.x + t1.y * t1.y + t1.z * t1.z);
	if (length == 0.0f) return XMFLOAT3(0.0f, 0.0f, 0.0f);
	return XMFLOAT3(t1.x / length, t1.y / length, t1.z / length);
}

/*************************//*
@brief  | XMFLOAT3ÇÃí∑Ç≥
*//*************************/
inline float StructMath::Length(XMFLOAT3 t1)
{
	return sqrtf(t1.x * t1.x + t1.y * t1.y + t1.z * t1.z);
}

/*************************//*
@brief  | XMFLOAT3ÇÃê‚ëŒíl
*//*************************/
inline XMFLOAT3 StructMath::Abs(XMFLOAT3 t1)
{
	return XMFLOAT3(fabsf(t1.x), fabsf(t1.y), fabsf(t1.z));
}

/*************************//*
@brief  | XMFLOAT3Ç∆XMFLOAT3ÇÃì‡êœ
*//*************************/
inline float StructMath::Dot(XMFLOAT3 t1, XMFLOAT3 t2)
{
	return t1.x * t2.x + t1.y * t2.y + t1.z * t2.z;
}

/*************************//*
@brief  | XMFLOAT3Ç∆XMFLOAT3ÇÃäOêœ
*//*************************/
inline XMFLOAT3 StructMath::Cross(XMFLOAT3 t1, XMFLOAT3 t2)
{
	return XMFLOAT3(
		t1.y * t2.z - t1.z * t2.y,
		t1.z * t2.x - t1.x * t2.z,
		t1.x * t2.y - t1.y * t2.x
	);
}

/*************************//*
@brief  | XMFLOAT3Ç∆XMFLOAT3ÇÃìôÇµÇ¢
*//*************************/
inline bool StructMath::Equal(XMFLOAT3 t1, XMFLOAT3 t2)
{
	return t1.x == t2.x && t1.y == t2.y && t1.z == t2.z;
}

/*************************//*
@brief  | XMFLOAT3Ç∆XMFLOAT3ÇÃãóó£
*//*************************/
inline float StructMath::Distance(XMFLOAT3 t1, XMFLOAT3 t2)
{
	return sqrtf((t1.x - t2.x) * (t1.x - t2.x) + (t1.y - t2.y) * (t1.y - t2.y) + (t1.z - t2.z) * (t1.z - t2.z));
}

/*************************//*
@brief  | XMFLOAT3Ç∆XMFLOAT3ÇÃï˚å¸
*//*************************/
inline XMFLOAT3 StructMath::Direction(XMFLOAT3 t1, XMFLOAT3 t2)
{
	XMFLOAT3 dir = Sub(t2, t1);
	return Normalize(dir);
}

/*************************//*
@brief  | XMFLOAT2Ç∆XMFLOAT2ÇÃâ¡éZ
*//*************************/
inline XMFLOAT2 StructMath::Add(XMFLOAT2 t1, XMFLOAT2 t2)
{
	return XMFLOAT2(t1.x + t2.x, t1.y + t2.y);
}

/*************************//*
@brief  | XMFLOAT2Ç∆XMFLOAT2ÇÃå∏éZ
*//*************************/
inline XMFLOAT2 StructMath::Sub(XMFLOAT2 t1, XMFLOAT2 t2)
{
	return XMFLOAT2(t1.x - t2.x, t1.y - t2.y);
}

/*************************//*
@brief  | XMFLOAT2Ç∆XMFLOAT2ÇÃèÊéZ
*//*************************/
inline XMFLOAT2 StructMath::Mul(XMFLOAT2 t1, XMFLOAT2 t2)
{
	return XMFLOAT2(t1.x * t2.x, t1.y * t2.y);
}

/*************************//*
@brief  | XMFLOAT2Ç∆XMFLOAT2ÇÃèúéZ
*//*************************/
inline XMFLOAT2 StructMath::Div(XMFLOAT2 t1, XMFLOAT2 t2)
{
	return XMFLOAT2(t1.x / t2.x, t1.y / t2.y);
}

/*************************//*
@brief  | XMFLOAT2Ç∆XMFLOAT2ÇÃìôÇµÇ¢
*//*************************/
inline bool StructMath::Equal(XMFLOAT2 t1, XMFLOAT2 t2)
{
	return t1.x == t2.x && t1.y == t2.y;
}


/*************************//*
@brief  | XMFLOAT3Ç∆floatÇÃâ¡éZ
*//*************************/
inline XMFLOAT3 StructMath::Add(XMFLOAT3 t1, float f)
{
	return XMFLOAT3(t1.x + f, t1.y + f, t1.z + f);
}

/*************************//*
@brief  | XMFLOAT3Ç∆floatÇÃå∏éZ
*//*************************/
inline XMFLOAT3 StructMath::Sub(XMFLOAT3 t1, float f)
{
	return XMFLOAT3(t1.x - f, t1.y - f, t1.z - f);
}

/*************************//*
@brief  | XMFLOAT3Ç∆floatÇÃèÊéZ
*//*************************/
inline XMFLOAT3 StructMath::Mul(XMFLOAT3 t1, float f)
{
	return XMFLOAT3(t1.x * f, t1.y * f, t1.z * f);
}

/*************************//*
@brief  | XMFLOAT3Ç∆floatÇÃèúéZ
*//*************************/
inline XMFLOAT3 StructMath::Div(XMFLOAT3 t1, float f)
{
	return XMFLOAT3(t1.x / f, t1.y / f, t1.z / f);
}

/*************************//*
@brief  | XMFLOAT3Ç∆floatÇÃà»â∫
*//*************************/
inline bool StructMath::LessEqual(XMFLOAT3 t1, float f)
{
	return t1.x <= f && t1.y <= f && t1.z <= f;
}

/*************************//*
@brief  | XMFLOAT3Ç∆floatÇÃà»è„
*//*************************/
inline bool StructMath::MoreEqual(XMFLOAT3 t1, float f)
{
	return t1.x >= f && t1.y >= f && t1.z >= f;
}


/*************************//*
@brief  | XMFLOAT2Ç∆floatÇÃâ¡éZ
*//*************************/
inline XMFLOAT2 StructMath::Add(XMFLOAT2 t1, float f)
{
	return XMFLOAT2(t1.x + f, t1.y + f);
}

/*************************//*
@brief  | XMFLOAT2Ç∆floatÇÃå∏éZ
*//*************************/
inline XMFLOAT2 StructMath::Sub(XMFLOAT2 t1, float f)
{
	return XMFLOAT2(t1.x - f, t1.y - f);
}

/*************************//*
@brief  | XMFLOAT2Ç∆floatÇÃèÊéZ
*//*************************/
inline XMFLOAT2 StructMath::Mul(XMFLOAT2 t1, float f)
{
	return XMFLOAT2(t1.x * f, t1.y * f);
}

/*************************//*
@brief  | XMFLOAT2Ç∆floatÇÃèúéZ
*//*************************/
inline XMFLOAT2 StructMath::Div(XMFLOAT2 t1, float f)
{
	return XMFLOAT2(t1.x / f, t1.y / f);
}

/*************************//*
@brief  | XMFLOAT2Ç∆floatÇÃà»â∫
*//*************************/
inline bool StructMath::LessEqual(XMFLOAT2 t1, float f)
{
	return t1.x <= f && t1.y <= f;
}

/*************************//*
@brief  | XMFLOAT2Ç∆floatÇÃà»è„
*//*************************/
inline bool StructMath::MoreEqual(XMFLOAT2 t1, float f)
{
	return t1.x >= f && t1.y >= f;
}


/*************************//*
@brief  | floatÇXMFLOAT2Ç…ïœä∑
*//*************************/
inline XMFLOAT2 StructMath::FtoF2(float f)
{
	return XMFLOAT2(f, f);
}

/*************************//*
@brief  | floatÇXMFLOAT3Ç…ïœä∑
*//*************************/
inline XMFLOAT3 StructMath::FtoF3(float f)
{
	return XMFLOAT3(f, f, f);
}

/*************************//*
@brief  | floatÇXMFLOAT4Ç…ïœä∑
*//*************************/
inline XMFLOAT4 StructMath::FtoF4(float f)
{
	return XMFLOAT4(f, f, f, f);
}

/*************************//*
@brief  | floatÇÃê‚ëŒíl
*//*************************/
inline float StructMath::Abs(float f)
{
	return fabsf(f);
}
