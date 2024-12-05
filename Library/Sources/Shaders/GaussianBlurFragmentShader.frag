#version 310 es

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Gaussian blur
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

precision highp float;
precision highp int;

layout(location = COLOR0) in vec4 v_color;
layout(location = TEXCOORD0) in vec2 v_texCoord;

layout(location = SV_Target0) out vec4 FragColor;

layout(binding = 0) uniform sampler2D u_tex0;

layout(std140) uniform fs_ub {
    vec2 u_step;
};

void main()
{
    vec4 sum = vec4(0.0);

    sum += texture(u_tex0, v_texCoord - u_step * 4.0) * 0.05;
    sum += texture(u_tex0, v_texCoord - u_step * 3.0) * 0.09;
    sum += texture(u_tex0, v_texCoord - u_step * 2.0) * 0.12;
    sum += texture(u_tex0, v_texCoord - u_step * 1.0) * 0.15;
    sum += texture(u_tex0, v_texCoord + u_step * 0.0) * 0.18;
    sum += texture(u_tex0, v_texCoord + u_step * 1.0) * 0.15;
    sum += texture(u_tex0, v_texCoord + u_step * 2.0) * 0.12;
    sum += texture(u_tex0, v_texCoord + u_step * 3.0) * 0.09;
    sum += texture(u_tex0, v_texCoord + u_step * 4.0) * 0.05;
    
    FragColor = sum;
}

