/**********************************************************************************//*
	@file		| Geometory.cpp
	@brief		| 3D図形描画クラス実装
*//***********************************************************************************/
#include "Geometory.h"

MeshBuffer* Geometory::m_pBox;
MeshBuffer* Geometory::m_pCylinder;
MeshBuffer* Geometory::m_pSphere;
MeshBuffer* Geometory::m_pLines;
Shader* Geometory::m_pVS;
Shader* Geometory::m_pPS;
Shader* Geometory::m_pLineShader[2];
DirectX::XMFLOAT4X4 Geometory::m_WVP[3];
void* Geometory::m_pLineVtx;
int Geometory::m_lineCnt = 0;

/*************************//*
@brief  | 初期化
*//*************************/
void Geometory::Init()
{
	for (int i = 0; i < 3; ++i)
		DirectX::XMStoreFloat4x4(&m_WVP[i], DirectX::XMMatrixIdentity());
	//MakeBox();
	//MakeCylinder();
	//MakeSphere();
	MakeVS();
	MakePS();
	MakeLineShader();
	MakeLine();
}

/*************************//*
@brief  | 終了
*//*************************/
void Geometory::Uninit()
{
	SAFE_DELETE_ARRAY(m_pLineVtx);
	SAFE_DELETE(m_pLineShader[1]);
	SAFE_DELETE(m_pLineShader[0]);
	SAFE_DELETE(m_pPS);
	SAFE_DELETE(m_pVS);
	SAFE_DELETE(m_pLines);
	SAFE_DELETE(m_pSphere);
	SAFE_DELETE(m_pCylinder);
	SAFE_DELETE(m_pBox);
}

/*************************//*
@brief		| ワールド行列セット
@param[in]	| world：ワールド行列
*//*************************/
void Geometory::SetWorld(DirectX::XMFLOAT4X4 world)
{
	m_WVP[0] = world;
}

/*************************//*
@brief		| ビュー行列セット
@param[in]  | view：ビュー行列
*//*************************/
void Geometory::SetView(DirectX::XMFLOAT4X4 view)
{
	m_WVP[1] = view;
}

/*************************//*
@brief		| プロジェクション行列セット
@param[in]  | proj：射影行列
*//*************************/
void Geometory::SetProjection(DirectX::XMFLOAT4X4 proj)
{
	m_WVP[2] = proj;
}

/*************************//*
@brief		| 線分追加
@param[in]  | start：線分の始点
@param[in]  | end：線分の終点
@param[in]  | color：線分の色
*//*************************/
void Geometory::AddLine(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end, DirectX::XMFLOAT4 color)
{
	if (m_lineCnt < MAX_LINE_NUM)
	{
		LineVertex* pVtx = reinterpret_cast<LineVertex*>(m_pLineVtx);
		pVtx[m_lineCnt * 2 + 0] = { start.x, start.y, start.z, color.x, color.y, color.z, color.w};
		pVtx[m_lineCnt * 2 + 1] = { end.x, end.y, end.z, color.x, color.y, color.z, color.w };
		++m_lineCnt;
	}
}

/*************************//*
@brief	| 線分描画
*//*************************/
void Geometory::DrawLines()
{
	m_pLineShader[0]->WriteBuffer(0, m_WVP);
	m_pLineShader[0]->Bind();
	m_pLineShader[1]->Bind();
	m_pLines->Write(m_pLineVtx);
	m_pLines->Draw(m_lineCnt * 2);
	m_lineCnt = 0;
}

/*************************//*
@brief  | 箱描画
*//*************************/
void Geometory::DrawBox()
{
	if (m_pBox == nullptr)
		return;
	m_pVS->WriteBuffer(0, m_WVP);
	m_pVS->Bind();
	m_pPS->Bind();
	m_pBox->Draw();
}

