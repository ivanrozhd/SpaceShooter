#version 450
#extension GL_ARB_separate_shader_objects : enable

const vec3 _forward = vec3(0,0,1);
const vec3 _right = vec3(1,0,0);
const vec3 _up = vec3(0,1,0);

layout(set = 0, binding = 0) uniform Camera{
    mat4 view;
    mat4 projection;
    mat4 toWorld;
} camera;

layout(set = 0, binding = 1) uniform Light{
    vec3 position;
    vec3 direction;
    vec4 color;
} light;

layout(set = 0, binding = 2) uniform System{
    vec2 resolution;
    float time;
} system;



/*TODO: Shader Specific Data Here*/


layout(location = 0) in FragData{
    vec2 uv;
} fragData;



layout(location = 0) out vec4 color;



void main()
{
    color = vec4(.72,.89,1,1);
}