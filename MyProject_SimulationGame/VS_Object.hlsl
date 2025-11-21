struct VS_IN
{
    float3 pos : POSITION;
    float3 normal : NORMAL0;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;
};
struct VS_OUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 wPos : POSITION0;
    float4 screenPos : POSITION1;
    float4 color : COLOR0;
};
cbuffer WVP : register(b0)
{
    float4x4 world;
    float4x4 view;
    float4x4 proj;
};
VS_OUT main(VS_IN vin)
{
    VS_OUT vout;
    vout.pos = float4(vin.pos, 1.0f);
    vout.pos = mul(world, vout.pos);
    vout.wPos = vout.pos.xyz;
    vout.pos = mul(view, vout.pos);
    vout.pos = mul(proj, vout.pos);
    vout.screenPos = vout.pos;
    vout.normal = mul(vin.normal, (float3x3) world);
    vout.uv = vin.uv;
    vout.color = vin.color;
    return vout;
}