/*************************//*
@brief  | 箱描画(ワイヤーフレーム)
@param[in]  | In_vCenter：箱の中心座標
@param[in]  | In_vSize：箱のサイズ
@param[in]  | In_Color：箱の色
*//*************************/
void Geometory::DrawBox(DirectX::XMFLOAT3 In_vCenter, DirectX::XMFLOAT3 In_vSize, DirectX::XMFLOAT4 In_Color)
{
	// 頂点データ作成
	DirectX::XMFLOAT3 halfSize = { In_vSize.x / 2
		, In_vSize.y / 2
		, In_vSize.z / 2 };

	// 8頂点分
	DirectX::XMFLOAT3 vertex[8] = {
		{In_vCenter.x - halfSize.x, In_vCenter.y + halfSize.y
		, In_vCenter.z - halfSize.z }, // 0
		{In_vCenter.x - halfSize.x, In_vCenter.y - halfSize.y
		, In_vCenter.z - halfSize.z}, // 1
		{In_vCenter.x + halfSize.x, In_vCenter.y - halfSize.y
		, In_vCenter.z - halfSize.z}, // 2
		{In_vCenter.x + halfSize.x, In_vCenter.y + halfSize.y
		, In_vCenter.z - halfSize.z}, // 3
		{In_vCenter.x - halfSize.x, In_vCenter.y + halfSize.y
		, In_vCenter.z + halfSize.z}, // 4
		{In_vCenter.x - halfSize.x, In_vCenter.y - halfSize.y
		, In_vCenter.z + halfSize.z}, // 5
		{In_vCenter.x + halfSize.x, In_vCenter.y - halfSize.y
		, In_vCenter.z + halfSize.z}, // 6
		{In_vCenter.x + halfSize.x, In_vCenter.y + halfSize.y
		, In_vCenter.z + halfSize.z}, // 7
	};

	// 前面
	AddLine(vertex[0], vertex[1], In_Color);
	AddLine(vertex[1], vertex[2], In_Color);
	AddLine(vertex[2], vertex[3], In_Color);
	AddLine(vertex[3], vertex[0], In_Color);
	// 背面
	AddLine(vertex[4], vertex[5], In_Color);
	AddLine(vertex[5], vertex[6], In_Color);
	AddLine(vertex[6], vertex[7], In_Color);
	AddLine(vertex[7], vertex[4], In_Color);
	// 左側面
	AddLine(vertex[0], vertex[4], In_Color);
	AddLine(vertex[1], vertex[5], In_Color);
	// 右側面
	AddLine(vertex[2], vertex[6], In_Color);
	AddLine(vertex[3], vertex[7], In_Color);
	// 上面
	AddLine(vertex[0], vertex[3], In_Color);
	AddLine(vertex[4], vertex[7], In_Color);
	// 下面
	AddLine(vertex[1], vertex[2], In_Color);
	AddLine(vertex[5], vertex[6], In_Color);
	// 描画
	DrawLines();



}

/*************************//*
@brief		| 板描画
@param[in]  | In_vCenter：板の中心座標
@param[in]  | In_vSize：板のサイズ
@param[in]  | In_vDir：板の法線方向
@param[in]  | In_Color：板の色
*//*************************/
void Geometory::DrawPlane(DirectX::XMFLOAT3 In_vCenter, DirectX::XMFLOAT3 In_vSize, DirectX::XMFLOAT3 In_vDir, DirectX::XMFLOAT4 In_Color)
{
	// カリング設定
	SetCullingMode(D3D11_CULL_BACK);

	DirectX::XMFLOAT3 halfSize = { In_vSize.x / 2
		, In_vSize.y / 2
		, In_vSize.z / 2 };

	if(In_vDir.y != 0.0f)
	{
		// Y軸方向
		DirectX::XMFLOAT3 vertex[4] = {
			{In_vCenter.x - halfSize.x, In_vCenter.y
			, In_vCenter.z + halfSize.z }, // 0
			{In_vCenter.x - halfSize.x, In_vCenter.y
			, In_vCenter.z - halfSize.z}, // 1
			{In_vCenter.x + halfSize.x, In_vCenter.y
			, In_vCenter.z - halfSize.z}, // 2
			{In_vCenter.x + halfSize.x, In_vCenter.y
			, In_vCenter.z + halfSize.z}, // 3
		};
		AddLine(vertex[0], vertex[1], In_Color);
		AddLine(vertex[1], vertex[2], In_Color);
		AddLine(vertex[2], vertex[3], In_Color);
		AddLine(vertex[3], vertex[0], In_Color);
		AddLine(vertex[0], vertex[2], In_Color);
		AddLine(vertex[1], vertex[3], In_Color);
		DrawLines();
		return;
	}

	DirectX::XMFLOAT3 vertex[4] = {
		{In_vCenter.x - halfSize.x, In_vCenter.y + halfSize.y
		, In_vCenter.z }, // 0
		{In_vCenter.x - halfSize.x, In_vCenter.y - halfSize.y
		, In_vCenter.z}, // 1
		{In_vCenter.x + halfSize.x, In_vCenter.y - halfSize.y
		, In_vCenter.z}, // 2
		{In_vCenter.x + halfSize.x, In_vCenter.y + halfSize.y
		, In_vCenter.z}, // 3
	};
	AddLine(vertex[0], vertex[1], In_Color);
	AddLine(vertex[1], vertex[2], In_Color);
	AddLine(vertex[2], vertex[3], In_Color);
	AddLine(vertex[3], vertex[0], In_Color);
	AddLine(vertex[0], vertex[2], In_Color);
	AddLine(vertex[1], vertex[3], In_Color);
	DrawLines();
}

