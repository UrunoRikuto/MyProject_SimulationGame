#pragma once

#include "Camera.h"

class CCameraDebug : public CCamera
{
public:
	CCameraDebug();
	virtual ~CCameraDebug();
	virtual void Update() final;
private:
	float m_fRadXZ;
	float m_fRadY;
	float m_fRadius;
};