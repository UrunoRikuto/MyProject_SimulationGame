struct VS_IN
{
    float3 pos : POSITION;
    float3 normal : NORMAL0;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;

    // インスタンス用ワールド行列（行ごと）
    float4 INSTANCE0 : INSTANCE0;
    float4 INSTANCE1 : INSTANCE1;
    float4 INSTANCE2 : INSTANCE2;
    float4 INSTANCE3 : INSTANCE3;
};
struct VS_OUT
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL0;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;
    float4 wPos : POSITION0;
};
cbuffer WVP : register(b0)
{
    float4x4 world;
    float4x4 view;
    float4x4 proj;
    float useInstance; //0: use cbuffer world,1: use per-instance world
    float3 pad;
};
VS_OUT main(VS_IN vin)
{
    VS_OUT vout;

    // 決定するワールド行列
    float4x4 wMat = world;
    if (useInstance >0.5f)
    {
        // インスタンス行列を構築（行ベース）
        wMat = float4x4(vin.INSTANCE0, vin.INSTANCE1, vin.INSTANCE2, vin.INSTANCE3);
    }

    vout.pos = float4(vin.pos,1.0f);
    vout.pos = mul(vout.pos, wMat);
    vout.wPos = vout.pos;
    vout.pos = mul(vout.pos, view);
    vout.pos = mul(vout.pos, proj);
    vout.normal = mul(vin.normal, (float3x3) wMat);
    vout.uv = vin.uv;
    vout.color = vin.color;
    return vout;
}
