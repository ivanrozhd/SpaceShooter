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



/*TODO: Shader Specific Data Here*/


layout(set = 1 , binding = 1) uniform sampler2D  MyTexture;


layout(location = 0) in FragData{
    vec4 clipped_coordinates;
    vec2 uv;
    vec3 normal;
} fragData;



layout(location = 0) out vec4 color;




void main()
{
    vec4 firstData = texture(MyTexture, fragData.uv);
    vec3 changed = vec3(fragData.normal.x, fragData.normal.z, fragData.normal.y);
    color = (0.4 * dot(light.direction,changed) * firstData * light.color) + (0.4 * firstData * light.color);
    // color = (normalize(fragData.normal),1);
    //color = vec4(1,0,0,1);
}