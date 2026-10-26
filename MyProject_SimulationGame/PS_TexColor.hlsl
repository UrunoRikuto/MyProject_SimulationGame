struct PS_IN
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL0;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;
    // オブジェクトごとの乱数(0-1)。季節による色の変化を、オブジェクトごとにばらつかせるために使用
    float objectRand : TEXCOORD1;
};

cbuffer Color : register(b0)
{
    float4 color;
};

// 季節による色の変化(木・草・地面のモデルだけが対象。それ以外は強さが0)
cbuffer Season : register(b1)
{
    // 変化後の色(rgb)
    float4 seasonColor;
    // x:変化の強さ y:変化させるオブジェクトの割合 z:0なら葉の緑の部分だけ 1なら全体 w:未使用
    float4 seasonParam;
};

Texture2D tex : register(t0);
SamplerState samp : register(s0);

float4 main(PS_IN pin) : SV_TARGET
{
    float4 texColor = tex.Sample(samp, pin.uv);

    // 季節による色の変化
    float strength = seasonParam.x;
    if (strength > 0.001f)
    {
        float3 c = texColor.rgb;

        // 葉(緑が強い部分)だけを対象にするためのマスク。幹などの茶色の部分は対象外
        // (テクスチャはsRGBで、暗い葉の緑は線形の値では小さいため、係数を大きくして、葉全体がマスクに入るようにしている)
        float greenMask = saturate((c.g - max(c.r, c.b)) * 30.0f);
        float mask = lerp(greenMask, 1.0f, seasonParam.z);

        // 割合の範囲内の乱数のオブジェクトだけが変化する
        float pick = step(pin.objectRand, seasonParam.y);

        // 元の明るさ(陰影)を残したまま、変化後の色に寄せる
        float lum = dot(c, float3(0.30f, 0.59f, 0.11f));
        float shade = lerp(0.8f, 1.0f, saturate(lum * 5.0f));
        float3 target = saturate(seasonColor.rgb * shade);

        texColor.rgb = lerp(c, target, mask * strength * pick);
    }

    // 頂点の色(地面の起伏の陰影など。色を持たないモデルは白なので変化しない)
    texColor.rgb *= pin.color.rgb;

    texColor.rgba *= color;

    return texColor;
}
