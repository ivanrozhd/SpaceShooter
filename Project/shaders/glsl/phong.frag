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

layout(set = 1 , binding = 0) uniform sampler2D MyTexture_dif;
layout (set = 1, binding = 1) uniform sampler2D MyTexture_em;
layout(set = 1, binding = 2) uniform sampler2D MyTexture_spec;
layout(set = 1, binding = 3) uniform CamController {
    vec3 position;
} camController;

layout(location = 0) in FragData{
    vec4 world_position;
    vec2 uv;
    vec3 normal;
    vec3 tangent;
} fragData;

layout(location = 0) out vec4 color;

void main()
{

    vec4 objectColor_dif = texture(MyTexture_dif, fragData.uv);
    vec4 objectColor_em = texture(MyTexture_em, fragData.uv);
    vec4 objectColor_spec = texture(MyTexture_spec, fragData.uv);
    vec3 normal = normalize(vec3(fragData.normal.x, fragData.normal.z, fragData.normal.y));
    vec3 lightDir = normalize(light.direction);
    vec4 reflectDir = vec4(reflect(-lightDir, normal), 1.0);
    vec4 viewDir = normalize(vec4(camController.position, 1.0) - fragData.world_position); // or ragData.world_position - vec4(camController.position, 1)???
    vec4 ambient = 0.1 * light.color;
    vec4 em = 0.1 * objectColor_em;
    float spec = clamp(pow(max(0.0, dot(viewDir, reflectDir)), 32), 0, 1);
    // diffuse light
    float diff = clamp(dot(normal, lightDir), 0, 1);
    vec4 diffuse = 0.7 * diff * objectColor_dif * light.color;
    vec4 specular = (0.2 * spec * objectColor_spec * light.color);
    float distance = length((camController.position,1) - fragData.world_position);
    color =  (diffuse +  specular + em);
    vec4 mixx = mix(color,vec4(.72,.89,1,1), distance/1000);
    if (distance <= 150) {
        color = mixx;
    }
    else
    {  color = vec4(.72,.89,1,1); }

}