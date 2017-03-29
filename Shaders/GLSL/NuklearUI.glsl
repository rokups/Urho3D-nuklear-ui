#include "Uniforms.glsl"
#include "Samplers.glsl"
#include "Transform.glsl"

varying vec2 vTexCoord;
varying vec4 vColor;

void VS()
{
    gl_Position = iPos * cViewProj;
    vTexCoord = iTexCoord;
    vColor = iColor;
}

void PS()
{
    gl_FragColor = vColor * texture2D(sDiffMap, vTexCoord);
}
