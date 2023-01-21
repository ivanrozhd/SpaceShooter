#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_KHR_vulkan_glsl: enable

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

// NEW
//reserved specifically and doesn't have to be changed
layout(set = 1 , binding = 0) uniform TerrainData {
    vec3 dimensions;
    float terrainHeight;
}heightmap;



// Vertex Shader Inputs
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 vertex_textureCoordinates;



//Vertex Shader output
layout (location = 0) out FragData{
    vec4 clipped_coordinates;
    vec2 uv;
    vec3 normal;
} fragData;


void main()
{
    //https://www.saschawillems.de/blog/2016/08/13/vulkan-tutorial-on-rendering-a-fullscreen-quad-without-buffers/
    //  fragData.uv = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
    //  gl_Position = vec4(fragData.uv * 2.0f + -1.0f, 0.0f, 1.0f);
  //  fragData.uv = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
  //  gl_Position = vec4(fragData.uv * 2.0f + -1.0f, 0.0f, 1.0f);
    fragData.clipped_coordinates =  camera.projection * camera.view *  (vec4(vertex_position * (heightmap.dimensions),1));
    gl_Position = fragData.clipped_coordinates;
    fragData.uv = vertex_textureCoordinates;
    fragData.normal = vertex_normal;



}