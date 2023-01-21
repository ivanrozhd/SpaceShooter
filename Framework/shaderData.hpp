#pragma once
#include "tga/tga.hpp"
#include "tga/tga_math.hpp"

static constexpr glm::vec3 forward{0., 0., 1.};
static constexpr glm::vec3 right{1., 0., 0.};
static constexpr glm::vec3 up{0., 1., 0.};

/*
    alignment requirements:
    vec3, vec4, mat4, etc: 16 byte
    vec2: 8 byte
    float, int, etc: 4 byte
*/

struct Camera {
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 projection;
    alignas(16) glm::mat4 toWorld;
};

struct Light {
    alignas(16) glm::vec3 position;
    alignas(16) glm::vec3 direction;
    alignas(16) glm::vec4 color;
};

struct SystemData {
    alignas(8) glm::vec2 resolution;
    alignas(4) float time;
};
