#version 120

uniform mat4 u_modelview;
uniform mat4 u_proj;

// geometry
attribute vec3 a_position;
attribute vec2 a_texcoord;
attribute vec3 a_normal;

// instance
attribute vec3 a_location;
attribute float a_scale;
attribute vec4 a_rotation;
attribute float a_saturation;

// varying vec4 v_color;
varying vec2 v_texcoord;
varying float v_saturation;

vec3 quat_mul(vec4 q, vec3 v) {
  return v + 2.0 * cross(cross(v, q.xyz ) - q.w * v, q.xyz);
}

void main()
{
    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
    gl_Position = u_proj * u_modelview * vec4(quat_mul(a_rotation, a_scale * a_position) + a_location, 1.0);

    /*
    vec3 L = normalize(vec3(1.0, 1.0, 1.0));
    vec3 N = quat_mul(a_rotation, a_normal);
    float lambert = abs(dot(N, L));
    float half_lambert = mix(lambert, 1.0, 0.7);
    */

    // v_color = vec4(1.0, 1.0, 1.0, 1.0);

    v_saturation = a_saturation;
    v_texcoord = a_texcoord;
}
