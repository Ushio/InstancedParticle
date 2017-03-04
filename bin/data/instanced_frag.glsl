#version 120

uniform sampler2D u_texture;

// varying vec4 v_color;
varying float v_saturation;
varying vec2 v_texcoord;

vec3 czm_saturation(vec3 rgb, float adjustment)
{
    // Algorithm from Chapter 16 of OpenGL Shading Language
    const vec3 W = vec3(0.2125, 0.7154, 0.0721);
    vec3 intensity = vec3(dot(rgb, W));
    return mix(intensity, rgb, adjustment);
}

void main()
{
    vec4 color = texture2D(u_texture, v_texcoord);
    gl_FragColor = vec4(czm_saturation(color.rgb, v_saturation), color.a);
}
