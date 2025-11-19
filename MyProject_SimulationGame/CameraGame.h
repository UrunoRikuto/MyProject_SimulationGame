#pragma once
#include "Camera.h"

class CCameraGame : public CCamera
{
public:
	CCameraGame();
	virtual ~CCameraGame();
	virtual void Update() final;
private:
	float m_fRadXZ;
	float m_fRadY;
	float m_fRadius;
};