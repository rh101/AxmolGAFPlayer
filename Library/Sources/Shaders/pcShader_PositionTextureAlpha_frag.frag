#version 310 es

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Simple shader with added color transform
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

precision mediump float;

layout(location = COLOR0) in vec4 v_color;
layout(location = TEXCOORD0) in vec2 v_texCoord;

layout(location = SV_Target0) out vec4 FragColor;

layout(binding = 0) uniform sampler2D u_tex0;

layout(std140) uniform fs_ub {
    vec4 colorTransformMult;
    vec4 colorTransformOffsets;
    mat4 colorMatrix;
    vec4 colorMatrix2;
};

void main()
{
    vec4 texColor = texture(u_tex0, v_texCoord);
    
    const float kMinimalAlphaAllowed = 1.0e-8;

    if (texColor.a > kMinimalAlphaAllowed)
    {
        texColor.a = clamp(texColor.a, kMinimalAlphaAllowed, 1.0);
        texColor = vec4(texColor.rgb / texColor.a, texColor.a);

        vec4 ctxColor = texColor * colorTransformMult + colorTransformOffsets;
        vec4 adjustColor = colorMatrix * ctxColor + colorMatrix2;
        adjustColor *= v_color;

        texColor = vec4(adjustColor.rgb * adjustColor.a, adjustColor.a);
    }
    
    FragColor = texColor;
}