/*************************//*
@brief  | 円柱描画
*//*************************/
void Geometory::DrawCylinder()
{
	if (m_pCylinder == nullptr)
		return;
	m_pVS->WriteBuffer(0, m_WVP);
	m_pVS->Bind();
	m_pPS->Bind();
	m_pCylinder->Draw();
}

/*************************//*
@brief  | 球描画
*//*************************/
void Geometory::DrawSphere()
{
	if (m_pSphere == nullptr)
		return;
	m_pVS->WriteBuffer(0, m_WVP);
	m_pVS->Bind();
	m_pPS->Bind();
	m_pSphere->Draw();
}

/*************************//*
@brief		| 水平四角形描画
@param[in]	| tPoint1：四角形を囲む2点のうちの1点
@param[in]	| tPoint2：四角形を囲む2点のうちの1点
@param[in]	| tColor：四角形の色
*//*************************/
void Geometory::DrawHorizontalSquare(DirectX::XMFLOAT3 tPoint1, DirectX::XMFLOAT3 tPoint2, DirectX::XMFLOAT4 tColor)
{
	DirectX::XMFLOAT3 tMaxPoint = { max(tPoint1.x, tPoint2.x), max(tPoint1.y, tPoint2.y), max(tPoint1.z, tPoint2.z) };
	DirectX::XMFLOAT3 tMinPoint = { min(tPoint1.x, tPoint2.x), min(tPoint1.y, tPoint2.y), min(tPoint1.z, tPoint2.z) };

	AddLine(tMaxPoint, { tMinPoint.x ,tMaxPoint.y,tMaxPoint.z }, { 1.0f,0.0f,0.0f,1.0f });
	AddLine(tMaxPoint, { tMaxPoint.x ,tMaxPoint.y,tMinPoint.z }, { 1.0f,0.0f,0.0f,1.0f });

	AddLine(tMinPoint, { tMinPoint.x ,tMinPoint.y,tMaxPoint.z }, { 1.0f,0.0f,0.0f,1.0f });
	AddLine(tMinPoint, { tMaxPoint.x ,tMinPoint.y,tMinPoint.z }, { 1.0f,0.0f,0.0f,1.0f });

	DrawLines();

}

/*************************//*
@brief		| 垂直X四角形描画
@param[in]	| tPoint1：四角形を囲む2点のうちの1点
@param[in]	| tPoint2：四角形を囲む2点のうちの1点
@param[in]	| tColor：四角形の色
*//*************************/
void Geometory::DrawVerticalXSquare(DirectX::XMFLOAT3 tPoint1, DirectX::XMFLOAT3 tPoint2, DirectX::XMFLOAT4 tColor)
{
	DirectX::XMFLOAT3 tMaxPoint = { max(tPoint1.x, tPoint2.x), max(tPoint1.y, tPoint2.y), max(tPoint1.z, tPoint2.z) };
	DirectX::XMFLOAT3 tMinPoint = { min(tPoint1.x, tPoint2.x), min(tPoint1.y, tPoint2.y), min(tPoint1.z, tPoint2.z) };

	AddLine(tMaxPoint, { tMaxPoint.x ,tMinPoint.y,tMaxPoint.z }, { 1.0f,0.0f,0.0f,1.0f });
	AddLine(tMinPoint, { tMinPoint.x ,tMaxPoint.y,tMinPoint.z }, { 1.0f,0.0f,0.0f,1.0f });

	AddLine(tMaxPoint, { tMaxPoint.x ,tMaxPoint.y,tMinPoint.z }, { 1.0f,0.0f,0.0f,1.0f });
	AddLine(tMinPoint, { tMinPoint.x ,tMinPoint.y,tMaxPoint.z }, { 1.0f,0.0f,0.0f,1.0f });

	DrawLines();
}

