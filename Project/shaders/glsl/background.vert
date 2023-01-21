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


layout (location = 0) out FragData{
    vec2 uv;
} fragData;
   

void main() 
{
    //https://www.saschawillems.de/blog/2016/08/13/vulkan-tutorial-on-rendering-a-fullscreen-quad-without-buffers/
    fragData.uv = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
    gl_Position = vec4(fragData.uv * 2.0f + -1.0f, 0.0f, 1.0f);
}