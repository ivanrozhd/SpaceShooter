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

layout(set = 1, binding = 4) uniform Translation{
    mat4 transform;
} translation;


/*TODO: Shader Specific Data Here*/
// Vertex Shader Inputs
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 vertex_textureCoordinates;
layout(location = 2) in vec3 vertex_normal;
layout(location = 3) in vec3 vertex_tangent;



layout (location = 0) out FragData{
    vec4 world_position;
    vec2 uv;
    vec3 normal;
    vec3 tangent;
} fragData;

void main() {

    mat4 scale = mat4(0.05, 0, 0, 0,
    0, 0.05, 0, 0,
    0, 0, 0.05, 0,
    0, 0, 0, 1);

//    mat4 translate = mat4(1, 0, 0, 0,
//    0, 1, 0, 0,
//    0, 0, 1, 0,
//    0, -0.8, -1.6, 1);


    gl_Position = fragData.world_position = camera.projection * (((translation.transform * scale)) * vec4(vertex_position, 1));
    fragData.uv = vertex_textureCoordinates;
    fragData.normal = vertex_normal;
    fragData.tangent = vertex_tangent;

}