/*************************//*
@brief		| 垂直Z四角形描画
@param[in]	| tPoint1：四角形を囲む2点のうちの1点
@param[in]	| tPoint2：四角形を囲む2点のうちの1点
@param[in]	| tColor：四角形の色
*//*************************/
void Geometory::DrawVerticalZSquare(DirectX::XMFLOAT3 tPoint1, DirectX::XMFLOAT3 tPoint2, DirectX::XMFLOAT4 tColor)
{
	DirectX::XMFLOAT3 tMaxPoint = { max(tPoint1.x, tPoint2.x), max(tPoint1.y, tPoint2.y), max(tPoint1.z, tPoint2.z) };
	DirectX::XMFLOAT3 tMinPoint = { min(tPoint1.x, tPoint2.x), min(tPoint1.y, tPoint2.y), min(tPoint1.z, tPoint2.z) };

	AddLine(tMaxPoint, { tMaxPoint.x ,tMinPoint.y,tMaxPoint.z }, { 1.0f,0.0f,0.0f,1.0f });
	AddLine(tMinPoint, { tMinPoint.x ,tMaxPoint.y,tMinPoint.z }, { 1.0f,0.0f,0.0f,1.0f });
	 
	AddLine(tMaxPoint, { tMinPoint.x ,tMaxPoint.y,tMaxPoint.z }, { 1.0f,0.0f,0.0f,1.0f });
	AddLine(tMinPoint, { tMaxPoint.x ,tMinPoint.y,tMinPoint.z }, { 1.0f,0.0f,0.0f,1.0f });

	DrawLines();
}

/*************************//*
@brief	| 頂点シェーダ作成
*//*************************/
void Geometory::MakeVS()
{
const char* VSCode = R"EOT(
struct VS_IN {
	float3 pos : POSITION0;
	float2 uv : TEXCOORD0;
};
struct VS_OUT {
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
};
cbuffer Matrix : register(b0) {
	float4x4 world;
	float4x4 view;
	float4x4 proj;
};
VS_OUT main(VS_IN vin) {
	VS_OUT vout;
	vout.pos = float4(vin.pos, 1.0f);
	vout.pos = mul(vout.pos, world);
	vout.pos = mul(vout.pos, view);
	vout.pos = mul(vout.pos, proj);
	vout.uv = vin.uv;
	return vout;
})EOT";

	m_pVS = new VertexShader();
	m_pVS->Compile(VSCode);
}

/*************************//*
@brief  | ピクセルシェーダ作成
*//*************************/
void Geometory::MakePS()
{
#ifdef _DEBUG
	const char* PSCode = R"EOT(
struct PS_IN {
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
};
float4 main(PS_IN pin) : SV_TARGET0 {
	float4 color = float4(1,1,1,1);
	float2 halfGrid = floor(abs(pin.uv) * 2.0f);
	float2 quatGrid = floor(abs(pin.uv) * 8.0f);

	float half = fmod(halfGrid.x + halfGrid.y, 2.0f);
	float quat = fmod(quatGrid.x + quatGrid.y, 2.0f);

	color.rgb = ((half * 0.1f) * quat + 0.45f) + (1 - quat) * 0.05f;
	return color;
})EOT";
#else
	const char* PSCode = R"EOT(
struct PS_IN {
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
};
float4 main(PS_IN pin) : SV_TARGET0 {
	return float4(1.0f, 0.0f, 1.0f, 1.0f);
})EOT";
#endif

	m_pPS = new PixelShader();
	m_pPS->Compile(PSCode);
}

/*************************//*
@brief  | 線分描画シェーダ作成
*//*************************/
void Geometory::MakeLineShader()
{
	const char* VSCode = R"EOT(
struct VS_IN {
	float3 pos : POSITION0;
	float4 color : COLOR0;
};
struct VS_OUT {
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
};
cbuffer Matrix : register(b0) {
	float4x4 world;
	float4x4 view;
	float4x4 proj;
};
VS_OUT main(VS_IN vin) {
	VS_OUT vout;
	vout.pos = float4(vin.pos, 1.0f);
	vout.pos = mul(vout.pos, view);
	vout.pos = mul(vout.pos, proj);
	vout.color = vin.color;
	return vout;
})EOT";

	const char* PSCode = R"EOT(
struct PS_IN {
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
};
float4 main(PS_IN pin) : SV_TARGET0 {
	return pin.color;
})EOT";

	m_pLineShader[0] = new VertexShader();
	m_pLineShader[0]->Compile(VSCode);
	m_pLineShader[1] = new PixelShader();
	m_pLineShader[1]->Compile(PSCode);
}

/*************************//*
@brief  | 線分描画用メッシュ作成
*//*************************/
void Geometory::MakeLine()
{
	m_pLineVtx = new LineVertex[MAX_LINE_NUM * 2];
	m_lineCnt = 0;
	MeshBuffer::Description desc = {};
	desc.pVtx = m_pLineVtx;
	desc.vtxCount = MAX_LINE_NUM * 2;
	desc.vtxSize = sizeof(LineVertex);
	desc.topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	desc.isWrite = true;
	m_pLines = new MeshBuffer();
	m_pLines->Create(desc);
}