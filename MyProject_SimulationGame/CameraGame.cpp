#include "CameraGame.h"	
#include "Main.h"
//#include "Player.h"

constexpr float ce_fRadXZ = 0.0f;
constexpr float ce_fRadY = DirectX::XMConvertToRadians(135.0f);
constexpr float ce_fRadius = 10.0f;

CCameraGame::CCameraGame()
{
	m_f3Look = DirectX::XMFLOAT3(0.0f,0.0f,0.0f);
	
	m_f3Pos.x = cosf(ce_fRadY) * sinf(ce_fRadXZ) * ce_fRadius + m_f3Look.x;
	m_f3Pos.y = sinf(ce_fRadY) * ce_fRadius + m_f3Look.y;
	m_f3Pos.z = cosf(ce_fRadY) * cosf(ce_fRadXZ) * ce_fRadius + m_f3Look.z;
}

CCameraGame::~CCameraGame()
{

}

void CCameraGame::Update()
{
	DirectX::XMFLOAT3 f3PlayerPos = DirectX::XMFLOAT3(0.0f,0.0f,0.0f)/*GetScene()->GetGameObject<CPlayer>()->GetPos()*/;
	m_f3Look = f3PlayerPos;

	m_f3Pos.x = cosf(ce_fRadY) * sinf(ce_fRadXZ) * ce_fRadius + m_f3Look.x;
	m_f3Pos.y = sinf(ce_fRadY) * ce_fRadius + m_f3Look.y;
	m_f3Pos.z = cosf(ce_fRadY) * cosf(ce_fRadXZ) * ce_fRadius + m_f3Look.z;
}
