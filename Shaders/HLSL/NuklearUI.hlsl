#include "Uniforms.hlsl"
#include "Samplers.hlsl"
#include "Transform.hlsl"

void VS(float4 iPos : POSITION,
    float2 iTexCoord : TEXCOORD0,
    float4 iColor : COLOR0,
    out float4 oColor : COLOR0,
    out float2 oTexCoord : TEXCOORD0,
    out float4 oPos : OUTPOSITION)
{
    oPos = mul(iPos, cViewProj);

    oColor = iColor;
    oTexCoord = iTexCoord;
}

void PS(float4 iColor : COLOR0,
        float2 iTexCoord : TEXCOORD0,
        out float4 oColor : OUTCOLOR0)
{
    oColor = iColor * Sample2D(DiffMap, iTexCoord);
}
