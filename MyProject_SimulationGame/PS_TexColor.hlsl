struct PS_IN
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL0;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;
};

cbuffer Color : register(b0)
{
    float4 color;
};

Texture2D tex : register(t0);
SamplerState samp : register(s0);

float4 main(PS_IN pin) : SV_TARGET
{
    float4 texColor = tex.Sample(samp, pin.uv);
    
    texColor.rgba *= color;
    
    return texColor;
}
